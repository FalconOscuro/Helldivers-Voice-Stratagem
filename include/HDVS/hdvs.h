#ifndef HDVS_H
#define HDVS_H

#include <QObject>

#include <vector>

#include "config.h"
#include "status.h"
#include "stratagem.h"

namespace hdvs {

class hdvs : public QObject
{
    Q_OBJECT
public:
    explicit hdvs(QObject* parent = nullptr);

signals:
    void SendLog(const QString& msg);
    void PhaseChange(Status::Phase phase);

protected:

private slots:
    void PostInit();

private:
    Config m_config;
    std::vector<Stratagem> m_stratagems;
};

}

#endif // HDVS_H
