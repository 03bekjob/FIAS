#include "checkboxstyleditemdelegate.h"
#include <QCheckBox>
#include <QApplication>
CheckBoxStyledItemDelegate::CheckBoxStyledItemDelegate(QWidget *parent):
    QStyledItemDelegate(parent)
{

}

CheckBoxStyledItemDelegate::~CheckBoxStyledItemDelegate()
{

}

QWidget *CheckBoxStyledItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid()){
        QCheckBox* editor = new QCheckBox(parent);
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent,option,index);
}

void CheckBoxStyledItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.isValid()){
        QCheckBox* chBox = static_cast<QCheckBox*>(editor);
        bool isCheck = index.data().toBool();
        chBox->setCheckState(isCheck ? Qt::Checked:Qt::Unchecked);
    }
    else{
        QStyledItemDelegate::setEditorData(editor,index);
    }
}

void CheckBoxStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.isValid()){
        QCheckBox* chBox = static_cast<QCheckBox*>(editor);
        int iCh = chBox->isChecked() ? 1:0;
        model->setData(index,iCh);
    }
    else{
        QStyledItemDelegate::setModelData(editor,model,index);
    }
}

void CheckBoxStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid()){
        QStyleOptionButton styleCheckBox;
        QString txt = index.data().toBool() ? tr("Выполнено"):tr("Невыполнено");
        styleCheckBox.rect  = option.rect;
        styleCheckBox.text  = txt;
        styleCheckBox.state = QStyle::State_Enabled;
        styleCheckBox.state|= index.data().toBool() ? QStyle::State_On:QStyle::State_Off;
        styleCheckBox.features|=QStyleOptionButton::Flat;

        QApplication::style()->drawControl(QStyle::CE_CheckBox,&styleCheckBox,painter);

    }
    else{
        QStyledItemDelegate::paint(painter,option,index);
    }
}

