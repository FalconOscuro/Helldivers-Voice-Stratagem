#include "HDVS/status.h"

#include <QColor>
#include <QPalette>
#include <QtWidgets>

namespace hdvs {

Status::Status(QWidget* parent):
    QGroupBox("Status", parent),
    m_PhaseDisplay(this),
    m_Log(this),
    m_Layout(this)
{
    m_PhaseDisplay.setReadOnly(true);
    m_Log.setReadOnly(true);

    SetPhase(Phase::IDLE);

    m_Layout.addWidget(&m_PhaseDisplay);
    m_Layout.addWidget(&m_Log);
    setLayout(&m_Layout);
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
        colour = QColor("Red");
        msg = "Processing...";
        break;

    case EXECUTING:
        colour = QColor("Green");
        msg = "Executing!";
        break;
    }

    QPalette palette;
    palette.setColor(QPalette::Text, colour);

    m_PhaseDisplay.setPalette(palette);
    m_PhaseDisplay.setText(msg);
}

void Status::ReceiveLog(const QString& msg)
{
    m_Log.insertPlainText(msg + "\n");
    m_Log.ensureCursorVisible();
}

}
