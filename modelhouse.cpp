#include "modelhouse.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

#include <QDebug>

ModelHouse::ModelHouse(const QSqlDatabase &db, QObject *parent) :
    QSqlQueryModel(parent),
    m_db(db)
{
}

void ModelHouse::refresh()
{
    clear();
//qDebug()<<"ModelHouse::refresh()"<<aoguid;
    QSqlQuery qry(m_db);
    QString strSql;
    strSql = "select case \
                         when hs.housenum !=' ' and hs.buildnum = ' ' and hs.strucnum = ' ' \
                             then 'д. ' || hs.housenum \
                         when hs.housenum !=' ' and hs.buildnum !=' ' and hs.strucnum = ' ' \
                             then 'д. ' || hs.housenum || ' корп. ' || hs.buildnum \
                         when hs.housenum !=' ' and hs.buildnum !=' ' and hs.strucnum != ' ' \
                             then 'д. ' || hs.housenum || ' корп. ' || hs.buildnum || ' стр. ' || hs.strucnum \
                         when hs.housenum =' ' and hs.buildnum !=' ' and hs.strucnum != ' ' \
                             then 'корп. ' || hs.buildnum || ' стр. ' || hs.strucnum \
                         when hs.housenum =' ' and hs.buildnum =' ' and hs.strucnum != ' ' \
                             then 'стр. ' || hs.strucnum \
                         when hs.housenum =' ' and hs.buildnum !=' ' and hs.strucnum = ' ' \
                             then 'корп. ' || hs.buildnum \
                         when hs.housenum !=' ' and hs.buildnum =' ' and hs.strucnum != ' ' \
                             then 'д. ' || hs.housenum || ' стр. ' || hs.strucnum \
                     end \
             from house hs \
             where hs.aoguid = :aoguid \
                and CURRENT_DATE<=hs.enddate \
             order by 1";
//qDebug()<<strSql<<aoguid;
    qry.prepare(strSql);
    qry.bindValue(":aoguid",aoguid);
    if(!qry.exec()){
        QMessageBox::critical(NULL,"ОШИБКА",qry.lastError().text());
        return;
    }
    setQuery(qry);
    setHeaderData(0,Qt::Horizontal,tr("Дома (кол-во %1)").arg(rowCount()));
}

void ModelHouse::setAoguid(const QString &aoguid_)
{
    aoguid = aoguid_;
}

Qt::ItemFlags ModelHouse::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    return flags;

}

QVariant ModelHouse::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() ||
       index.row() < 0  || index.row() >= this->rowCount() ||
       index.column()<0 || index.column()>=this->columnCount())
    {
        return QVariant();
    }

    QVariant value = QSqlQueryModel::data(index, role);
    switch(role){
    case Qt::DisplayRole:
        return value;
    }

    return value;
}
