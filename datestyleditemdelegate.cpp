#include "datestyleditemdelegate.h"
#include <QDateEdit>
//#include <QDateTimeEdit>
#include <QDate>
#include <QCalendarWidget>
#include <QApplication>
#include <QStyleOptionViewItem>

#include <QTableView>

#include <QDebug>
DateStyledItemDelegate::DateStyledItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    qDebug()<<"ok"<<qobject_cast<QTableView*>(parent);
}

QWidget *DateStyledItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid() && index.column() == 0){
        QDateEdit* editor = new QDateEdit(parent);
//        QDate value = index.model()->index(index.row(),5).data().toDate();
//        editor->setDate(value);
        editor->setCalendarPopup(true);
        QCalendarWidget* cw = new QCalendarWidget();
        editor->setCalendarWidget(cw);
        editor->setDisplayFormat("dd.MM.yyyy");
        editor->installEventFilter(const_cast<DateStyledItemDelegate*>(this));

        return editor;
    }
    return QStyledItemDelegate::createEditor(parent,option,index);

}

void DateStyledItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.isValid() && index.column() == 0){
        QDate value = QDate::fromString(index.data().toString(),"dd.MM.yyyy");//index.model()->index(index.row(),0).data().toDate();
        QDateEdit* dt = qobject_cast<QDateEdit*>(editor);
        dt->setDate(value);
    }
}

void DateStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
qDebug()<<"DateStyledItemDelegate::setModelData";
    QDateEdit* dt = qobject_cast<QDateEdit*>(editor);
    model->setData(index,dt->date(),Qt::EditRole);
}

//void DateStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    if(index.isValid() && index.column() == 0){
//        QStyleOptionViewItemV4 opt = QStyleOptionViewItemV4(option);
//        initStyleOption(&opt,index);
//        const QWidget *wgt = opt.widget;
//qDebug()<<"DateStyledItemDelegate::paint"<<wgt<<index.data().toDateTime();
//        QStyle* style  = wgt ? wgt->style() : QApplication::style();
//        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
// //        style->drawComplexControl(QStyle::CC_ToolButton,&opt,painter);
//    }
//}
