#include "HDVS/mainwindow.h"
#include "HDVS/hdvs.h"

#include <QApplication>
#include <QThread>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    hdvs::MainWindow window;
    window.show();

    QThread* workerThread = new QThread;
    hdvs::hdvs* core = new hdvs::hdvs;
    core->moveToThread(workerThread);

    QObject::connect(
        core,                   &hdvs::hdvs::SendLog,
        window.GetStatus(),     &hdvs::Status::ReceiveLog
    );
    QObject::connect(
        core,                   &hdvs::hdvs::PhaseChange,
        window.GetStatus(),     &hdvs::Status::SetPhase
    );

    QObject::connect(
        core,                   &hdvs::hdvs::LoadStratagem,
        window.GetStratPane(),  &hdvs::StratPane::AddStratagem
    );
    QObject::connect(
        window.GetStratPane(),  &hdvs::StratPane::UpdateStratagems,
        core,                   &hdvs::hdvs::UpdateStratagems
    );

    workerThread->start();
    int exitCode = app.exec();
    workerThread->quit();
    workerThread->wait();

    delete workerThread;
    delete core;

    return exitCode;
}
