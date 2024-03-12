#include "HDVS/hdvs.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    hdvs::hdvs window;
    window.show();

    return app.exec();
}
