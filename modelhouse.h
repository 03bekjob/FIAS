#ifndef MODELHOUSE_H
#define MODELHOUSE_H

#include <QSqlQueryModel>
#include <QSqlDatabase>

class ModelHouse : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ModelHouse(const QSqlDatabase& db, QObject *parent = 0);
    void refresh();
    void setAoguid(const QString& aoguid_);

signals:

public slots:
private:
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QSqlDatabase m_db;
    QString aoguid;
};

#endif // MODELHOUSE_H
