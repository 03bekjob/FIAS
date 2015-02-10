#include "modeltwnstreet.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>

#include <QDebug>

ModelTwnStreet::ModelTwnStreet(const QSqlDatabase &db, const QString& aoguid_, const int& aol_, QObject *parent) :
    QSqlQueryModel(parent)
  ,m_db(db)
  ,aoguid(aoguid_)
  ,aol(aol_)
{
    qDebug()<<"ModelTwnStreet::ModelTwnStreet"<<aoguid<<aol;
}

void ModelTwnStreet::refresh()
{
//qDebug()<<"ModelTwnStreet::refresh()";
    clear();

    QSqlQuery qry(m_db);
    QString strSql;

    strSql = "with recursive sh(addr, \"aoguid\", \"parentguid\" ,\"aolevel\", \"level\", \"updatedate\") as ( \
            select cast(ao.shortname || '. ' || ao.formalname as varchar(2000)) addr \
                ,ao.aoguid \
                ,ao.parentguid \
                ,ao.aolevel \
                ,1 \
                ,ao.updatedate \
            from addrobj ao \
            where ao.aoguid = :aoguid \
                and ao.currstatus = 0 \
            union \
            select cast(aor.shortname || '. ' || aor.formalname as varchar(2000)) addr \
                ,aor.aoguid \
                ,aor.parentguid \
                ,aor.aolevel \
                ,sh.level+1 \
                ,aor.updatedate \
            from addrobj aor inner join sh on sh.aoguid = aor.parentguid \
            where aor.currstatus = 0 and aor.aolevel = :aol and aor.parentguid = :aoguid \
        ) \
        select sh.aoguid \
            ,sh.parentguid \
            ,sh.addr \
            ,sh.aolevel \
            ,sh.level \
            ,sh.updatedate \
        from sh \
        where sh.aolevel not in ( 4) \
        order by sh.addr, sh.aolevel, sh.level"; // sh.addr desc

    qry.prepare(strSql);
    qry.bindValue(":aoguid",aoguid);
    qry.bindValue(":aol",aol);
    if(!qry.exec()){
            QMessageBox::critical(NULL,"ВНИМАНИЕ",qry.lastError().text());
            return;
    }
    setQuery(qry);

    setHeaderData(2,Qt::Horizontal,tr("Улицы (кол-во %1)").arg(rowCount()));
    setHeaderData(5,Qt::Horizontal,tr("Дата"));


}

void ModelTwnStreet::setAoguid(const QString &aoguid_)
{
    aoguid = aoguid_;
}

void ModelTwnStreet::setAolevel(const int &aolevel)
{
    aol = aolevel;
}

Qt::ItemFlags ModelTwnStreet::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    return flags;

}

QVariant ModelTwnStreet::data(const QModelIndex &index, int role) const
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
//        if(index.column() == 5){
//            QString strDate= value.toDate().toString("dd.MM.yyyy");

//            return strDate ;//QDate::fromString(strDate,"dd.MM.yyyy"); //value.toDate();
//        }
        return value;
    }

    return value;
}
