#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>

#include "status.h"
#include "config.h"

namespace Ui {
    class MainWindow;
}

namespace hdvs {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    Status* GetStatus() const;

private:
    Ui::MainWindow* m_ui;
    QVBoxLayout* m_layout;

    Status* m_Status;
};

}

#endif //MAINWINDOW_H
