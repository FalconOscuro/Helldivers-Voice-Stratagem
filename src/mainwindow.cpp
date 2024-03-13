#include "HDVS/mainwindow.h"
#include "../ui/ui_MainWindow.h"

#include <QtWidgets>
#include <QIcon>

namespace hdvs {

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    setWindowTitle("HDVS");
    setWindowIcon(QIcon("./data/icons/Icon.png"));

    m_status = new Status;
    m_strats = new StratPane;

    m_layout = new QHBoxLayout;
    m_layout->addWidget(m_strats);
    m_layout->addWidget(m_status);
    centralWidget()->setLayout(m_layout);
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_status;
    delete m_strats;
}

Status* MainWindow::GetStatus() const {
    return m_status;
}

StratPane* MainWindow::GetStratPane() const {
    return m_strats;
}

}
