#ifndef STATUS_H
#define STATUS_H

#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>

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
    void Log(QString msg);

private:

    QLineEdit m_PhaseDisplay;
    QPlainTextEdit m_Log;
    QVBoxLayout m_Layout;
};

}

#endif
