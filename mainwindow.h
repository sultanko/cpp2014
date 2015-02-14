#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "httpserver.h"
#include <map>
#include "downloadlistmodel.h"
#include "downloadlistviewitemdelegate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void messageSignal(HttpResponse* resp);

public slots:
    void saveFile(HttpResponse* resp);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<DownloadListModel> listModel;
    std::unique_ptr<DownloadListViewItemDelegate> listViewItem;
    HttpServer server;
    const QString defaultDir = "/home/sultan";
    std::map<HttpResponse*, QPair<QString, int> > respToRow;
public:
    std::vector<char> buf;
};

#endif // MAINWINDOW_H
