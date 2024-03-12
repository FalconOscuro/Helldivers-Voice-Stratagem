#include "HDVS/mainwindow.h"
#include "../ui/ui_MainWindow.h"

#include <QtWidgets>
#include <QTimer>

#include <yaml-cpp/yaml.h>

#define LOG(msg) emit SendLog((msg))

namespace hdvs {

MainWindow::MainWindow(QWidget* parent):
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_Status(this),
    m_Layout(this),
    m_Main(this)
{
    m_ui->setupUi(this);

    m_Layout.addWidget(&m_Status);
    m_Main.setLayout(&m_Layout);
    setCentralWidget(&m_Main);

    connect(this, &MainWindow::SendLog, &m_Status, &Status::ReceiveLog);
}

MainWindow::~MainWindow() {
    delete m_ui;
}

void MainWindow::showEvent(QShowEvent* e) {
    QMainWindow::showEvent(e);
    QTimer::singleShot(0, this, SLOT(PostInit()));
}

void MainWindow::PostInit()
{
    LOG("Beginning startup");

    LOG("Loading configuration...");
    try {
        YAML::Node config = YAML::LoadFile("./data/config.yml");
    }
    catch (std::runtime_error e) {
        LOG(QString("Error whilst loading './data/config.yml': ") + e.what());
        return;
    }
    LOG("Configuration loaded");

    LOG("Loading stratagems...");
    try {
        YAML::Node stratagems = YAML::LoadFile("./data/stratagem.yml");
    }
    catch (std::runtime_error e) {
        LOG(QString("Error whilst loading './data/stratagem.yml': ") + e.what());
        return;
    }
    LOG("Stratagems loaded");

    LOG("Ready");
}

}
