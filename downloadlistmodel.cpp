#include "downloadlistmodel.h"
#include "supertcpmanager.h"

DownloadListModel::DownloadListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

DownloadListModel::~DownloadListModel()
{
}

int DownloadListModel::rowCount(const QModelIndex &parent) const
{
    return (int)rowData.size();
}

QVariant DownloadListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        return QVariant::fromValue(QPair<QString, double>(rowData[index.row()]));
    }

    return QVariant();
}

bool DownloadListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    SuperTcpManager::printMyDebug("start insert rows", row, count);
    int last = row + count - 1;
    beginInsertRows(parent, row, last);
    while (count > 0)
    {
        rowData.push_back(QPair<QString, double>());
        SuperTcpManager::printMyDebug("rows changed", rowData.size());
        count--;
    }
    endInsertRows();
    SuperTcpManager::printMyDebug("end insert rows", row, count);
    return true;
}

bool DownloadListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    SuperTcpManager::printMyDebug("set data");
    QPair<QString, double> pair = value.value<QPair<QString, double> >();
    SuperTcpManager::printMyDebug(index.row(), pair.second);
    rowData[index.row()] = pair;
    SuperTcpManager::printMyDebug("set data end");
    qRegisterMetaType<QVector<int> >("QVector<int>");
    emit dataChanged(index, index);
    return true;
}
