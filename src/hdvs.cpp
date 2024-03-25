#include "HDVS/hdvs.h"

#include <QTimer>
#include <QThread>
#include <yaml-cpp/yaml.h>
#include <fstream>

#include <windows.h>
#include <winuser.h>
// Windows Error macro collides with status enum
#ifdef ERROR
#undef ERROR
#endif

#define LOG(msg) emit SendLog(msg)
#define PHASE(phase) emit PhaseChange(hdvs::Status::Phase::phase)

namespace hdvs {

HDVS::HDVS(QObject* parent):
    QObject(parent)
{
    QTimer::singleShot(50, this, SLOT(PostInit()));
}

HDVS::~HDVS() {
    delete m_audio;
    whisper_free(m_wctx);
}

void HDVS::UpdateStratagems(const QList<hdvs::Stratagem>& stratagems)
{
    YAML::Node ymlOut;
    m_stratagems.clear();

    for (int i = 0; i < stratagems.size(); i++)
    {
        Stratagem stratagem = stratagems[i];

        m_stratagems.push_back(stratagem);
        ymlOut.push_back(stratagem);
    }

    LOG("Recieved stratagem data writing...");

    std::ofstream fout(STRAT_PATH);
    fout << ymlOut;

    if (fout.bad())
    {
        LOG("Error whilst writing to '" STRAT_PATH "'");
        PHASE(ERROR);
    }

    SetTriggerWords();

    fout.close();
}

void HDVS::ForceExit() {
    m_running = false;
}

void HDVS::PostInit()
{
    LOG("Beginning startup");

    LOG("Loading configuration...");
    try {
        YAML::Node config = YAML::LoadFile(CONFIG_PATH);
        m_config = config.as<Config>();
    }
    catch (std::runtime_error& e) {
        LOG(QString("Error whilst loading '" CONFIG_PATH "': ") + e.what());
        PHASE(ERROR);
        return;
    }
    LOG("Configuration loaded");

    LOG("Loading stratagems...");
    try {
        YAML::Node stratagems = YAML::LoadFile(STRAT_PATH);

        // could just emit as list
        for (size_t i = 0; i < stratagems.size(); i++)
        {
            Stratagem strat = stratagems[i].as<Stratagem>();
            m_stratagems.push_back(strat);
            emit LoadStratagem(strat);
        }
        
    }
    catch (std::runtime_error& e) {
        LOG(QString("Error whilst loading '" STRAT_PATH "': ") + e.what());
        PHASE(ERROR);
        return;
    }
    LOG(QString::number(m_stratagems.size()) +  " Stratagems loaded");

    LOG("Initializing audio...");
    m_audio = new util::audio_async(m_config.listen.length_ms);
    if (!m_audio->init(WHISPER_SAMPLE_RATE, m_config.listen.capture_id))
    {
        LOG("Error whilst initializing audio");
        PHASE(ERROR);
        return;
    }
    LOG("Audio loaded");

    LOG("Initializing Whisper...");
    whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = m_config.listen.use_gpu;

    m_wctx = whisper_init_from_file_with_params(m_config.listen.model.c_str(), cparams);
    if (m_wctx == nullptr)
    {
        LOG("Error whilst initializing whisper");
        PHASE(ERROR);
        return;
    }
    LOG("Whisper loaded");

    SetTriggerWords();

    LOG("Ready");
    QTimer::singleShot(m_config.tick_rate_ms, this, SLOT(CheckState()));
}

void HDVS::CheckState()
{
    if (StratKeyHeld())
    {
        // start listening
        QTimer::singleShot(0, this, SLOT(Listen()));
        return;
    }

    QTimer::singleShot(m_config.tick_rate_ms, this, SLOT(CheckState()));
}

void HDVS::Listen()
{
    LOG("Listening...");

    Config::Listen params = m_config.listen;

    const size_t n_samples_step = (1e-3 * params.step_ms  ) * WHISPER_SAMPLE_RATE; // Number of samplse per step
    const size_t n_samples_len  = (1e-3 * params.length_ms) * WHISPER_SAMPLE_RATE; // Total sample number
    const size_t n_samples_keep = (1e-3 * params.keep_ms  ) * WHISPER_SAMPLE_RATE; // Audio samples to keep from last sample
    const size_t n_samples_30s  = (1e-3 * 30000.0         ) * WHISPER_SAMPLE_RATE; // Number of samples per 30s

    // Buffers storing audio samples
    std::vector<float> pcmf32    (n_samples_30s, 0.0f);
    std::vector<float> pcmf32_old;
    std::vector<float> pcmf32_new(n_samples_30s, 0.0f);

    //std::vector<whisper_token> prompt_tokens;

    m_audio->resume();
    m_audio->clear();

    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    {
        wparams.print_progress = false;
        wparams.print_special = false;
        wparams.print_realtime = false;
        wparams.print_timestamps = false;
        wparams.translate = false;
        wparams.no_context = true;
        wparams.single_segment = true;
        wparams.max_tokens = 1;
        wparams.language = params.language.c_str();
        wparams.n_threads = params.n_threads;

        wparams.audio_ctx = 0;
        wparams.speed_up = false;

        wparams.tdrz_enable = false; // Tiny Diarization NOTE

        // Use trigger word tokens
        wparams.prompt_tokens = m_trigger_prompt.data();
        wparams.prompt_n_tokens = m_trigger_prompt.size();
    }

    m_running = true;
    
    while (m_running)
    {
        PHASE(LISTENING);
        while (true)
        {
            m_audio->get(params.step_ms, pcmf32_new);

            // Processing too slow!
            if (pcmf32_new.size() > n_samples_step * 2)
            {
                LOG("WARNING: Cannot process audio fast enough, dropping...");
                m_audio->clear();
                continue;
            }

            // Reached max sample size
            else if (pcmf32_new.size() >= n_samples_step)
            {
                m_audio->clear();
                break;
            }
        
            // Sleep thread then check again
            // QThread sleep appears to pause async audio too...
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        PHASE(PROCESSING);

        const size_t n_samples_new = pcmf32_new.size();
        // Take samples from previous iteration
        const size_t n_samples_old = std::min(pcmf32_old.size(), 
            (n_samples_keep + n_samples_len > n_samples_new ? 
                n_samples_keep + n_samples_len - n_samples_new : 0));
    
        pcmf32.resize(n_samples_new + n_samples_old);

        //for (size_t i = 0; i < n_samples_old.size(); i++)
        //    pcmf32[i] = pcmf32_old[pcmf32_old.size() - n_samples_old + i];

        // Copy in data
        memcpy(pcmf32.data(), pcmf32_old.data() + pcmf32_old.size() - (n_samples_old + 1), n_samples_old * sizeof(float));
        memcpy(pcmf32.data() + n_samples_old, pcmf32_new.data(), n_samples_new * sizeof(float));

        pcmf32_old = pcmf32;

        // run whisper inference

        if (whisper_full(m_wctx, wparams, pcmf32.data(), pcmf32.size()) != 0)
        {
            LOG("Failed to process audio!");
            PHASE(ERROR);
            return; // TODO: Don't quit thread on fail?
        }

        // Estimate spoken trigger phrase
        {
            const float* logits = whisper_get_logits(m_wctx);
            std::vector<float> probs(whisper_n_vocab(m_wctx), 0.0f);

            if (probs.size() > 0)
            {
                float max = logits[0];
                for (size_t i = 1; i < probs.size(); i++)
                    max = std::max(max, logits[i]);
                
                float sum = 0.0f;
                for (size_t i = 0; i < probs.size(); i++) {
                    probs[i] = expf(logits[i] - max);
                    sum += probs[i];
                }

                for (size_t i = 0; i < probs.size(); i++)
                    probs[i] /= sum;
            }

            // Probabilities + id for sorting
            std::vector<std::pair<float, size_t>> probs_id;
            size_t max_id = 0;
            //double psum = 0.0;
            for (size_t i = 0; i < m_trigger_tokens.size(); i++)
            {
                double prob = 0.0;
                for (size_t j = 0; j < m_trigger_tokens[i].size(); j++)
                    prob += probs[m_trigger_tokens[i][j]];
                
                prob /= m_trigger_tokens[i].size();
                //psum += prob;
                probs_id.emplace_back(prob, m_triggers_id[i].second); // Change to point to stratagem

                if (prob > probs_id[max_id].first)
                    max_id = i;
            }

            // normalize
            //for (size_t i = 0; i < probs_id.size(); i++)
            //    probs_id[i].first /= psum;

            // TODO: Sorting?
            // TODO: Thresholds?
            //for (const auto& p : probs_id)
            //    LOG("Heard Keyword: " + QString::fromStdString(m_triggers_id[p.second].first) + " : " + QString::number(p.first));

            if (probs_id.size() > 0)
            {
                const auto& max_prob = probs_id[max_id];
                LOG("Heard Stratagem: " + QString::fromStdString(m_stratagems[max_prob.second].name) + " : " + QString::number(max_prob.first));
                ExecuteStratagem(max_prob.second);
                running = false;
            }
        }

        // Condition for identified token
        m_running &= StratKeyHeld();
    }

    LOG("Listening Stopped");
    m_audio->pause();

    // Sleep thread until stratagem key release to avoid extra inputs
    while (StratKeyHeld())
        QThread::msleep(5);

    PHASE(IDLE);
    QTimer::singleShot(m_config.tick_rate_ms, this, SLOT(CheckState()));
}

// TODO: Must interrupt current state if processing
bool HDVS::StratKeyHeld() const {
    return (GetKeyState(m_config.keys.stratagem) & WM_KEYDOWN) != 0;
}

void HDVS::SetTriggerWords()
{
    const size_t n_tokens = 1024;

    LOG("Building trigger tokens...");
    PHASE(PROCESSING);
    m_trigger_tokens.clear();
    m_trigger_prompt.clear();
    m_triggers_id.clear();

    std::string prompt = "";
    for (size_t i = 0; i < m_stratagems.size(); i++)
    {
        if (!m_stratagems[i].enabled)
            continue;
        
        for (const auto& trigger : m_stratagems[i].trigger)
        {
            m_triggers_id.emplace_back(trigger, i);

            if (prompt != "")
                prompt += ", ";
            prompt += trigger;

            whisper_token tokens[n_tokens];
            m_trigger_tokens.emplace_back();

            for (size_t j = 0; j < trigger.size(); j++)
            {
                // Whitespace is required as first decoded token begins with whitespace
                std::string ss = std::string(" ") + trigger.substr(0, j + 1);

                int n = whisper_tokenize(m_wctx, ss.c_str(), tokens, n_tokens);
                //LOG(QString::number(n));
                if (n < 0)
                {
                    LOG(QString::fromStdString("Failed to tokenize trigger: " + trigger));
                    m_trigger_tokens.pop_back();
                    break;
                }

                else if (n == 1)
                    m_trigger_tokens.back().push_back(tokens[0]);
            }
        }
    }

    prompt = "select one from the available triggers: " + prompt + ". selected trigger: ";

    m_trigger_prompt.resize(n_tokens);
    int n = whisper_tokenize(m_wctx, prompt.c_str(), m_trigger_prompt.data(), n_tokens);
    if (n < 0)
        LOG(QString::fromStdString("Failed to tokenize prompt: " + prompt));
    
    m_trigger_prompt.resize(static_cast<size_t>(n));

    PHASE(IDLE);
}

void HDVS::ExecuteStratagem(size_t strat_id)
{
    PHASE(EXECUTING);
    size_t code_len = m_stratagems[strat_id].code.size();

    // Parse code
    for (size_t i = 0; i < code_len; i++)
    {
        INPUT input[2] = {};
        int key;
        Stratagem::Direction dir = m_stratagems[strat_id].code[i];

        switch (dir)
        {
        case Stratagem::Direction::UP:
            key = m_config.keys.up;
            break;
        
        case Stratagem::Direction::DOWN:
            key = m_config.keys.down;
            break;

        case Stratagem::Direction::RIGHT:
            key = m_config.keys.right;
            break;
        
        default:
            key = m_config.keys.left;
            break;
        }

        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = key;

        input[1] = input[1];
        input[1].ki.dwFlags = KEYEVENTF_KEYUP;

        // Send keydown
        if (!SendInput(1, input, sizeof(INPUT)))
            LOG("SendInput Failed: "); // TODO: Better error reporting
        
        QThread::msleep(m_config.diallingSpeed / 2);

        if (!SendInput(1, input + 1, sizeof(INPUT)))
            LOG("SendInput Failed: "); // TODO: Better error reporting
        
        // no delay after last keypress
        if (i != code_len - 1)
            QThread::msleep(m_config.diallingSpeed / 2);
    }
}

}
