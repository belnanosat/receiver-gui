#include "mainwindow.h"
#include <QApplication>

#include "configdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //ConfigDialog w;
    w.show();

    return a.exec();
}
