#include "mainwindow.h"
#include <QApplication>
#include "supertcpmanager.h"
#include "httpresponse.h"
#include "httpserver.h"
#include <signal.h>
#include <QObject>
#include <memory>
#include <functional>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
