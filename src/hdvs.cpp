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
            emit LoadStratagem(stratagems[i].as<Stratagem>());
        
    }
    catch (std::runtime_error& e) {
        LOG(QString("Error whilst loading '" STRAT_PATH "': ") + e.what());
        PHASE(ERROR);
        return;
    }
    LOG("Stratagems loaded");

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
        whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
        {
            wparams.print_progress   = false;
            wparams.print_special    = false;
            wparams.print_realtime   = false;
            wparams.print_timestamps = false;
            wparams.translate        = false;
            wparams.single_segment   = true;
            wparams.max_tokens       = params.max_tokens;
            wparams.language         = params.language.c_str();
            wparams.n_threads        = params.n_threads;

            wparams.audio_ctx        = 0;
            wparams.speed_up         = false;

            wparams.tdrz_enable      = false; // Tiny Diarization NOTE

            // Use trigger word tokens
            //wparams.prompt_tokens    = m_trigger_prompt.data();
            //wparams.prompt_n_tokens  = m_trigger_prompt.size();
        }

        if (whisper_full(m_wctx, wparams, pcmf32.data(), pcmf32.size()) != 0)
        {
            LOG("Failed to process audio!");
            PHASE(ERROR);
            return; // TODO: Don't quit thread on fail?
        }

        for (int i = 0; i < whisper_full_n_segments(m_wctx); i++)
        {
            const char* text = whisper_full_get_segment_text(m_wctx, i);
            LOG(text);
        }

        // Condition for identified token
        m_running &= StratKeyHeld();
    }

    LOG("Listening Stopped");
    m_audio->pause();

    // await key release

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

    std::string prompt = "";

    for (const auto& stratagem : m_stratagems)
    {
        // Skip disabled
        if (!stratagem.enabled)
            continue;
        
        for (const auto& trigger : stratagem.trigger)
        {
            if (prompt != "")
                prompt += ", ";
            
            prompt += trigger;

            whisper_token tokens[n_tokens];
            m_trigger_tokens.emplace_back();

            for (size_t i = 0; i < trigger.size(); i++)
            {
                // Whitespace is required as first decoded token begins with whitespace
                std::string ss = std::string(" ") + trigger.substr(0, i + 1);

                if (whisper_tokenize(m_wctx, ss.c_str(), tokens, n_tokens) != 1)
                {
                    LOG(QString::fromStdString("Failed to tokenize trigger: " + trigger));
                    m_trigger_tokens.pop_back();
                    break;
                }

                m_trigger_tokens.back().push_back(tokens[0]);
            }
        }
    }
    prompt = "Select one from the available phrases: " + prompt + ". selected phrase: ";

    m_trigger_prompt.resize(n_tokens);
    if (whisper_tokenize(m_wctx, prompt.c_str(), m_trigger_prompt.data(), n_tokens) < 0)
        LOG(QString::fromStdString("Failed to tokenize prompt: " + prompt));

    PHASE(IDLE);
}

}
