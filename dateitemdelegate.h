#ifndef DATEITEMDELEGATE_H
#define DATEITEMDELEGATE_H

//#include <QObject>
#include <QItemDelegate>
class DateItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DateItemDelegate(QWidget* parent=0);
    ~DateItemDelegate();
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DATEITEMDELEGATE_H
