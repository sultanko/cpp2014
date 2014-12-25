#ifndef DOWNLOADLISTMODEL_H
#define DOWNLOADLISTMODEL_H

#include <QAbstractListModel>
#include "downloadlistviewitemdelegate.h"
#include <vector>

class DownloadListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit DownloadListModel(QObject *parent = 0);
    ~DownloadListModel();
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

signals:

public slots:

private:
    std::vector< QPair<QString, double> > rowData;

};

Q_DECLARE_METATYPE(QVector<int>)

#endif // DOWNLOADLISTMODEL_H
