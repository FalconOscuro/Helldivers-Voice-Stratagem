#ifndef HDVS_H
#define HDVS_H

#include <QObject>

#include "config.h"
#include "status.h"

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
};

}

#endif // HDVS_H
