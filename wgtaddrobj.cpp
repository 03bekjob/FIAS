#include "wgtaddrobj.h"
#include "ui_wgtaddrobj.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include "datestyleditemdelegate.h"

#include <QDebug>

WgtAddrObj::WgtAddrObj(const QSqlDatabase &db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgtAddrObj),
    m_db(db),
    mdlCmbRegions(new QSqlQueryModel(this)),
    mdlCmbArea(new QSqlQueryModel(this)),
    mdlCmbTowns(new QSqlQueryModel(this)),
    mdlCmbStreet(new QSqlQueryModel(this))
    //mdlTwnStreet(new QSqlQueryModel(this))
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->label_street->setVisible(false);
    ui->comboBox_Street->setVisible(false);

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

    mdlTwnStreet = new ModelTwnStreet(m_db,"",7,this);
    ui->tableView->setModel(mdlTwnStreet);
    DateStyledItemDelegate* dateDlgt = new DateStyledItemDelegate(ui->tableView);
    ui->tableView->setItemDelegateForColumn(5,dateDlgt);
//    ui->tableView->setItemDelegate(dateDlgt);

    mdlHouse = new ModelHouse(m_db,this);
    ui->tableView_3->setModel(mdlHouse);

    mdlLcl = new ModelLcl(m_db,this);
    ui->tableView_4->setModel(mdlLcl);

    mdlLclStreet = new ModelLclStreet(m_db,this);
    ui->tableView_5->setModel(mdlLclStreet);

    mdlHouseLcl = new ModelHouse(m_db,this);
    ui->tableView_6->setModel(mdlHouseLcl);

    refreshRegions();
    refreshArea();
    //refreshTowns();


    createConnect();
}

WgtAddrObj::~WgtAddrObj()
{
    delete ui;
}

void WgtAddrObj::createConnect()
{
    connect(act,SIGNAL(triggered()),this,SLOT(showMaximized()));
    connect(ui->pushButton_Exit,SIGNAL(clicked()),this,SLOT(sltExit()));

    connect(ui->comboBox_Regions,SIGNAL(currentIndexChanged(QString)),this,SLOT(sltChangedRegions(QString)));
    connect(ui->comboBox_Area,SIGNAL(currentIndexChanged(QString)),this,SLOT(sltChangedArea(QString)));

    connect(ui->tableView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(sltChangedTwnStreet(QModelIndex,QModelIndex)));
    connect(ui->tableView_4->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(sltChangedLcl(QModelIndex,QModelIndex)));
    connect(ui->tableView_5->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(sltChangedLclStreet(QModelIndex,QModelIndex)));
}

void WgtAddrObj::refreshRegions()
{
    QString strSql;
    strSql = "select ao.regioncode \
            ,left(ao.offname,40) || ' ' || left(ao.shortName,8) || ' '  || ao.regioncode \
     from addrobj ao \
     where ao.parentguid = '' \
            and ao.currstatus = 0 \
     order by ao.regioncode";

    mdlCmbRegions->clear();
    mdlCmbRegions->setQuery(strSql,m_db);
    ui->comboBox_Regions->setModel(mdlCmbRegions);
    ui->comboBox_Regions->setModelColumn(1);

}

void WgtAddrObj::refreshArea()
{

    QSqlRecord rec = mdlCmbRegions->record(ui->comboBox_Regions->currentIndex());

    QString strSql;
    QSqlQuery qry(m_db);
    int aol = 3;
    int aola= 4;
    QString rgn = "";
    rgn = rec.value(0).toString();
/*    strSql = "with recursive t1(addr,\"parentguid\",\"aoguid\",\"aolevel\",\"level\") as ( \
            select distinct cast(aobj.shortname || '. ' || aobj.formalname as varchar(2000)) as addr \
                  ,aobj.parentguid \
                  ,aobj.aoguid \
                  ,aobj.aolevel \
                  ,1 \
            from addrobj aobj \
            where aobj.regioncode = :rgn \
                and aobj.aolevel in (:aol) \
                and aobj.currstatus = 0 \
            union \
            select cast( aobju.shortname || '. ' || aobju.formalname || ' ' || t1.addr as varchar(2000)) \
                   ,aobju.parentguid \
                   ,aobju.aoguid \
                   ,aobju.aolevel \
                   ,t1.level + 1 \
            from addrobj aobju inner join t1 on (aobju.aoguid = t1.parentguid) \
            where aobju.currstatus =0 \
        ) \
        select t1.aoguid ,t1.addr \
        from t1 \
        where t1.aolevel in (:aol) \
            and t1.level in ( 1) \
        order by t1.aolevel ,t1.level,t1.addr";
    qry.prepare(strSql);
    qry.bindValue(":rgn",rgn);
    qry.bindValue(":aol",aol);
*/
    strSql = "with recursive tt(addr,\"aoguid\",\"parentguid\",\"aolevel\",\"level\") as( \
                select cast(ao.shortname || '. ' || ao.formalname as varchar(2000)) as addr \
                        ,ao.aoguid \
                        ,ao.parentguid \
                        ,ao.aolevel \
                        ,1 \
                from addrobj ao \
                where ao.regioncode = :rgn \
                    and ao.parentguid= '' \
                union \
                select cast(aor.shortname || '. ' || aor.formalname as varchar(2000)) \
                        ,aor.aoguid \
                        ,aor.parentguid \
                        ,aor.aolevel \
                        ,level+1 \
                from addrobj aor inner join tt on tt.aoguid = aor.parentguid \
                where aor.currstatus = 0 \
                    and aor.aolevel in ( :aola) \
            ), \
            t1(addr,\"parentguid\",\"aoguid\",\"aolevel\",\"level\") as ( \
                select distinct cast(aobj.shortname || '. ' || aobj.formalname as varchar(2000)) as addr \
                        ,aobj.parentguid \
                        ,aobj.aoguid \
                        ,aobj.aolevel \
                        ,1 \
                from addrobj aobj \
                where aobj.regioncode = :rgn \
                    and aobj.aolevel in (:aol) \
                    and aobj.currstatus = 0 \
                union \
                select cast( aobju.shortname || '. ' || aobju.formalname || ' ' || t1.addr as varchar(2000)) \
                        ,aobju.parentguid \
                        ,aobju.aoguid \
                        ,aobju.aolevel \
                        ,t1.level + 1 \
                from addrobj aobju inner join t1 on (aobju.aoguid = t1.parentguid) \
                where aobju.currstatus =0 \
            ) \
            select tt.aoguid,tt.addr,tt.aolevel,tt.level \
            from tt \
            where tt.aolevel !=1 and tt.level !=1 \
            union \
            select t1.aoguid,t1.addr, t1.aolevel,t1.level \
            from t1 \
            where t1.aolevel in (:aol) \
                and  t1.level in ( 1) \
            order by 3 desc,4 desc,2 asc";
            qry.prepare(strSql);
            qry.bindValue(":rgn",rgn);
            qry.bindValue(":aola",aola);
            qry.bindValue(":aol",aol);

    if(!qry.exec()){
            QMessageBox::critical(NULL,"ВНИМАНИЕ",qry.lastError().text());
            return;
    }



    mdlCmbArea->clear();
    mdlCmbArea->setQuery(qry);
//qDebug()<<mdlCmbArea->lastError()<<qry.lastQuery();
    ui->comboBox_Area->setModel(mdlCmbArea);
    ui->comboBox_Area->setModelColumn(1);

}

void WgtAddrObj::refreshTowns()
{
    QSqlRecord rec = mdlCmbArea->record(ui->comboBox_Area->currentIndex());
    QString aoguid = "";
    int aol = 7;
    aoguid = rec.value(0).toString();
qDebug()<<"WgtAddrObj::refreshTowns"<<aoguid;
/*    QString strSql;
    strSql = "select  aobj.aoguid \
            ,aobj.offname || ' ' || aobj.shortname \
            from addrobj aobj \
            where aobj.regioncode = '24' \
            and aobj.aolevel in (4,6) \
            and aobj.shortname not in ('волость','п/о','с/а','с/с','снт','тер','с/о','с/мо','аал','автодорога','арбан', \
                                       'высел','ж/д_будка','ж/д_казарм','ж/д_оп','ж/д_платф','ж/д_пост','ж/д_рзд','ж/д_ст', \
                                       'жилзона','жилрайон','заимка','казарма','кв-л','кордон','мкр','нп','п/р','починок','ж/д_ст','рзд', \
                                       'погост','сл','ст','массив','м','промзона') \
            and aobj.actstatus = 1 \
            order by aobj.offname , aobj.shortname";

    mdlCmbTowns->clear();
    mdlCmbTowns->setQuery(strSql,m_db);
    ui->comboBox_Towns->setModel(mdlCmbTowns);
    ui->comboBox_Towns->setModelColumn(1);
*/
}

void WgtAddrObj::refreshStreet()
{

}

void WgtAddrObj::refreshTwnStreet()
{
    QSqlRecord rec = mdlCmbArea->record(ui->comboBox_Area->currentIndex());
    QString aoguid = "";
    int aol = 7;
    aoguid = rec.value(0).toString();
    mdlTwnStreet->setAoguid(aoguid);
    mdlTwnStreet->setAolevel(aol);
    mdlTwnStreet->refresh();

}

void WgtAddrObj::resizeTwnStreetTV()
{
    ui->tableView->setColumnHidden(0,true);
    ui->tableView->setColumnHidden(1,true);
    ui->tableView->setColumnHidden(3,true);
    ui->tableView->setColumnHidden(4,true);

    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

void WgtAddrObj::refreshHouse()
{
    mdlHouse->refresh();
    ui->tableView_3->setModel(mdlHouse);
}

void WgtAddrObj::resizeHouseTV3()
{
    ui->tableView_3->horizontalHeader()->setStretchLastSection(true);
}

void WgtAddrObj::refreshLcl()
{
    QSqlRecord rec = mdlCmbArea->record(ui->comboBox_Area->currentIndex());
    QString aoguid = "";
    //int aol = 7;
    aoguid = rec.value(0).toString();
    mdlLcl->setAoguid(aoguid);
    mdlLcl->refresh();
}

void WgtAddrObj::resizeLclTV4()
{
    ui->tableView_4->setColumnHidden(0,true);
    ui->tableView_4->setColumnHidden(1,true);
    ui->tableView_4->setColumnHidden(3,true);
    ui->tableView_4->setColumnHidden(4,true);

    ui->tableView_4->horizontalHeader()->setStretchLastSection(true);
}

void WgtAddrObj::refreshLclStreet()
{
    mdlLclStreet->refresh();
}

void WgtAddrObj::resizeLclStreetTV5()
{
    ui->tableView_5->setColumnHidden(0,true);
    ui->tableView_5->setColumnHidden(1,true);
    ui->tableView_5->setColumnHidden(3,true);
    ui->tableView_5->setColumnHidden(4,true);

    ui->tableView_5->horizontalHeader()->setStretchLastSection(true);

}

void WgtAddrObj::refreshHouseLcl()
{
    mdlHouseLcl->refresh();
}

void WgtAddrObj::resizeHouseLclTV6()
{
    ui->tableView_6->horizontalHeader()->setStretchLastSection(true);
}

void WgtAddrObj::switchTableView(const int &aolevel)
{
    switch (aolevel) {
    case 3:  //районы
        ui->tableView_2->setVisible(true);
        break;
    case 4: //города
        ui->tableView_2->setVisible(false);
        break;
    }
}

void WgtAddrObj::sltExit()
{
    close();
    emit sgnExit();

}

void WgtAddrObj::sltChangedRegions(const QString &region)
{
//qDebug()<<region;
    refreshArea();
}

void WgtAddrObj::sltChangedArea(const QString &area)
{
//qDebug()<<"WgtAddrObj::sltChangedArea"<<area;
    QSqlRecord rec = mdlCmbArea->record(ui->comboBox_Area->currentIndex());
    QString aoguid = "";
    bool ok=false;
    int aolevel = -1;
    aoguid = rec.value(0).toString();
    aolevel= rec.value(2).toInt(&ok);
//qDebug()<<"WgtAddrObj::sltChangedArea"<<aoguid<<aolevel;
    if(ok){
        modeAolevel = aolevel;
        switchTableView(modeAolevel);
        switch(modeAolevel){
        case 3: //районы
            if(mdlTwnStreet){
                mdlTwnStreet->clear();
                //ui->tableView->setModel(NULL);
                //resizeTwnStreetTV();
            }
            break;
        case 4: //города
            refreshTwnStreet();
            resizeTwnStreetTV();

            refreshLcl();
            resizeLclTV4();
            break;
        }
    }

    //refreshTowns();
}

void WgtAddrObj::sltChangedTwnStreet(const QModelIndex &cur, const QModelIndex &prev)
{
//qDebug()<<"WgtAddrObj::sltChangedTwnStreet"<<modeAolevel;
    QString aoguid = "";
    switch(modeAolevel){
    case 3:
        break;
    case 4:
        if(cur.isValid()){
            //aoguid  = cur.model()->index(cur.row(),0).data().toString();
            aoguid  = mdlTwnStreet->index(cur.row(),0).data().toString();
            mdlHouse->setAoguid(aoguid);
            refreshHouse();
            resizeHouseTV3();
        }
        break;
    }
}

void WgtAddrObj::sltChangedLcl(const QModelIndex &cur, const QModelIndex &prev)
{
    QString aoguid = "";
    int aol = 7;
    switch(modeAolevel){
    case 3:
        break;
    case 4:
        if(cur.isValid()){
            //aoguid  = cur.model()->index(cur.row(),0).data().toString();
            aoguid  = mdlLcl->index(cur.row(),0).data().toString();
            qDebug()<<"WgtAddrObj::sltChangedLcl"<<aoguid;
            mdlLclStreet->setAoguid(aoguid);
            mdlLclStreet->setAolevel(aol);
            refreshLclStreet();
            resizeLclStreetTV5();
        }
        break;
    }

}

void WgtAddrObj::sltChangedLclStreet(const QModelIndex &cur, const QModelIndex &prev)
{
    QString aoguid = "";
    switch(modeAolevel){
    case 3:
        break;
    case 4:
        if(cur.isValid()){
            //aoguid  = cur.model()->index(cur.row(),0).data().toString();
            aoguid  = mdlLclStreet->index(cur.row(),0).data().toString();
            mdlHouseLcl->setAoguid(aoguid);
            refreshHouseLcl();
            resizeHouseLclTV6();
        }
        break;
    }

}
