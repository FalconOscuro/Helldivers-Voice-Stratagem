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

    SetPhase(Phase::IDLE);

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_phaseDisplay);
    m_layout->addWidget(m_log);
}

Status::~Status()
{
    delete m_phaseDisplay;
    delete m_log;
    delete m_layout;
}

void Status::SetPhase(Phase phase)
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

    case ERROR:
        colour = QColor("Red");
        msg = "ERROR!";
        break;
    }

    QPalette palette;
    palette.setColor(QPalette::Text, colour);

    m_phaseDisplay->setPalette(palette);
    m_phaseDisplay->setText(msg);
}

void Status::ReceiveLog(const QString& msg)
{
    m_log->insertPlainText(msg + "\n");
    m_log->ensureCursorVisible();
}

}
