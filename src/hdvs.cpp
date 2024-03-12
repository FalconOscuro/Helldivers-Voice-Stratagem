#include "HDVS/hdvs.h"

namespace hdvs {

hdvs::hdvs(int& argc, char* argv[]):
    QApplication(argc, argv)
{
    setApplicationName("HDVS");
    setApplicationVersion("2.0b");
}

}
