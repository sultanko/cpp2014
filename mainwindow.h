#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "supertcpmanager.h"
#include "httpserver.h"

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
    void messageSignal(int v);

public slots:
    void displayMessage(int count);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    HttpServer server;
public:
    std::vector<char> buf;
};

#endif // MAINWINDOW_H
