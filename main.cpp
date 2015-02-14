#include "mainwindow.h"
#include <QApplication>
#include "supertcpmanager.h"
#include "httpresponse.h"
#include "httpserver.h"
#include <signal.h>
#include <QObject>
#include <memory>
#include <functional>

MainWindow* mainWindow = nullptr;

void signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGKILL
            || signum == SIGTERM)
    {
        if (mainWindow != nullptr)
        {
            SuperTcpManager::printMyDebug("sig handler");
            Epoll::sigHandle = true;
            while (Epoll::epollRunning)
            {
            }
        }
    }
    exit(signum);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    mainWindow = &w;
    w.show();

    signal(SIGINT, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGTERM, signal_handler);

    return a.exec();
}
