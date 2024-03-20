#ifndef HDVS_H
#define HDVS_H

#include <QObject>

#include <vector>

#include "config.h"
#include "status.h"
#include "stratagem.h"

#define DATA_PATH               "./data"
#define CONFIG_PATH DATA_PATH   "/config.yml"
#define STRAT_PATH  DATA_PATH   "/stratagem.yml"

namespace hdvs {

class hdvs : public QObject
{
    Q_OBJECT
public:
    explicit hdvs(QObject* parent = nullptr);

signals:
    void SendLog(const QString& msg);
    void PhaseChange(Status::Phase phase);

    void LoadStratagem(const QVariant& strat);

public slots:
    void UpdateStratagems(const QList<QVariant>& stratagems);

private slots:
    void PostInit();

private:
    Config m_config;
    std::vector<Stratagem> m_stratagems;
};

}

#endif // HDVS_H
