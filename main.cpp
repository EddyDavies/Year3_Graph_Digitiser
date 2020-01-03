#include "digitiser.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Digitiser w;
    w.show();
    return a.exec();
}
