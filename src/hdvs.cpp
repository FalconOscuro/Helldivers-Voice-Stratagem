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
#define PHASE(phase) emit PhaseChange(phase)
// Temp!!
#define TICK_RATE 50

namespace hdvs {

HDVS::HDVS(QObject* parent):
    QObject(parent)
{
    QTimer::singleShot(0, this, SLOT(PostInit()));
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
        PHASE(Status::Phase::ERROR);
    }

    fout.close();
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
        PHASE(Status::Phase::ERROR);
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
        PHASE(Status::Phase::ERROR);
        return;
    }
    LOG("Stratagems loaded");

    LOG("Ready");
    QTimer::singleShot(TICK_RATE, this, SLOT(CheckState()));
}

void HDVS::CheckState()
{
    if (GetKeyState(m_config.keys.stratagem) & WM_KEYDOWN)
    {
        // start listening
        QTimer::singleShot(0, this, SLOT(Listen()));
        return;
    }

    QTimer::singleShot(TICK_RATE, this, SLOT(CheckState()));
}

void HDVS::Listen()
{
    LOG("Listening...");
    PHASE(Status::Phase::LISTENING);

    // Listen
    bool decoded = false;

    // Listen again if still held
    if (GetKeyState(m_config.keys.stratagem) & WM_KEYDOWN && !decoded)
        QTimer::singleShot(500, this, SLOT(Listen()));

    // TODO: Pause until stratagem released
    else
    {
        PHASE(Status::Phase::IDLE);
        QTimer::singleShot(TICK_RATE, this, SLOT(CheckState()));
    }
}

}
