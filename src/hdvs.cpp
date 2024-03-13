#include "HDVS/hdvs.h"

#include <QTimer>
#include <QThread>
#include <yaml-cpp/yaml.h>

#define LOG(msg) emit SendLog(msg)
#define PHASE(phase) emit PhaseChange(phase)

namespace hdvs {

hdvs::hdvs(QObject* parent):
    QObject(parent)
{
    QTimer::singleShot(50, this, SLOT(PostInit()));
}

hdvs::~hdvs()
{}

void hdvs::PostInit()
{
    LOG("Beginning startup");

    LOG("Loading configuration...");
    try {
        YAML::Node config = YAML::LoadFile("./data/config.yml");
        m_config = config.as<Config>();
    }
    catch (std::runtime_error e) {
        LOG(QString("Error whilst loading './data/config.yml': ") + e.what());
        PHASE(Status::Phase::ERROR);
        return;
    }
    LOG("Configuration loaded");

    LOG("Loading stratagems...");
    try {
        YAML::Node stratagems = YAML::LoadFile("./data/stratagem.yml");
        m_stratagems = stratagems.as<std::vector<Stratagem>>();

        for (size_t i = 0; i < m_stratagems.size(); i++)
        {
            QVariant var;
            var.setValue(m_stratagems[i]);
            emit LoadStratagem(var);
        }
    }
    catch (std::runtime_error e) {
        LOG(QString("Error whilst loading './data/stratagem.yml': ") + e.what());
        PHASE(Status::Phase::ERROR);
        return;
    }
    LOG("Stratagems loaded");

    LOG("Ready");
}

}
