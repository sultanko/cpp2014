#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void setServer(SuperTcpManager* tcpServer);

public slots:
    void showMessageFromServer(int clientId, QString message);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    SuperTcpManager* tcpServer;
};

#endif // MAINWINDOW_H
