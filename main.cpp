#include "mainwindow.h"
#include <QApplication>
#include "supertcpmanager.h"
#include <signal.h>
#include <QObject>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    SuperTcpManager superTcpServer;
    w.setServer(&superTcpServer);
    QObject::connect(&superTcpServer, &SuperTcpManager::newMessageReceived,
                     &w, &MainWindow::showMessageFromServer);
    const int var = 1;
    if (var == 1)
    {
        w.setWindowTitle("Server");
        superTcpServer.listen();
//        superTcpServer.startServer();
    }
    else
    {
        w.setWindowTitle("Client");
        superTcpServer.connect("192.168.102.52", "80");
        superTcpServer.connect("127.0.0.1", superTcpServer.getServerPort());
//        superTcpServer.connect("188.227.78.184", superTcpServer.getServerPort());
        superTcpServer.sendToAll("Hello server!");
    }

//    int n;
//    scanf("%d", &n);

//    superTcpServer.stopServer();
    return a.exec();
    return 0;
}
