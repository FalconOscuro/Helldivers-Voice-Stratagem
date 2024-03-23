#include "HDVS/mainwindow.h"
#include "HDVS/hdvs.h"

#include <SDL.h>

#include <QApplication>
#include <QThread>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<hdvs::Status::Phase>();
    qRegisterMetaType<hdvs::Stratagem>();

    hdvs::MainWindow window;
    window.show();

    QThread* workerThread = new QThread;
    hdvs::HDVS* core = new hdvs::HDVS;
    core->moveToThread(workerThread);

    QObject::connect(
        core,                   &hdvs::HDVS::SendLog,
        window.GetStatus(),     &hdvs::Status::ReceiveLog
    );
    QObject::connect(
        core,                   &hdvs::HDVS::PhaseChange,
        window.GetStatus(),     &hdvs::Status::SetPhase
    );

    QObject::connect(
        core,                   &hdvs::HDVS::LoadStratagem,
        window.GetStratPane(),  &hdvs::StratPane::AddStratagem
    );
    QObject::connect(
        window.GetStratPane(),  &hdvs::StratPane::UpdateStratagems,
        core,                   &hdvs::HDVS::UpdateStratagems
    );

    workerThread->start();
    int exitCode = app.exec();
    workerThread->quit();
    workerThread->wait();

    delete workerThread;
    delete core;

    return exitCode;
}
