#ifndef MODELLCLSTREET_H
#define MODELLCLSTREET_H

#include <QSqlQueryModel>
#include <QSqlDatabase>

class ModelLclStreet : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ModelLclStreet(const QSqlDatabase& db, QObject *parent = 0);
    void refresh();
    void setAoguid(const QString& aoguid_);
    void setAolevel(const int& aolevel);
signals:

public slots:
private:
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QSqlDatabase m_db;
    QString aoguid;
    int aol;

};

#endif // MODELLCLSTREET_H
