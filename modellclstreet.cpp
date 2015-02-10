#include "modellclstreet.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

ModelLclStreet::ModelLclStreet(const QSqlDatabase &db, QObject *parent) :
    QSqlQueryModel(parent)
  ,m_db(db)
{
}

void ModelLclStreet::refresh()
{
    clear();

    QSqlQuery qry(m_db);

    QString strSql = "";
    strSql ="with recursive sh(addr, \"aoguid\",\"parentguid\",\"aolevel\",\"level\") as ( \
                select cast(ao.shortname || '. ' || ao.formalname as varchar(2000)) addr \
                    ,ao.aoguid \
                    ,ao.parentguid \
                    ,ao.aolevel \
                    ,1 \
                from addrobj ao \
                where ao.aoguid = :aoguid \
                    and ao.currstatus = 0 \
                union \
                select  cast(aor.shortname || '. ' || aor.formalname as varchar(2000)) addr \
                    ,aor.aoguid \
                    ,aor.parentguid \
                    ,aor.aolevel \
                    ,sh.level+1 \
                from addrobj aor inner join sh on sh.aoguid = aor.parentguid \
                where aor.currstatus = 0 and aor.aolevel = :aol and aor.parentguid = :aoguid \
            ) \
            select sh.aoguid \
                ,sh.parentguid \
                ,sh.addr \
                ,sh.aolevel \
                ,sh.level \
            from sh \
            where sh.aolevel not in ( 6,90) \
            order by sh.addr, sh.aolevel, sh.level";
    qry.prepare(strSql);
    qry.bindValue(":aoguid",aoguid);
    qry.bindValue(":aol",aol);
    if(!qry.exec()){
            QMessageBox::critical(NULL,"ВНИМАНИЕ",qry.lastError().text());
            return;
    }
    setQuery(qry);

    setHeaderData(2,Qt::Horizontal,tr("Улицы (кол-во %1)").arg(rowCount()));

}

void ModelLclStreet::setAoguid(const QString &aoguid_)
{
    aoguid = aoguid_;
}

void ModelLclStreet::setAolevel(const int &aolevel)
{
    aol = aolevel;
}

Qt::ItemFlags ModelLclStreet::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);

    return flags;
}

QVariant ModelLclStreet::data(const QModelIndex &index, int role) const
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
