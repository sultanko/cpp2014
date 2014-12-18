#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->setText("https://androidnetworktester.googlecode.com/files/1mb.txt");
    QObject::connect(this, &MainWindow::messageSignal, this, &MainWindow::displayMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayMessage(int count)
{
    SuperTcpManager::printDebug("received", count);
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_4_5);
//        buf.push_back('\0');
        for (size_t i = 0; i < buf.size(); i++)
        {
            out << buf[i];
        }
        ui->textBrowser->setText("Saved as" + fileName);
    }
//    ui->textBrowser->setText(QString::fromUtf8(buf, count));
}

void MainWindow::on_pushButton_clicked()
{
//    std::cerr << ui->textEdit->toPlainText().toStdString().c_str() << "\n";
    QString str = ui->textEdit->toPlainText();
    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), "", tr("All files(*)"));
    int num = str.indexOf("://");
    int num2 = str.indexOf("/", num + 4);
    HttpRequest request;
    request.setHost(str.mid(num + 3, num2 - num - 3).toStdString());
    request.setUrl(str.mid(num2, str.size() - num2).toStdString());
    server.send(request, [&](HttpResponse response)
    {
        this->buf.assign(response.getResponseBuffer().begin(), response.getResponseBuffer().end());
        SuperTcpManager::printDebug("responseBuffer ixooooo", this->buf.data());
        emit this->messageSignal((int) response.getResponseSize());
    });
}
