#include "mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QWindow>
#include <QSettings>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QCoreApplication::setApplicationName(QString("Diagram maker"));
    w.show();
    return a.exec();
}
