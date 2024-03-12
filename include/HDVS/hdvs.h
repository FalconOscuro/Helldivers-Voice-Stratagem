#ifndef HDVS_H
#define HDVS_H

#include <QApplication>

namespace hdvs {

class hdvs : public QApplication
{
    Q_OBJECT
public:
    explicit hdvs(int& argc, char* argv[]);

protected:

private:
};

}

#endif // HDVS_H
