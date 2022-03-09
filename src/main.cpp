#include "include/mainwindow.hpp"
#include <QApplication>

int main(int p_argc, char *p_argv[])
{
    QApplication::setSetuidAllowed(true);
    QApplication a(p_argc, p_argv);
    MainWindow w;
    w.show();

    return a.exec();
}
