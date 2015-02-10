#ifndef MODELLCL_H
#define MODELLCL_H

#include <QSqlQueryModel>
#include <QSqlDatabase>

class ModelLcl : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ModelLcl(const QSqlDatabase& db, QObject *parent = 0);
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

#endif // MODELLCL_H
