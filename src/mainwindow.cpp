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

    m_Status = new Status;

    m_layout = new QVBoxLayout;
    m_layout->addWidget(m_Status);
    centralWidget()->setLayout(m_layout);
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_Status;
}

Status* MainWindow::GetStatus() const {
    return m_Status;
}

}
