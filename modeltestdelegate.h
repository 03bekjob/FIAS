#ifndef MODELTESTDELEGATE_H
#define MODELTESTDELEGATE_H

#include <QObject>
#include <QSqlQueryModel>

class ModelTestDelegate : public QSqlQueryModel
{
public:
    ModelTestDelegate(const QSqlDatabase& db, QObject* parent = 0);
    ~ModelTestDelegate();
    void refresh();
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
private:
    QSqlDatabase m_db;
};

#endif // MODELTESTDELEGATE_H
