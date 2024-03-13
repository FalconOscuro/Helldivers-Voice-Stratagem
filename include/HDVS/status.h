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
    explicit Status(QWidget* parent = nullptr);

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

    QLineEdit* m_phaseDisplay;
    QPlainTextEdit* m_log;
}; // class Status

} // namespace hdvs

#endif
