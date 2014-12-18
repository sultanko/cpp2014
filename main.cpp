#include "mainwindow.h"
#include <QApplication>
#include "supertcpmanager.h"
#include "httpresponse.h"
#include "httpserver.h"
#include <signal.h>
#include <QObject>
#include <memory>
#include <functional>

void message(MainWindow* w, int v)
{
    SuperTcpManager::printDebug("received", v);
    emit w->messageSignal(v);
}

void messageResponse(MainWindow* w, HttpResponse response)
{
    w->buf.assign(response.getResponseBuffer().begin(), response.getResponseBuffer().end());
    SuperTcpManager::printDebug("responseBuffer ixooooo", response.getResponseSize());
    emit w->messageSignal((int) response.getResponseSize());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    SuperTcpManager superTcpServer;

    const int var = 3;
    auto func = std::bind(&messageResponse, &w, std::placeholders::_1);
//    HttpServer server;
    if (var == 1)
    {
        w.setWindowTitle("Server");
//        auto func = std::bind(&MainWindow::displayMessage, &w, std::placeholders::_1);
/*        w.setSocket(superTcpServer.listen([&](ClientSocket* socket)
        {
            socket->setFunction(func);
            socket->setBuffer(w.buf);
            socket->setBufferSize(MainWindow::BUF_SIZE);
        }));*/
//        raise(SIGINT);
//        superTcpServer.startServer();
    }
    else if (var == 2)
    {
        w.setWindowTitle("Client");
        HttpRequest request;
        request.setMethod("GET");
        request.setUrl("/files/1mb.txt");
        request.setHost("androidnetworktester.googlecode.com");
//        server.send(request, func);
//        w.setSocket(superTcpServer.connect("127.0.0.1", superTcpServer.getServerPort().c_str(), func, w.buf, MainWindow::BUF_SIZE));
    }

    return a.exec();
}
