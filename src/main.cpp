#include "HDVS/mainwindow.h"
#include "HDVS/hdvs.h"

int main(int argc, char* argv[])
{
    hdvs::hdvs app(argc, argv);

    hdvs::MainWindow window;
    window.show();

    return app.exec();
}
