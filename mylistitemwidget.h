#ifndef MYLISTITEMWIDGET_H
#define MYLISTITEMWIDGET_H

#include <QObject>
#include <QListWidgetItem>

class MyListItemWidget : public QObject
{
    Q_OBJECT
public:
    explicit MyListItemWidget(QObject *parent = 0);

signals:

public slots:

};

#endif // MYLISTITEMWIDGET_H
