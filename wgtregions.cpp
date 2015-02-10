#include "wgtregions.h"
#include "ui_wgtregions.h"

#include <QMessageBox>
#include "modelregions.h"
#include <QHeaderView>

#include <QDebug>

WgtRegions::WgtRegions(const QSqlDatabase &db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgtRegions),
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

    mRegions = new ModelRegions(m_db,this);
    mProxy = new  QSortFilterProxyModel(this);
    mProxy->setSourceModel(mRegions);
    ui->tableView->setModel(mProxy);
    //ui->tableView->setModel(mRegions);

    refresh();
    resizeView();
    createConnect();
}

WgtRegions::~WgtRegions()
{
    delete ui;

}

void WgtRegions::createConnect()
{
    connect(act,SIGNAL(triggered()),this,SLOT(showMaximized()));
    connect(ui->pushButton_Exit,SIGNAL(clicked()),this,SLOT(sltExit()));
}

void WgtRegions::refresh()
{
    mRegions->refresh();
    resizeView();
}

void WgtRegions::resizeView()
{
    ui->tableView->hideColumn(4);
    ui->tableView->hideColumn(5);
    ui->tableView->hideColumn(6);

    ui->tableView->verticalHeader()->hide();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

}

void WgtRegions::sltExit()
{
    close();
    emit sgnExit();

}
