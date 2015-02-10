#ifndef MODELREGIONS_H
#define MODELREGIONS_H

#include <QSqlQueryModel>
#include <QSqlDatabase>


class ModelRegions : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ModelRegions(const QSqlDatabase& db, QObject *parent = 0);
    void refresh();
signals:

public slots:
private:

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role) const;
    QSqlDatabase m_db;

};

#endif // MODELREGIONS_H
