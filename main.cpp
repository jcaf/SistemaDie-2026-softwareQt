#include "mainwindow.h"

#include <QApplication>
#include <QtSerialPort/qserialport>
#include <QtSerialPort/qserialportinfo>
#include <QDebug>
#include <QMessageBox>
#include <QStyleFactory>

int main(int argc, char *argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setStyle(QStyleFactory::create("Fusion")); // these lines before the next


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
