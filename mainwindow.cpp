#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    listModel(new DownloadListModel()),
    listViewItem(new DownloadListViewItemDelegate())

{
    ui->setupUi(this);
//    ui->textEdit->setText("https://androidnetworktester.googlecode.com/files/10mb.txt");
    ui->textEdit->setText("http://upload.wikimedia.org/wikipedia/en/thumb/2/24/Lenna.png/220px-Lenna.png");
    QObject::connect(this, &MainWindow::messageSignal, this, &MainWindow::saveFile);
    SuperTcpManager::printMyDebug("start Widget");
    QListView *listView = ui->listView;
    listView->setItemDelegate(listViewItem.get());
    listView->setModel(listModel.get());
    SuperTcpManager::printMyDebug("end Widget");
}

MainWindow::~MainWindow()
{
    if (ui != nullptr)
    {
        delete ui;
    }
    ui = nullptr;
}

void MainWindow::saveFile(HttpResponse* resp)
{
    SuperTcpManager::printMyDebug("received", resp->getResponseSize());
    QString filename = respToRow[resp].first;
    if (!filename.isEmpty())
    {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_4_5);
        out.writeRawData(resp->getResponseBuffer().data(), resp->getResponseSize());
//        for (size_t i = 0; i < resp->getResponseSize(); i++)
//        {
//            out << resp->getResponseBuffer()[i];
//        }
        resp->close();
        file.close();
    }
}

void MainWindow::on_pushButton_clicked()
{
//    std::cerr << ui->textEdit->toPlainText().toStdString().c_str() << "\n";
    QString str = ui->textEdit->toPlainText();
    int num3 = str.lastIndexOf("/");
    QString nowDir = defaultDir + "/" + str.mid(num3 + 1, str.size() - num3 - 1);
    QString filename = QFileDialog::getSaveFileName(this, tr("Save file"),  nowDir, tr("All files(*)"));
    int num = str.indexOf("://");
    int num2 = str.indexOf("/", num + 4);
    if (filename.isEmpty())
    {
        return;
    }
    HttpRequest request;
    request.setHost(str.mid(num + 3, num2 - num - 3).toStdString());
    request.setUrl(str.mid(num2, str.size() - num2).toStdString());
    server.send(request, [&, filename](HttpResponse* response, int getted)
    {
        if (getted == -1)
        {
            response->close();
            return;
        }
        if (getted == 0)
        {
            int rowNum = ui->listView->model()->rowCount();
            ui->listView->model()->insertRows(rowNum, 1);
            respToRow[response] = QPair<QString, int>(filename, rowNum);
            ui->listView->model()->setData(ui->listView->model()->index(rowNum, 0), QVariant::fromValue(
                                               QPair<QString, double>(filename, 0)));
            return;
        }
        SuperTcpManager::printMyDebug(respToRow[response].second);
        ui->listView->model()->setData(ui->listView->model()->index(respToRow[response].second, 0),
                                       QVariant::fromValue(QPair<QString, double>(respToRow[response].first,
                                                                 1.0 * getted / response->getResponseSize())));
        if (response->getResponseSize() == getted)
        {
            SuperTcpManager::printMyDebug("responseBuffer ixooooo");
            emit this->messageSignal(response);
        }
    });
}

