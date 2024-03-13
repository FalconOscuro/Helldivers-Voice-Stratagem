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

    delete centralWidget()->layout();
    QHBoxLayout* layout = new QHBoxLayout(centralWidget());
    layout->addWidget(m_strats);
    layout->addWidget(m_status);
}

MainWindow::~MainWindow() {
    delete m_ui;
}

Status* MainWindow::GetStatus() const {
    return m_status;
}

StratPane* MainWindow::GetStratPane() const {
    return m_strats;
}

}
