#ifndef HDVS_H
#define HDVS_H

#include <QMainWindow>
#include <QHBoxLayout>

#include "status.h"

namespace hdvs {
class hdvs : public QMainWindow
{
public:
    hdvs(QWidget* parent = nullptr);

private:

    QIcon m_Icon;

    Status m_Status;

    QWidget m_Main;
    QHBoxLayout m_MainLayout;
};

}

#endif
