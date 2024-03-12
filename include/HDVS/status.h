#ifndef STATUS_H
#define STATUS_H

#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>

namespace hdvs {

class Status : public QGroupBox
{
public:
    Status(QWidget* parent = nullptr);

    enum Phase
    {
        IDLE,
        LISTENING,
        PROCESSING,
        EXECUTING,
    };

public slots:

    void SetPhase(Phase phase);
    void Log(std::string msg);

private:

    QLineEdit m_PhaseDisplay;
    QPlainTextEdit m_Log;
};

}

#endif
