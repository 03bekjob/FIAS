#ifndef DATESTYLEDITEMDELEGATE_H
#define DATESTYLEDITEMDELEGATE_H

//#include <QStyledItemDelegate>
#include <QStyledItemDelegate>
class DateStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateStyledItemDelegate(QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
//    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

};

#endif // DATESTYLEDITEMDELEGATE_H
