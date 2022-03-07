#include "include/mainwindow.hpp"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication::setSetuidAllowed(true);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
