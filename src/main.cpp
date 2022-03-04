#include "include/mainwindow.hpp"
#include <QApplication>
#include <getopt.h>

int main(int argc, char *argv[])
{
    QApplication::setSetuidAllowed(true);
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    const struct option stopcoes[] = {
        {"pid", no_argument, 0, 'p'}};

    int opt;
    while ((opt = getopt_long(argc, argv, "h:p", stopcoes, NULL)) > 0)
    {
        switch (opt)
        {
        case 'p':
            w.m_pid = std::atoi(argv[optind]);
            w.mainMapper();
            break;
        default:
            break;
        }
    }

    return a.exec();
}