#include "HDVS/hdvs.h"

namespace hdvs {

hdvs::hdvs(QWidget* parent):
    QMainWindow(parent)
{
    setWindowTitle("HDVS");
    
    m_Icon = QIcon("./data/icons/Icon.png");
    setWindowIcon(m_Icon);
}
}
