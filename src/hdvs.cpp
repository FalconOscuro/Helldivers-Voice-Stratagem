#include "HDVS/hdvs.h"

#include <QTimer>
#include <QThread>
#include <yaml-cpp/yaml.h>
#include <fstream>

#define LOG(msg) emit SendLog(msg)
#define PHASE(phase) emit PhaseChange(phase)

namespace hdvs {

hdvs::hdvs(QObject* parent):
    QObject(parent)
{
    QTimer::singleShot(50, this, SLOT(PostInit()));
}

void hdvs::UpdateStratagems(const QList<QVariant>& stratagems)
{
    std::vector<Stratagem> temp;
    YAML::Node ymlOut;
    for (int i = 0; i < stratagems.size(); i++)
    {
        QVariant var = stratagems[i];
        if (!var.canConvert<Stratagem>())
        {
            LOG("Error whilst reading back data, could not convert!");
            PHASE(Status::Phase::ERROR);
            return;
        }

        Stratagem stratagem = var.value<Stratagem>();
        temp.push_back(stratagem);
        ymlOut.push_back(stratagem);
    }

    LOG("Successfuly recieved stratagem data writing...");
    m_stratagems = temp;

    std::ofstream fout(STRAT_PATH);
    fout << ymlOut;

    if (fout.bad())
    {
        LOG("Error whilst writing to '" STRAT_PATH "'");
        PHASE(Status::Phase::ERROR);
    }

    fout.close();
}

void hdvs::PostInit()
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
        //m_stratagems = stratagems.as<std::vector<Stratagem>>();

        for (size_t i = 0; i < stratagems.size(); i++)
        {
            Stratagem strat = stratagems[i].as<Stratagem>();
            m_stratagems.push_back(strat);

            QVariant var;
            var.setValue(strat);

            emit LoadStratagem(var);
        }
    }
    catch (std::runtime_error& e) {
        LOG(QString("Error whilst loading '" STRAT_PATH "': ") + e.what());
        PHASE(Status::Phase::ERROR);
        return;
    }
    LOG("Stratagems loaded");

    LOG("Ready");
}

}
