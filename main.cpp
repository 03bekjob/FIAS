#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("FreeDeveloper");
    a.setApplicationName("FIAS");
    MainWindow w;

    w.showMaximized();

    return a.exec();
}
