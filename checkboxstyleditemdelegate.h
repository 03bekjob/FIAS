#ifndef CHECKBOXSTYLEDITEMDELEGATE_H
#define CHECKBOXSTYLEDITEMDELEGATE_H

#include <QStyledItemDelegate>

class CheckBoxStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CheckBoxStyledItemDelegate(QWidget* parent=0);
    ~CheckBoxStyledItemDelegate();
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // CHECKBOXSTYLEDITEMDELEGATE_H
