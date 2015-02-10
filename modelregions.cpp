#include "modelregions.h"
#include <QDebug>

ModelRegions::ModelRegions(const QSqlDatabase &db, QObject *parent) :
    QSqlQueryModel(parent),
    m_db(db)
{
}

void ModelRegions::refresh()
{
    clear();

    QString qry;
    qry = "select aoid";
    qry+= ",regioncode";
    qry+= ",offname";
    qry+= ",shortname";
    qry+= ",aolevel";
    qry+= ",aoguid";
    qry+= ",actstatus ";
    qry+= "from addr_regions ";
    qry+= "order by regioncode";
//    qry = "select * from addr_regions order by regioncode";
//    qDebug()<<qry;
    setQuery(qry,m_db);

    setHeaderData(0,Qt::Horizontal,tr("aoid"));
    setHeaderData(1,Qt::Horizontal,tr("Код региона"));
    setHeaderData(2,Qt::Horizontal,tr("Офицальное название"));
    setHeaderData(3,Qt::Horizontal,tr("Короткое название"));
    setHeaderData(4,Qt::Horizontal,tr("Уровень"));
    setHeaderData(5,Qt::Horizontal,tr("aoguid"));
    setHeaderData(6,Qt::Horizontal,tr("actstatus"));

}

Qt::ItemFlags ModelRegions::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    return flags;

}

QVariant ModelRegions::data(const QModelIndex &item, int role) const
{
    QVariant value = QSqlQueryModel::data(item, role);
    switch(role){
    case Qt::DisplayRole:
        return value;
    }

    return value;
}
