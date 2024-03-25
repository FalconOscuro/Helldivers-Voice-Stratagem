#include "HDVS/status.h"

#include <QColor>
#include <QPalette>
#include <QtWidgets>

namespace hdvs {

Status::Status(QWidget* parent):
    QGroupBox("Status", parent)
{
    m_phaseDisplay = new QLineEdit;
    m_phaseDisplay->setReadOnly(true);

    m_log = new QPlainTextEdit;
    m_log->setReadOnly(true);
    m_log->setTextInteractionFlags(Qt::NoTextInteraction);

    SetPhase(Phase::IDLE);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_phaseDisplay);
    layout->addWidget(m_log);
}

void Status::SetPhase(hdvs::Status::Phase phase)
{
    QColor colour;
    QString msg;

    switch (phase)
    {
    case IDLE:
        colour = QColor("Black");
        msg = "Idle";
        break;
    
    case LISTENING:
        colour = QColor("Blue");
        msg = "Listening...";
        break;

    case PROCESSING:
        colour = QColor("Grey");
        msg = "Processing...";
        break;

    case EXECUTING:
        colour = QColor("Green");
        msg = "Executing!";
        break;

    default:
        colour = QColor("Red");
        msg = "ERROR!";
        break;
    }

    QPalette palette;
    palette.setColor(QPalette::Text, colour);

    // TODO: use signals/slots
    m_phaseDisplay->setPalette(palette);
    m_phaseDisplay->setText(msg);
}

void Status::ReceiveLog(const QString& msg)
{
    // TODO: use signals/slots
    m_log->insertPlainText(msg + "\n");
    m_log->ensureCursorVisible();
}

}
