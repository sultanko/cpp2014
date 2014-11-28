#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::showMessageFromServer(int clientId, QString message)
{
   ui->textBrowser->setText(message);
//   fprintf(stderr, "GOOOOOOOD %s", message);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setServer(SuperTcpManager *tcpServer)
{
    this->tcpServer = tcpServer;
}

void MainWindow::on_pushButton_clicked()
{
    std::cerr << ui->textEdit->toPlainText().toStdString().c_str() << "\n";
    this->tcpServer->sendToAll(ui->textEdit->toPlainText().toStdString().c_str());
}
