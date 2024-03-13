#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>

#include "status.h"
#include "stratpane.h"

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
    StratPane* GetStratPane() const;

private:
    Ui::MainWindow* m_ui;
    QHBoxLayout* m_layout;

    Status* m_status;
    StratPane* m_strats;
};

}

#endif //MAINWINDOW_H
