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

class HDVS : public QObject
{
    Q_OBJECT
public:
    explicit HDVS(QObject* parent = nullptr);

signals:
    void SendLog(const QString& msg);
    void PhaseChange(hdvs::Status::Phase phase);

    void LoadStratagem(const hdvs::Stratagem& strat);

public slots:
    void UpdateStratagems(const QList<hdvs::Stratagem>& stratagems);

private slots:
    void PostInit();

    void CheckState();
    void Listen();

private:
    Config m_config;
    std::vector<Stratagem> m_stratagems;
}; // class HDVS

} // namespace hdvs

#endif // HDVS_H