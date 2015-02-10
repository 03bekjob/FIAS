#include "modellcl.h"
#include <QMessageBox>
#include <QSqlError>
ModelLcl::ModelLcl(const QSqlDatabase &db, QObject *parent) :
    QSqlQueryModel(parent)
  ,m_db(db)
{
}

void ModelLcl::refresh()
{
    clear();

    QString strSql= "";
    strSql = "with recursive sh(addr, \"aoguid\",\"parentguid\",\"aolevel\",\"level\") as ( \
                    select cast(ao.shortname || '. ' || ao.formalname as varchar(2000)) addr \
                        ,ao.aoguid \
                        ,ao.parentguid \
                        ,ao.aolevel \
                        ,1 \
                    from addrobj ao \
                    where ao.aoguid = '"+aoguid+"' \
                        and ao.currstatus = 0 \
                    union \
                    select cast(aor.shortname || '. ' || aor.formalname as varchar(2000)) addr \
                        ,aor.aoguid \
                        ,aor.parentguid \
                        ,aor.aolevel \
                        ,sh.level+1 \
                    from addrobj aor inner join sh on sh.aoguid = aor.parentguid \
                    where aor.currstatus = 0 and aor.aolevel in ( 6,90)  and aor.parentguid = '"+aoguid+"' \
                ) \
                select sh.aoguid \
                    ,sh.parentguid \
                    ,sh.addr \
                    ,sh.aolevel \
                    ,sh.level \
                from sh \
                where sh.aolevel not in ( 4) \
                order by sh.aolevel, sh.level, sh.addr desc";
    setQuery(strSql,m_db);
    if(lastError().isValid()){
        QMessageBox::critical(NULL,"ОШИБКА",lastError().text());

    }
    setHeaderData(2,Qt::Horizontal,tr("Подчиненые нас.пункты (кол-во %1)").arg(rowCount()));
}

void ModelLcl::setAoguid(const QString &aoguid_)
{
    aoguid = aoguid_;
}

Qt::ItemFlags ModelLcl::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    return flags;
}

QVariant ModelLcl::data(const QModelIndex &index, int role) const
{
    if( !index.isValid() ||
        index.row()<0    || index.row() >= this->rowCount() ||
        index.column()<0 || index.column() >= this->columnCount() ){
        return QVariant();
    }

    QVariant value = QSqlQueryModel::data(index, role);
    switch(role){
    case Qt::DisplayRole:
        return value;
    }

    return value;
}
