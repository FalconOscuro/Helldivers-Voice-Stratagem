#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>

#include "status.h"
#include "stratpane.h"
#include "options.h"

namespace Ui {
    class MainWindow;
} // namespace Ui

namespace hdvs {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    Status* GetStatus() const;
    StratPane* GetStratPane() const;
    Options* GetOptions() const;

private:
    Ui::MainWindow* m_ui;

    Status* m_status;
    StratPane* m_strats;
    Options* m_options;
}; // class MainWindow

} // namespace hdvs

#endif //MAINWINDOW_H
