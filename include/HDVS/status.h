#ifndef STATUS_H
#define STATUS_H

#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>

namespace hdvs {

class Status : public QGroupBox
{
    Q_OBJECT

public:
    Status(QWidget* parent = nullptr);

    enum Phase
    {
        IDLE,
        LISTENING,
        PROCESSING,
        EXECUTING,
        ERROR,
    };

public slots:

    void SetPhase(Phase phase);
    void ReceiveLog(const QString& msg);

private:

    QLineEdit m_PhaseDisplay;
    QPlainTextEdit m_Log;
    QVBoxLayout m_Layout;
};

}

#endif
