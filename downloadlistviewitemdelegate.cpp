#include "downloadlistviewitemdelegate.h"
#include <QtWidgets>
#include "supertcpmanager.h"

DownloadListViewItemDelegate::DownloadListViewItemDelegate(QObject *parent) :
    QItemDelegate(parent),
    itemWidget(new DownloadListViewItem())
{
}

DownloadListViewItemDelegate::~DownloadListViewItemDelegate()
{
    delete itemWidget;
}

void DownloadListViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    itemWidget->resize(option.rect.size());

    if (index.data().isValid())
    {
        QPair<QString, double> pair = index.data().value< QPair<QString, double> >();
        itemWidget->ui.label_num->setText(QString::number(index.row() + 1));
        itemWidget->ui.label_name->setText(pair.first);
        itemWidget->ui.progressBar->setValue(pair.second * 100);

    }

    // Change the background color of the widget if it is selected.
    QPalette pal;
    if ((option.state & QStyle::State_Selected) == QStyle::State_Selected)
    {
        pal.setBrush(QPalette::Window, QBrush(QColor(Qt::lightGray)));
    }
    else
    {
        pal.setBrush(QPalette::Window, QBrush(QColor(Qt::transparent)));
    }
    itemWidget->setPalette(pal);

    painter->save();
    painter->translate(option.rect.topLeft());
    itemWidget->render(painter);
    painter->restore();
}

QSize DownloadListViewItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const int width = itemWidget->minimumWidth();
    const int height = itemWidget->height();
    return QSize(width, height);
}
