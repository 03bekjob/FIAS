#ifndef MODELTWNSTREET_H
#define MODELTWNSTREET_H

#include <QSqlQueryModel>

class ModelTwnStreet : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ModelTwnStreet(const QSqlDatabase& db, const QString& aoguid_, const int& aol_, QObject *parent = 0);
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

#endif // MODELTWNSTREET_H
