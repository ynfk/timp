#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Андреев Артем 251-351");
    a.setApplicationVersion("1.0");
    MainWindow w;
    w.show();
    return a.exec();
}
