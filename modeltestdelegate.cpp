#include "modeltestdelegate.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDate>

#include <QDebug>
ModelTestDelegate::ModelTestDelegate(const QSqlDatabase &db, QObject *parent) :
    QSqlQueryModel(parent)
    ,m_db(db)
{

}

ModelTestDelegate::~ModelTestDelegate()
{
    clear();
}

void ModelTestDelegate::refresh()
{
    clear();
//Проверить не использовать QSqlQuery !!!!
//    QSqlQuery qry(m_db);
    QString strSql;

    strSql = "select td.dt \
                    ,td.ch \
                    ,td.id \
              from test_delegate td \
              order by td.id";
//    qry.prepare(strSql);
//    if(!qry.exec()){
//            QMessageBox::critical(NULL,"ВНИМАНИЕ",qry.lastError().text());
//            return;
//    }
//    setQuery(qry);
    setQuery(strSql,m_db);
    if(lastError().isValid()){
        QMessageBox::critical(NULL,"ОШИБКА",lastError().text());

    }

    setHeaderData(0,Qt::Horizontal,tr("Дата "));
    setHeaderData(1,Qt::Horizontal,tr("Да/нет"));
    setHeaderData(2,Qt::Horizontal,tr("Ид"));


}

QVariant ModelTestDelegate::data(const QModelIndex &index, int role) const
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
    case Qt::EditRole:
        if(index.column() == 0){

            return value.toDate().toString("dd.MM.yyyy");
        }
        return value;
    }

    return value;

}

Qt::ItemFlags ModelTestDelegate::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QSqlQueryModel::flags(index);
    if(index.column() == 0){
        theFlags|=Qt::ItemIsEditable;
    }
    if(index.column()== 1){
        theFlags|=Qt::ItemIsEditable;
        theFlags|=Qt::ItemIsUserCheckable;
    }
    return theFlags;

}

bool ModelTestDelegate::setData(const QModelIndex &index, const QVariant &value, int role)
{
//qDebug()<<"ModelTestDelegate::setData";
    if(index.isValid() /*&& (index.column() <0 || index.column()>3)*/){
        QModelIndex primaryKey = QSqlQueryModel::index(index.row(),2);
        int id = QSqlQueryModel::data(primaryKey).toInt();
        bool ok = false;
        QSqlQuery qry(m_db);
        switch (index.column()) {
        case 0:
            qry.prepare("update test_delegate as td set dt = ? where td.id = ?");
            qry.addBindValue(value);
            qry.addBindValue(id);
            break;
        case 1:
            qry.prepare("update test_delegate as td set ch = ? where td.id = ?");
            qry.addBindValue(value);
            qry.addBindValue(id);
            break;
        default:
            return false;
        }
        ok = qry.exec();
//qDebug()<<"ModelTestDelegate::setData"<<ok<<qry.lastError().text();
        if(!ok){
            QMessageBox::critical(NULL,tr("ОШИБКА"),qry.lastError().text());
            qry.clear();
            return ok;
        }
        refresh();
        qry.clear();
        return ok;
    }
    return false;
}

