#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "status.h"

namespace Ui {
    class MainWindow;
}

namespace hdvs {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow() override;

signals:
    void SendLog(const QString& msg);

protected:
    void showEvent(QShowEvent* e) override;

private slots:
    void PostInit();

private:
    Ui::MainWindow* m_ui;

    QWidget m_Main;
    QVBoxLayout m_Layout;
    Status m_Status;
};

}

#endif //MAINWINDOW_H
