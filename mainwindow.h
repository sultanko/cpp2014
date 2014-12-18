#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "httpserver.h"
#include "supertcpmanager.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void displayMessage(int count);


signals:
    void messageSignal(int count);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
public:
    std::vector<char> buf;
    static const int BUF_SIZE = 1024*1024*2;
    QString fileName;
    HttpServer server;
};

#endif // MAINWINDOW_H
