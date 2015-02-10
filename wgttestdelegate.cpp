#include "wgttestdelegate.h"
#include "ui_wgttestdelegate.h"
#include <QMessageBox>
#include "datestyleditemdelegate.h"
#include "dateitemdelegate.h"
#include "checkboxstyleditemdelegate.h"

//#include "testtableview.h"

WgtTestDelegate::WgtTestDelegate(const QSqlDatabase &db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgtTestDelegate),
    m_db(db)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    if(!m_db.isValid()){
        QMessageBox::critical(this, tr("Ошибка"), tr("Пожалуйста, прежде соеденитесь с БАЗОЙ"));
        return;
    }
    if(!m_db.isOpen()){
        QMessageBox::critical(this, tr("Ошибка"), tr("Соединения нет!!!\nПовторите процедуру Соединения"));
        return;
    }
    QString sTtl;
    sTtl = tr("%1").arg(this->windowTitle().trimmed());

    act = new QAction(sTtl,this);
    act->setCheckable(true);

    createConnect();

    mdlTestDelegate = new ModelTestDelegate(m_db,parent);
    mdlTestDelegate->refresh();
    ui->tableView->setModel(mdlTestDelegate);
    DateStyledItemDelegate* dtDelegate = new DateStyledItemDelegate(ui->tableView);
    //DateItemDelegate* dtDelegate = new DateItemDelegate();
    ui->tableView->setItemDelegateForColumn(0,dtDelegate);
    CheckBoxStyledItemDelegate* chDelegate = new CheckBoxStyledItemDelegate(ui->tableView);
    ui->tableView->setItemDelegateForColumn(1,chDelegate);
//    TestTableView* vw = new TestTableView(this);
//    vw->setModel(mdlTestDelegate);
//    vw->resizeRowsToContents();
//    vw->resizeColumnsToContents();
//    vw->show();

//    ui->horizontalLayout_2->addWidget(vw);

}

WgtTestDelegate::~WgtTestDelegate()
{
    delete ui;
}

void WgtTestDelegate::createConnect()
{
    connect(act,SIGNAL(triggered()),this,SLOT(showMaximized()));
    connect(ui->pushButton_Exit,SIGNAL(clicked()),this,SLOT(sltExit()));


}

void WgtTestDelegate::sltExit()
{
    close();
    emit sgnExit();

}
