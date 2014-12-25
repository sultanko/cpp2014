#ifndef DOWNLOADLISTVIEWITEM_H
#define DOWNLOADLISTVIEWITEM_H

#include <QWidget>
#include "ui_download_item_row.h"
#include <QItemDelegate>

class DownloadListViewItem : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadListViewItem(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~DownloadListViewItem();

public:
    Ui::DownloadWidgetForm ui;

signals:

public slots:

};

#endif // DOWNLOADLISTVIEWITEM_H
