#include "mainwindow.h"
#include <QApplication>
#include "supertcpmanager.h"
#include <signal.h>

//void sig_handler(int signo)
//{
//    if (signo == SIGINT)
//    {
//        fprintf(stderr, "Noooooooo\n");
//    }
//}

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    signal(SIGINT, sig_handler);
    SuperTcpManager superTcpServer;
    const char *PORT = "3498";
    const int var = 2;
    if (var == 1)
    {
        superTcpServer.listen(PORT);
        superTcpServer.start();
    }
    else
    {
        superTcpServer.connect("192.168.102.52", "80");
        superTcpServer.connect("188.227.78.184", PORT);
        superTcpServer.connect("192.108.2.105", PORT);
        superTcpServer.connect("127.0.0.1", PORT);
        superTcpServer.connect("127.0.0.1", PORT);
        superTcpServer.connect("127.0.0.1", PORT);
        superTcpServer.sendToAll("bla", 3);
    }

//    return a.exec();
    return 0;
}
