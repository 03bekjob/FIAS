#include "dateitemdelegate.h"
#include <QDateEdit>
#include <QDate>
#include <QCalendarWidget>

#include <QDebug>
#include "testtableview.h"
DateItemDelegate::DateItemDelegate(QWidget *parent)
    :QItemDelegate(parent)
{
qDebug()<<"DateItemDelegate::DateItemDelegate"<<(qobject_cast<TestTableView*>(parent));
}

DateItemDelegate::~DateItemDelegate()
{

}

QWidget *DateItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QDateEdit *editor = new QDateEdit(parent);
    editor->setCalendarPopup(true);
    editor->installEventFilter(const_cast<DateItemDelegate*>(this));
    return editor;
}

void DateItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.isValid() && index.column() == 0){
        QDate value = QDate::fromString(index.data().toString(),"dd.MM.yyyy"); //index.model()->data(index, Qt::EditRole).toDate();
        QDateEdit *de = static_cast<QDateEdit*>(editor);
//qDebug()<<value;
        de->setDate(value);
    }
}

void DateItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.isValid() && index.column() == 0){
        QDateEdit *de = static_cast<QDateEdit*>(editor);
        de->interpretText();
        QDate value = de->date();
qDebug()<<"DateItemDelegate::setModelData "<<value;
        if (model->setData(index, value)){
                qDebug()<<"fasle";
        }
    }

}

void DateItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

