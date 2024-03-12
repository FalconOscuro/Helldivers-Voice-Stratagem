#include "HDVS/hdvs.h"

namespace hdvs {

hdvs::hdvs(QWidget* parent):
    QMainWindow(parent),
    m_Icon("./data/icons/Icon.png"),
    m_Status(this),
    m_Main(this),
    m_MainLayout(this)
{
    setWindowTitle("HDVS");
    setWindowIcon(m_Icon);

    m_MainLayout.addWidget(&m_Status);
    m_Main.setLayout(&m_MainLayout);
    setCentralWidget(&m_Main);
}
}
