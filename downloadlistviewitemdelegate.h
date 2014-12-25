#ifndef DOWNLOADLISTVIEWITEMDELEGATE_H
#define DOWNLOADLISTVIEWITEMDELEGATE_H

#include <QItemDelegate>
#include "downloadlistviewitem.h"

class DownloadListViewItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit DownloadListViewItemDelegate(QObject *parent = 0);
    ~DownloadListViewItemDelegate();
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

signals:

public slots:

protected:
    DownloadListViewItem* itemWidget;
};

#endif // DOWNLOADLISTVIEWITEMDELEGATE_H
