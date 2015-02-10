#include "wgtfilesfias.h"
#include "ui_wgtfilesfias.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QByteArray>
#include <QRegExp>
#include <QTimer>
#include <QFile>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

#include "qdbftable.h"      //qdbf
#include "qdbffield.h"      //qdbf
#include "qdbfrecord.h"     //qdbf

WgtFilesFias::WgtFilesFias(const QSqlDatabase &db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgtFilesFias),
    m_db(db),
    pathFiasSrc("")/*,
    url(QUrl())*/
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->label_Prep->setVisible(false);
    ui->progressBar_Prep->setVisible(false);


    switchVisibleBar(false);

    setAttribute(Qt::WA_DeleteOnClose);
    if(!m_db.isValid()){
        QMessageBox::critical(this, tr("Ошибка"), tr("Пожалуйста, прежде соеденитесь с БАЗОЙ"));
        return;
    }
    if(!m_db.isOpen()){
        QMessageBox::critical(this, tr("Ошибка"), tr("Соединения нет!!!\nПовторите процедуру Соединения"));
        return;
    }
//qDebug()<<"WgtFilesFias::WgtFilesFias 1";
    QString sTtl;
    sTtl = tr("%1").arg(this->windowTitle().trimmed());

    act = new QAction(sTtl,this);
    act->setCheckable(true);

    networkFiasAccess = new QNetworkAccessManager(this);
    networkFilesAccess = new QNetworkAccessManager(this);
    //replyFile = NULL;
    mdlCmbRegions = new QSqlQueryModel(this);

/*    QStyleOptionGroupBox style;
    style.lineWidth = 10;
    ui->groupBox_UpdDb->initStyleOption(&style);
*/
    refreshCmbRegions();

    createConnect();

    readSettings();

    QTimer::singleShot(0,this,SLOT(sltDownLoderInit()));
}

WgtFilesFias::~WgtFilesFias()
{
    //delete replyFile;
    saveSettings();

    delete ui;
}

void WgtFilesFias::createConnect()
{
    connect(act,SIGNAL(triggered()),this,SLOT(showMaximized()));
    connect(ui->pushButton_Exit,SIGNAL(clicked()),this,SLOT(sltExit()));
    connect(ui->pushButton_SaveFile,SIGNAL(clicked()),this,SLOT(sltSelectDirFias()));
    connect(ui->pushButton_DownLoader,SIGNAL(clicked()),this,SLOT(sltRequestFiles()));
    connect(networkFiasAccess,SIGNAL(finished(QNetworkReply*)),this,SLOT(sltReadFiles(QNetworkReply*)));
    connect(networkFilesAccess,SIGNAL(finished(QNetworkReply*)),this,SLOT(sltDownLoaderFiles(QNetworkReply*)));

    connect(ui->pushButton_UpdateDb,SIGNAL(clicked()),this,SLOT(sltUpdateDb()));

    connect(ui->radioButton_Delta,SIGNAL(clicked()),this,SLOT(sltRadioDelta()));
    connect(ui->radioButton_Full,SIGNAL(clicked()),this,SLOT(sltRadioFull()));

    connect(ui->lineEdit_3,SIGNAL(textChanged(QString)),this,SLOT(sltChangeText(QString)));
    connect(ui->lineEdit_CmbRegions,SIGNAL(textChanged(QString)),this,SLOT(sltChangTextL(QString)));
}

void WgtFilesFias::readSettings()
{
    QSettings st;
    //QString url;
    QString key;
    key = tr("WgtFilesFias/pathFias");
    pathFiasSrc = st.value(key).toString().trimmed();
    if(pathFiasSrc.trimmed().isEmpty()){
        pathFiasSrc= tr("http://fias.nalog.ru/Public/DownloadPage.aspx");
    }

    ui->lineEdit->setText(pathFiasSrc);

}

void WgtFilesFias::saveSettings()
{
//qDebug()<<"WgtFilesFias::saveSettings";

    QSettings st;
    if(pathFiasSrc.trimmed().isEmpty()){
        pathFiasSrc= tr("http://fias.nalog.ru/Public/DownloadPage.aspx");
    }
    QString key;
    key = tr("WgtFilesFias/pathFias");
    st.setValue(key,pathFiasSrc);

}

void WgtFilesFias::switchVisibleBar(bool b)
{
//qDebug()<<"WgtFilesFias::switchVisibleBar"<<b;
    if(ui->radioButton_Delta->isChecked()){
        ui->label_Prep->setVisible(b);
        ui->progressBar_Prep->setVisible(b);
    }

    ui->label_Total->setVisible(b);
    ui->progressBar_Files->setVisible(b);
    ui->label_curFile->setVisible(b);
    ui->progressBar_File->setVisible(b);
    if(!b){
        ui->progressBar_Files->reset();
        ui->progressBar_File->reset();
        ui->label_File->setText("");
    }
}

void WgtFilesFias::refreshCmbRegions()
{
    QString strSql;
    bool bDelta = true;
    bDelta = ui->radioButton_Delta->isChecked();
    mdlCmbRegions->clear();
    if(bDelta){
        strSql = "select ar.regioncode \
                        ,left(ar.offname,40) || ' ' || left(ar.shortName,8) || ' '  || ar.regioncode \
                  from addr_regions ar \
                  order by ar.regioncode";
    }
    else{ // полное пополнение возможно только для отсутвующих регионов в addrobj
        strSql = "select ar.regioncode \
                        ,left(ar.offname,40) || ' ' || left(ar.shortName,8) || ' '  || ar.regioncode \
                  from addr_regions ar \
                  where ar.regioncode not in (select distinct aobj.regioncode from addrobj aobj ) \
                  order by ar.regioncode";
    }
    mdlCmbRegions->setQuery(strSql,m_db);
    ui->comboBox_Regions->setModel(mdlCmbRegions);
    ui->comboBox_Regions->setModelColumn(1);
    ui->comboBox_Regions->setLineEdit(ui->lineEdit_CmbRegions);
}

void WgtFilesFias::execBatchAddrobj(QSqlQuery &qry, const QString &strQry, int &iLst)
{
    //QSqlQuery qry = sqlQry;
    qry.prepare(strQry);

    qry.addBindValue( aoidLst);
    qry.addBindValue( formalnameLst);
    qry.addBindValue( regioncodeLst);
    qry.addBindValue( autocodeLst);
    qry.addBindValue( areacodeLst);
    qry.addBindValue( citycodeLst);
    qry.addBindValue( ctarcodeLst);
    qry.addBindValue( placecodeLst);
    qry.addBindValue( streetcodeLst);
    qry.addBindValue( extrcodeLst);
    qry.addBindValue( sextcodeLst);
    qry.addBindValue( offnameLst);
    qry.addBindValue( postalcodeLst);
    qry.addBindValue( okatoLst);
    qry.addBindValue( oktmoLst);
    qry.addBindValue( updatedateLst);
    qry.addBindValue( shortnameLst);
    qry.addBindValue( aolevelLst);
    qry.addBindValue( parentguidLst);
    qry.addBindValue( aoguidLst);
    qry.addBindValue( previdLst);
    qry.addBindValue( nextidLst);
    qry.addBindValue( codeLst);
    qry.addBindValue( plaincodeLst);
    qry.addBindValue( actstatusLst);
    qry.addBindValue( centstatusLst);
    qry.addBindValue( operstatusLst);
    qry.addBindValue( currstatusLst);
    qry.addBindValue( startdateLst);
    qry.addBindValue( enddateLst);

    m_db.transaction();
    if(!qry.execBatch()){
        qDebug()<<qry.lastError();
//                        qDebug()<<qry.lastQuery();
    }
    m_db.commit();
    qry.clear();

    iLst = 0;
    aoidLst.clear();
    formalnameLst.clear();
    regioncodeLst.clear();
    autocodeLst.clear();
    areacodeLst.clear();
    citycodeLst.clear();
    ctarcodeLst.clear();
    placecodeLst.clear();
    streetcodeLst.clear();
    extrcodeLst.clear();
    sextcodeLst.clear();
    offnameLst.clear();
    postalcodeLst.clear();
    okatoLst.clear();
    oktmoLst.clear();
    updatedateLst.clear();
    shortnameLst.clear();
    aolevelLst.clear();
    parentguidLst.clear();
    aoguidLst.clear();
    previdLst.clear();
    nextidLst.clear();
    codeLst.clear();
    plaincodeLst.clear();
    actstatusLst.clear();
    centstatusLst.clear();
    operstatusLst.clear();
    currstatusLst.clear();
    startdateLst.clear();
    enddateLst.clear();
}

void WgtFilesFias::execBatchHouse(QSqlQuery &qry, const QString &strQry, int &iLst)
{
//    QSqlQuery qry = sqlQry;

    qry.prepare(strQry);
    qry.addBindValue( postalcodeLst);
    qry.addBindValue( updatedateLst);
    qry.addBindValue( housenumLst);
    qry.addBindValue( eststatusLst);
    qry.addBindValue( buildnumLst);
    qry.addBindValue( strucnumLst);
    qry.addBindValue( strstatusLst);
    qry.addBindValue( houseidLst);
    qry.addBindValue( houseguidLst);
    qry.addBindValue( aoguidLst);
    qry.addBindValue( startdateLst);
    qry.addBindValue( enddateLst);
    qry.addBindValue( statstatusLst);
    qry.addBindValue( counterLst);

    m_db.transaction();
    if(!qry.execBatch()){
        qDebug()<<qry.lastError();
//                        qDebug()<<qry.lastQuery();
    }
    m_db.commit();
    qry.clear();

    iLst = 0;
    postalcodeLst.clear();
    updatedateLst.clear();
    housenumLst.clear();
    eststatusLst.clear();
    buildnumLst.clear();
    strucnumLst.clear();
    strstatusLst.clear();
    houseidLst.clear();
    houseguidLst.clear();
    aoguidLst.clear();
    startdateLst.clear();
    enddateLst.clear();
    statstatusLst.clear();
    counterLst.clear();

}

void WgtFilesFias::execBatchUpdHouse(QSqlQuery &qry, const QString &strQry, int &iLst)
{
    qry.prepare(strQry);
    qry.addBindValue( postalcodeLstUpd);
    qry.addBindValue( updatedateLstUpd);
    qry.addBindValue( housenumLstUpd);
    qry.addBindValue( eststatusLstUpd);
    qry.addBindValue( buildnumLstUpd);
    qry.addBindValue( strucnumLstUpd);
    qry.addBindValue( strstatusLstUpd);
//    qry.addBindValue( houseidLstUpd);
    qry.addBindValue( houseguidLstUpd);
    qry.addBindValue( aoguidLstUpd);
    qry.addBindValue( startdateLstUpd);
    qry.addBindValue( enddateLstUpd);
    qry.addBindValue( statstatusLstUpd);
    qry.addBindValue( counterLstUpd);
    qry.addBindValue( houseidLstUpd);

    m_db.transaction();
    if(!qry.execBatch()){
        qDebug()<<qry.lastError();
//                        qDebug()<<qry.lastQuery();
    }
    m_db.commit();
    qry.clear();

    iLst = 0;
    postalcodeLstUpd.clear();
    updatedateLstUpd.clear();
    housenumLstUpd.clear();
    eststatusLstUpd.clear();
    buildnumLstUpd.clear();
    strucnumLstUpd.clear();
    strstatusLstUpd.clear();
    houseidLstUpd.clear();
    houseguidLstUpd.clear();
    aoguidLstUpd.clear();
    startdateLstUpd.clear();
    enddateLstUpd.clear();
    statstatusLstUpd.clear();
    counterLstUpd.clear();

}

void WgtFilesFias::execOneAddrobj(QSqlQuery &qry, const QString &strQry)
{
    qry.prepare(strQry);
    qry.bindValue(":aoid",aoid);
    qry.bindValue(":formalname", formalname);
    qry.bindValue(":regioncode", regioncode);
    qry.bindValue(":autocode", autocode);
    qry.bindValue(":areacode", areacode);
    qry.bindValue(":citycode", citycode);
    qry.bindValue(":ctarcode", ctarcode);
    qry.bindValue(":placecode", placecode);
    qry.bindValue(":streetcode", streetcode);
    qry.bindValue(":extrcode", extrcode);
    qry.bindValue(":sextcode", sextcode);
    qry.bindValue(":offname", offname);
    qry.bindValue(":postalcode", postalcode);
    qry.bindValue(":okato", okato);
    qry.bindValue(":oktmo", oktmo);
    qry.bindValue(":updatedate", updatedate);
    qry.bindValue(":shortname", shortname);
    qry.bindValue(":aolevel", aolevel);
    qry.bindValue(":parentguid", parentguid);
    qry.bindValue(":aoguid", aoguid);
    qry.bindValue(":previd", previd);
    qry.bindValue(":nextid", nextid);
    qry.bindValue(":code", code);
    qry.bindValue(":plaincode", plaincode);
    qry.bindValue(":actstatus", actstatus);
    qry.bindValue(":centstatus", centstatus);
    qry.bindValue(":operstatus", operstatus);
    qry.bindValue(":currstatus", currstatus);
    qry.bindValue(":startdate", startdate);
    qry.bindValue(":enddate", enddate);
    if(!qry.exec()){
        //qDebug()<<qry.lastError();
        //qDebug()<<qry.lastQuery();
    }
    qry.clear();
}

void WgtFilesFias::execDelOneAddrobj(QSqlQuery &qry, const QString &strQry)
{
    qry.prepare(strQry);
    qry.bindValue(":aoid",aoid);

    if(!qry.exec()){
        qDebug()<<qry.lastError();
    }
    qry.clear();
}



void WgtFilesFias::execOneHouse(QSqlQuery &qry, const QString &strQry)
{
    qry.prepare(strQry);

    qry.bindValue(":postalcode", postalcode);
    qry.bindValue(":updatedate", updatedate);
    qry.bindValue(":housenum", housenum);
    qry.bindValue(":eststatus", eststatus);
    qry.bindValue(":buildnum", buildnum);
    qry.bindValue(":strucnum", strucnum);
    qry.bindValue(":strstatus", strstatus);
    qry.bindValue(":houseid", houseid);
    qry.bindValue(":houseguid", houseguid);
    qry.bindValue(":aoguid", aoguid);
    qry.bindValue(":startdate", startdate);
    qry.bindValue(":enddate", enddate);
    qry.bindValue(":statstatus", statstatus);
    qry.bindValue(":counter", counter);

//qDebug()<<postalcode<<updatedate<<housenum<<eststatus<<buildnum<<strucnum<<strstatus<<houseid<<houseguid<<aoguid<<startdate<<enddate<<statstatus<<counter;
    if(!qry.exec()){
        qDebug()<<qry.lastError();
//        qDebug()<<"lastQuery = "<<qry.lastQuery();
    }
    qry.clear();
}

void WgtFilesFias::execDelOneHouse(QSqlQuery &qry, const QString &strQry)
{
    qry.prepare(strQry);
    qry.bindValue(":houseid",houseid);

    if(!qry.exec()){
        qDebug()<<qry.lastError();
    }
    qry.clear();

}

void WgtFilesFias::execDelLstHouse(QSqlQuery &qry, const QString &strQry, const QString &strIn)
{
    QString strCurQry = strQry;
    strCurQry+=strIn+")";
//qDebug()<<strCurQry;
//    qry.prepare(strQry);
    qry.prepare(strCurQry);

    if(!qry.exec()){
        qDebug()<<qry.lastError();
    }
    qry.clear();

}

void WgtFilesFias::execOneSocrBase(QSqlQuery &qry, const QString &strQry)
{
//    qDebug()<<"record "<<level<<scname<<socrname<<kod_t_st;
    qry.prepare(strQry);
    qry.bindValue(":level", level);
    qry.bindValue(":scname", scname);
    qry.bindValue(":socrname", socrname);
    qry.bindValue(":kod_t_st", kod_t_st);
    if(!qry.exec()){
        qDebug()<<qry.lastError();
//                    qDebug()<<qry.lastQuery();
    }
    qry.clear();

}




void WgtFilesFias::sltExit()
{
//qDebug()<<"WgtFilesFias::sltExit()";
    this->close();
    emit sgnExit();
}

void WgtFilesFias::sltSelectDirFias()
{
//qDebug()<<"WgtFilesFias::sltSaveFileFias()";

    QString file;
    QString strPath = QApplication::applicationDirPath()+tr("/Source_FIAS");
    QDir dirPath;
    if(!dirPath.exists(strPath)){
        dirPath.mkpath(strPath);
    }
//    file =  QFileDialog::getSaveFileName(this,tr("Сохранить..."),strPath,tr("Файлы (*.DBF)"));
    strPath = QFileDialog::getExistingDirectory(this,tr("Выбирете папку..."),strPath,QFileDialog::ShowDirsOnly);
    ui->lineEdit_2->setText(strPath);
}

void WgtFilesFias::sltReadFiles(QNetworkReply *rply)
{
//    qDebug()<<"WgtFilesFias::sltReadFiles";
    //QByteArray cnt = rply->readAll();
    QString data = rply->readAll().constData();
    QStringList resultFiles;
    QStringList resultDates;
//    QRegExp rx("fias.\\.rar"); //\\.rar   fias_dbf|fias_xml)
    QRegExp rx("(fias_dbf|fias_delta_dbf)\\.rar");
//    QRegExp rx("\\d{1,2}\\.\\d{1,2}\\.\\d{4,}");
    int pos = 0; //rx.indexIn(data);
    //result = rx.capturedTexts();
    QString str;
    while( (pos = rx.indexIn(data,pos)) !=-1){
        str = rx.cap(0);
        if(!resultFiles.contains(str))
            resultFiles<<str; //rx.cap(0);
        pos+= rx.matchedLength();
        //qDebug()<<pos<<rx.cap(0);
    }
//qDebug()<<resultFiles;
//   qDebug()<<data<<result;//<<cnt;
    rx.setPattern("\\d{1,2}\\.\\d{1,2}\\.\\d{4,}");
    pos = 0;
    int yyyy =  0;
    QDate dt = QDate::currentDate();
    while( (pos = rx.indexIn(data,pos)) !=-1){
        str = rx.cap(0);
        yyyy = str.right(4).toInt();
        if(yyyy< dt.year()){
            pos+= rx.matchedLength();
            continue;
        }
        if(!resultDates.contains(str))
            resultDates<<str; //rx.cap(0);
        pos+= rx.matchedLength();
    //qDebug()<<pos<<rx.cap(0);
    }
//qDebug()<<resultDates;

    ui->comboBox->addItems(resultFiles);
    ui->comboBox_2->addItems(resultDates);
}


void WgtFilesFias::sltDownLoderInit()
{
    QString url = ui->lineEdit->text().trimmed();
    if(url.isEmpty()){
        QMessageBox::information(this,tr("ВНИМАНИЕ"),tr("Нет, адреса откуда качать"));
        return;
    }
    rqst.setUrl(QUrl(url));
    networkFiasAccess->get(rqst);
}

void WgtFilesFias::sltRequestFiles()
{
    // http://fias.nalog.ru/Public/Downloads/20140825/fias_dbf.rar
    // http://fias.nalog.ru/Public/Downloads/20140825/fias_delta_dbf.rar
    QString strUrl;
    QString nameFile="";
    QString strDate="";
    nameFile = ui->comboBox->itemText(ui->comboBox->currentIndex());
    strDate = ui->comboBox_2->itemText(ui->comboBox_2->currentIndex());
    if(ui->comboBox->currentIndex()==-1 || nameFile.trimmed().isEmpty()){
        QMessageBox::information(this,tr("ВНИМАНИЕ"),tr("Не выбран файл"));
        return;
    }
    if(ui->comboBox_2->currentIndex()==-1 && strDate.trimmed().isEmpty()){
        QMessageBox::information(this,tr("ВНИМАНИЕ"),tr("Не выбрана дата"));
        return;
    }
    if(ui->lineEdit_2->text().trimmed().isEmpty()){
        QMessageBox::information(this,tr("ВНИМАНИЕ"),tr("Не выбрана папка для закачки файла"));
        return;
    }
    QStringList sList = strDate.split('.');
    QString strD,strM,strY;
    strY = sList[2];
    strM = sList[1];
    strD = sList[0];
    strDate = strY+strM+strD;

    strUrl = tr("http://fias.nalog.ru/Public/Downloads/");
    strUrl+= (strDate + tr("/"));
    strUrl+= nameFile;
//qDebug()<<s<<nameFile<<strDate;
//qDebug()<<strUrl;
    rqst.setUrl(QUrl(strUrl));
    replyFile = networkFilesAccess->get(rqst);
    replyFile->setObjectName("file");
    ui->progressBar->setVisible(true);

    connect(replyFile,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(sltProgressBarFile(qint64,qint64)));

    //networkFilesAccess->get(rqst);

}

void WgtFilesFias::sltDownLoaderFiles(QNetworkReply *rply)
{
//qDebug()<<rply->objectName();
    QString strTagDir = ui->lineEdit_2->text().trimmed();
    QString nameFile = ui->comboBox->itemText(ui->comboBox->currentIndex());
    QFile file(strTagDir+"/"+nameFile);


    if(file.open(QIODevice::WriteOnly)){
        file.write(rply->readAll());
    }

    file.close();
    disconnect(replyFile,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(sltProgressBarFile(qint64,qint64)));

    QMessageBox::information(this,"ЗАГРУЗКА","Загрузка закончена");

    ui->progressBar->setVisible(false);


}

void WgtFilesFias::sltProgressBarFile(qint64 rcv, qint64 total)
{
//qDebug()<<"sltProgressBarFile";

    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(rcv);
}

void WgtFilesFias::sltUpdateDb()
{
    QDbf::QDbfTable table;
    QString file;
    QStringList files;
    QString strPath;
    QString codeRegion;
    codeRegion = mdlCmbRegions->index(ui->comboBox_Regions->currentIndex(),0).data().toString();
    bool bDelta = ui->radioButton_Delta->isChecked();
    if(bDelta){
        strPath = QApplication::applicationDirPath()+tr("/Source_FIAS/fias_delta_dbf");
        files<<"ADDROBJ.DBF"
             <<("HOUSE"+codeRegion+".DBF")
             <<"DADDROBJ.DBF"
             <<"DHOUSE.DBF";
             //<<"SOCRBASE.DBF";
    }
    else{ //FULL
        strPath = QApplication::applicationDirPath()+tr("/Source_FIAS/fias_dbf");
        files<<"ADDROBJ.DBF"
             <<("HOUSE"+codeRegion+".DBF")
             <<"SOCRBASE.DBF";

    }
    QDir dirPath;
    if(!dirPath.exists(strPath)){
        dirPath.mkpath(strPath);
    }
    switchVisibleBar(true);
    QString fileName;
    QString ins,upd,del,slct,delLstHouse;
    QString strTmp;

    int maxLstHouse   = 22000;
    int maxLstAddrobj = 11500;
    int maxDelHouse   = 100;
    int maxUpdHouse   = 65;
    int iLst = 0;
    int iDelHouse = 0;
    int iLstUpd = 0;

    QString strDelLstHouse;

    QSqlQuery qry(m_db);

    QStringList::Iterator it = files.begin();
    int maxTotal = files.count();
    int curStep = 0;
    ui->progressBar_Files->setRange(curStep,maxTotal);

    QTime t;
    t.start();

    if(bDelta){
        QString tmpSlct;
        tmpSlct = "select distinct hs.houseid \
                from house hs inner join addrobj aobj on hs.aoguid = aobj.aoguid \
                where aobj.regioncode = '"+codeRegion+"'";
        qry.prepare(tmpSlct);
//qDebug()<<tmpSlct;
        ui->progressBar_Prep->setRange(0,0);
//        QApplication::processEvents();

        QApplication::setOverrideCursor(Qt::WaitCursor);
        qry.exec();
        QApplication::restoreOverrideCursor();
//        ui->label_Prep->setVisible(true);
//        ui->progressBar_Prep->setVisible(true);
        ui->progressBar_Prep->setRange(0,qry.size());
        int stp=0;
        lstHouseDelta.clear();
        lstHouseDelta.reserve(qry.size());
        while(qry.next()){
            lstHouseDelta<<qry.value(0).toString().trimmed();
            ++stp;
            ui->progressBar_Prep->setValue(stp);
            QApplication::processEvents();
        }
qDebug()<<"QSet"
       <<"count = "<<lstHouseDelta.count()
       <<t.elapsed()<<t.elapsed()/1000
       <<t.elapsed()/(1000*60)
       <<t.elapsed()/(1000*60*60);
        qry.clear();
        ui->label_Prep->setVisible(false);
        ui->progressBar_Prep->setVisible(false);
        t.restart();
    }

    while(it != files.end()) {
        fileName = (*it).trimmed();
        file = strPath+"/"+fileName;
        ++it;
//        ++curStep;
//        ui->progressBar_Files->setValue(curStep);
qDebug()<<file;

        if(!table.open(file)){
            qDebug()<<"!!!!! Error open file"<<file;
            continue ;//return;
        };

        QDbf::QDbfRecord record = table.record();
        int cntFields = record.count();
        int cntRow = table.size();
        int curRow = 0;
        iLst = 0;
        iDelHouse = 0;
        iLstUpd = 0;
        ui->label_curFile->setText(fileName);
        ui->progressBar_File->setRange(curRow,cntRow);
//создание  strQry
        if(!bDelta){ //FULL
            if(fileName=="ADDROBJ.DBF" ){
                ins = "insert into addrobj(aoid, formalname, regioncode, autocode, areacode, citycode, ctarcode, placecode, streetcode, extrcode, sextcode, ";
                ins+= "offname, postalcode, okato, oktmo, updatedate, shortname, aolevel, parentguid, aoguid, previd, nextid, code, plaincode, ";
                ins+= "actstatus, centstatus, operstatus, currstatus, startdate, enddate) ";
                ins+= "values(:aoid, :formalname, :regioncode, :autocode, :areacode, :citycode, :ctarcode, :placecode, :streetcode, :extrcode, :sextcode, ";
                ins+= ":offname, :postalcode, :okato, :oktmo, :updatedate, :shortname, :aolevel, :parentguid, :aoguid, :previd, :nextid, :code, :plaincode, ";
                ins+= ":actstatus, :centstatus, :operstatus, :currstatus, :startdate, :enddate)";
            }
            if(fileName.contains("HOUSE"+codeRegion+".DBF")){
                ins = "insert into house(postalcode, updatedate, housenum, eststatus, buildnum, strucnum, ";
                ins+= "strstatus, houseid, houseguid, aoguid, startdate, enddate, statstatus, counter) ";
                ins+= "values(:postalcode, :updatedate, :housenum, :eststatus, :buildnum, :strucnum, ";
                ins+= ":strstatus, :houseid, :houseguid, :aoguid, :startdate, :enddate, :statstatus, :counter)";
            }
            if(fileName=="SOCRBASE.DBF"){
                ins = "insert into socrbase(level, scname, socrname, kod_t_st) values(:level, :scname, :socrname, :kod_t_st)";
            }
        }
        else{        //DELTA
            if(fileName == "ADDROBJ.DBF"){
                slct =  "select aobj.aoid from  addrobj aobj where aobj.aoid = :aoid";

                upd = "update addrobj aobj set formalname = :formalname, regioncode = :regioncode, autocode = :autocode, ";
                upd+= "areacode=:areacode, citycode = :citycode, ctarcode = :ctarcode, placecode = :placecode, ";
                upd+= "streetcode = :streetcode, extrcode = :extrcode, sextcode = :sextcode, offname = :offname, ";
                upd+= "postalcode = :postalcode, okato = :okato, oktmo = :oktmo, updatedate = :updatedate, ";
                upd+= "shortname = :shortname, aolevel = :aolevel, parentguid = :parentguid, aoguid = :aoguid, ";
                upd+= "previd = :previd, nextid = :nextid, code = :code, plaincode = :plaincode, ";
                upd+= "actstatus = :actstatus, centstatus = :centstatus, operstatus = :operstatus, currstatus = :currstatus, ";
                upd+= "startdate = :startdate, enddate = :enddate ";
                upd+= "where aobj.aoid = :aoid";

                ins = "insert into addrobj(aoid, formalname, regioncode, autocode, areacode, citycode, ctarcode, placecode, streetcode, extrcode, sextcode, ";
                ins+= "offname, postalcode, okato, oktmo, updatedate, shortname, aolevel, parentguid, aoguid, previd, nextid, code, plaincode, ";
                ins+= "actstatus, centstatus, operstatus, currstatus, startdate, enddate) ";
                ins+= "values(:aoid, :formalname, :regioncode, :autocode, :areacode, :citycode, :ctarcode, :placecode, :streetcode, :extrcode, :sextcode, ";
                ins+= ":offname, :postalcode, :okato, :oktmo, :updatedate, :shortname, :aolevel, :parentguid, :aoguid, :previd, :nextid, :code, :plaincode, ";
                ins+= ":actstatus, :centstatus, :operstatus, :currstatus, :startdate, :enddate)";

            }
            if(fileName.contains("DADDROBJ.DBF")){
                del = "delete from addrobj aobj where aobj.aoid = :aoid";
            }
            if(fileName.contains("HOUSE"+codeRegion+".DBF")){
                slct = "select houseid from house where houseid = :houseid";

                ins = "insert into house(postalcode, updatedate, housenum, eststatus, buildnum, strucnum, ";
                ins+= "strstatus, houseid, houseguid, aoguid, startdate, enddate, statstatus, counter) ";
                ins+= "values(:postalcode, :updatedate, :housenum, :eststatus, :buildnum, :strucnum, ";
                ins+= ":strstatus, :houseid, :houseguid, :aoguid, :startdate, :enddate, :statstatus, :counter)";
//qDebug()<<ins;
                upd = "update house hs set postalcode = :postalcode, updatedate = :updatedate, housenum = :housenum, ";
                upd+= "eststatus = :eststatus, buildnum = :buildnum, strucnum = :strucnum, strstatus = :strstatus, ";
                upd+= "houseguid = :houseguid, aoguid = :aoguid, startdate = :startdate, enddate = :enddate, ";
                upd+= "statstatus = :statstatus, counter = :counter ";
                upd+= "where hs.houseid = :houseid";

            }
            if(fileName.contains("DHOUSE.DBF")){
                del = "delete from house hs where hs.houseid = :houseid";
                delLstHouse = "delete from house hs where hs.houseid in (";
            }
        }

        while(table.next()){
//            qDebug()<<"Запись "<<fileName<<"--------------------------";
            record = table.record();
            if(fileName=="ADDROBJ.DBF" && record.value(24).toString().trimmed()!=codeRegion){
                ++curRow;
                ui->progressBar_File->setValue(curRow);
                QApplication::processEvents();
                continue;
            }

            for(int i = 0; i<cntFields; ++i){

                strTmp = record.value(i).toString().trimmed();

//                if(fileName.contains("SOCRBASE.DBF")){
//                    qDebug()<<"i="<<i<<record.fieldName(i)<<strTmp;
//                }
                strTmp = (strTmp.isEmpty() || strTmp.isNull()) ? " ":strTmp;
//--Построчное наполнение qry
                if(fileName=="ADDROBJ.DBF" && bDelta){
                    switch (i) {
                    case 0:
                        actstatus = strTmp.toInt();
                        break;
                    case 1:
                        aoguid = strTmp;
                        break;
                    case 2:
                        aoid = strTmp;
                        break;
                    case 3:
                        aolevel = strTmp.toInt();
                        break;
                    case 4:
                        strTmp = (strTmp.trimmed().isEmpty() || strTmp.trimmed().isNull()) ? "000":strTmp;
                        areacode = strTmp;
                        break;
                    case 5:
                        strTmp = (strTmp.trimmed().isEmpty() || strTmp.trimmed().isNull()) ? "0":strTmp;
                        autocode = strTmp;
                        break;
                    case 6:
                        centstatus = strTmp.toInt();
                        break;
                    case 7:
                        citycode = strTmp;
                        break;
                    case 8:
                        code = strTmp;
                        break;
                    case 9:
                        currstatus = strTmp.toInt();
                        break;
                    case 10:
                        enddate = QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 11:
                        formalname = strTmp;
                        break;
                    case 14:
                        nextid = strTmp;
                        break;
                    case 15:
                        offname = strTmp;
                        break;
                    case 16:
                        okato = strTmp;
                        break;
                    case 17:
                        oktmo = strTmp;
                        break;
                    case 18:
                        operstatus = strTmp.toInt();
                        break;
                    case 19:
                        parentguid = strTmp;
                        break;
                    case 20:
                        placecode = strTmp;
                        break;
                    case 21:
                        plaincode = strTmp;
                        break;
                    case 22:
                        postalcode = strTmp;
                        break;
                    case 23:
                        previd = strTmp;
                        break;
                    case 24:
                        regioncode = strTmp;
                        break;
                    case 25:
                        shortname = strTmp;
                        break;
                    case 26:
                        startdate = QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 27:
                        streetcode = strTmp;
                        break;
                    case 30:
                        updatedate = QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 31:
                        ctarcode = strTmp;
                        break;
                    case 32:
                        extrcode = strTmp;
                        break;
                    case 33:
                        sextcode = strTmp;
                        break;
                    }
                }

                if(fileName == "DADDROBJ.DBF" && bDelta){
                    switch(i){
                    case 1:
                        aoid = strTmp;
                        break;
                    case 24:
                        regioncode = strTmp;
                        break;
                    }
                }
                if(fileName.contains("HOUSE"+codeRegion+".DBF") && bDelta){
                    switch (i) {
                    case 0:
                        aoguid=strTmp;
                        break;
                    case 1:
                        buildnum=strTmp;
                        break;
                    case 2:
                        enddate=QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 3:
                        eststatus=strTmp.toInt();
                        break;
                    case 4:
                        houseguid=strTmp;
                        break;
                    case 5:
                        houseid=strTmp;
                        break;
                    case 6:
                        housenum=strTmp;
                        break;
                    case 7:
                        statstatus=strTmp.toInt();
                        break;
                    case 12:
                        postalcode=strTmp;
                        break;
                    case 13:
                        startdate=QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 14:
                        strucnum=strTmp;
                        break;
                    case 15:
                        strstatus=strTmp.toInt();
                        break;
                    case 18:
                        updatedate=QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 20:
                        counter=strTmp.toInt();
                        break;
                    }
                }
                if(fileName == "DHOUSE.DBF" && bDelta){
                    switch(i){
                    case 5:
                        houseid=strTmp;
                        if(iDelHouse==1){
                            strDelLstHouse = "'"+houseid+"'";
                        }
                        else{
                            strDelLstHouse+= ",'"+houseid+"'";
                        }
                        break;
                    }
                }
                if(fileName=="SOCRBASE.DBF" && !bDelta){
                    switch (i) {
                    case 0:
                        level  = strTmp.toInt();
                        break;
                    case 1:
                        socrname = strTmp;
                        break;
                    case 2:
                        scname = strTmp;
                        break;
                    case 3:
                        kod_t_st = strTmp.toInt();
                        break;
                    }
                }
//--END Построчное наполнение qry

//Batch наполенение
                if(fileName=="ADDROBJ.DBF" && !bDelta){
                    switch (i) {
                    case 0:
                        actstatusLst << strTmp.toInt();
                        break;
                    case 1:
                        aoguidLst << strTmp;
                        break;
                    case 2:
                        aoidLst << strTmp;
                        break;
                    case 3:
                        aolevelLst << strTmp.toInt();
                        break;
                    case 4:
                        strTmp = (strTmp.trimmed().isEmpty() || strTmp.trimmed().isNull()) ? "000":strTmp;
                        areacodeLst << strTmp;
                        break;
                    case 5:
                        strTmp = (strTmp.trimmed().isEmpty() || strTmp.trimmed().isNull()) ? "0":strTmp;
                        autocodeLst << strTmp;
                        break;
                    case 6:
                        centstatusLst << strTmp.toInt();
                        break;
                    case 7:
                        citycodeLst << strTmp;
                        break;
                    case 8:
                        codeLst << strTmp;
                        break;
                    case 9:
                        currstatusLst << strTmp.toInt();
                        break;
                    case 10:
                        enddateLst << QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 11:
                        formalnameLst << strTmp;
                        break;
                    case 14:
                        nextidLst << strTmp;
                        break;
                    case 15:
                        offnameLst << strTmp;
                        break;
                    case 16:
                        okatoLst << strTmp;
                        break;
                    case 17:
                        oktmoLst << strTmp;
                        break;
                    case 18:
                        operstatusLst << strTmp.toInt();
                        break;
                    case 19:
                        parentguidLst << strTmp;
                        break;
                    case 20:
                        placecodeLst << strTmp;
                        break;
                    case 21:
                        plaincodeLst << strTmp;
                        break;
                    case 22:
                        postalcodeLst << strTmp;
                        break;
                    case 23:
                        previdLst << strTmp;
                        break;
                    case 24:
                        regioncodeLst << strTmp;
                        break;
                    case 25:
                        shortnameLst << strTmp;
                        break;
                    case 26:
                        startdateLst << QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 27:
                        streetcodeLst << strTmp;
                        break;
                    case 30:
                        updatedateLst << QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 31:
                        ctarcodeLst << strTmp;
                        break;
                    case 32:
                        extrcodeLst << strTmp;
                        break;
                    case 33:
                        sextcodeLst << strTmp;
                        break;
                    }
                }

                if(fileName.contains("HOUSE"+codeRegion+".DBF") && !bDelta){
                    switch (i) {
                    case 0:
                        aoguidLst<<strTmp;
                        break;
                    case 1:
                        buildnumLst<<strTmp;
                        break;
                    case 2:
                        enddateLst<<QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 3:
                        eststatusLst<<strTmp.toInt();
                        break;
                    case 4:
                        houseguidLst<<strTmp;
                        break;
                    case 5:
                        houseidLst<<strTmp;
                        break;
                    case 6:
                        housenumLst<<strTmp;
                        break;
                    case 7:
                        statstatusLst<<strTmp.toInt();
                        break;
                    case 12:
                        postalcodeLst<<strTmp;
                        break;
                    case 13:
                        startdateLst<<QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 14:
                        strucnumLst<<strTmp;
                        break;
                    case 15:
                        strstatusLst<<strTmp.toInt();
                        break;
                    case 18:
                        updatedateLst<<QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 20:
                        counterLst<<strTmp.toInt();
                        break;
                    }
                }
//END Batch наполенение
            }

//--Построчная запись/удаление в базу
            if(fileName=="ADDROBJ.DBF" && bDelta ){
                qry.prepare(slct);
                qry.bindValue(":aoid",aoid);
                qry.exec();

                QString cur = "";
                while(qry.next()){
                    cur = qry.value(0).toString();
                }
                qry.clear();

                if(cur.trimmed().isEmpty()){
                    execOneAddrobj(qry,ins);
                }
                else{
                    execOneAddrobj(qry,upd);
                }
            }

            if(fileName.contains("HOUSE"+codeRegion+".DBF") && bDelta){
                QString cur = "";
                if(lstHouseDelta.contains(houseid)){
                    cur = houseid;
                }


                if(cur.trimmed().isEmpty()){
/*                    switch (i) {
                    case 0:
                        aoguidLst<<strTmp;
                        break;
                    case 1:
                        buildnumLst<<strTmp;
                        break;
                    case 2:
                        enddateLst<<QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 3:
                        eststatusLst<<strTmp.toInt();
                        break;
                    case 4:
                        houseguidLst<<strTmp;
                        break;
                    case 5:
                        houseidLst<<strTmp;
                        break;
                    case 6:
                        housenumLst<<strTmp;
                        break;
                    case 7:
                        statstatusLst<<strTmp.toInt();
                        break;
                    case 12:
                        postalcodeLst<<strTmp;
                        break;
                    case 13:
                        startdateLst<<QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 14:
                        strucnumLst<<strTmp;
                        break;
                    case 15:
                        strstatusLst<<strTmp.toInt();
                        break;
                    case 18:
                        updatedateLst<<QDate::fromString(strTmp,"yyyy-MM-dd");
                        break;
                    case 20:
                        counterLst<<strTmp.toInt();
                        break;
                    }
*/
                    aoguidLst<<aoguid;
                    buildnumLst<<buildnum;
                    enddateLst<<enddate;
                    eststatusLst<<eststatus;
                    houseguidLst<<houseguid;
                    houseidLst<<houseid;
                    housenumLst<<housenum;
                    statstatusLst<<statstatus;
                    postalcodeLst<<postalcode;
                    startdateLst<<startdate;
                    strucnumLst<<strucnum;
                    strstatusLst<<strstatus;
                    updatedateLst<<updatedate;
                    counterLst<<counter;

                    //execOneHouse(qry, ins);
                    if(iLst>=maxLstHouse && bDelta){
                        qDebug()<<"insert delta house"<<iLst<<houseidLst.count(),houseid;
                        //execBatchHouse(qry, ins, iLst);
                        //УБРАТЬ обнуление сделал для отладки
                        iLst = 0;
                        aoguidLst.clear();
                        buildnumLst.clear();
                        enddateLst.clear();
                        eststatusLst.clear();
                        houseguidLst.clear();
                        houseidLst.clear();
                        housenumLst.clear();
                        statstatusLst.clear();
                        postalcodeLst.clear();
                        startdateLst.clear();
                        strucnumLst.clear();
                        strstatusLst.clear();
                        updatedateLst.clear();
                        counterLst.clear();
                    }
                }
                else{
                   // qDebug()<<"update delta house 1"<<cur<< iLstUpd;
                    aoguidLstUpd<<aoguid;
                    buildnumLstUpd<<buildnum;
                    enddateLstUpd<<enddate;
                    eststatusLstUpd<<eststatus;
                    houseguidLstUpd<<houseguid;
                    houseidLstUpd<<houseid;
                    housenumLstUpd<<housenum;
                    statstatusLstUpd<<statstatus;
                    postalcodeLstUpd<<postalcode;
                    startdateLstUpd<<startdate;
                    strucnumLstUpd<<strucnum;
                    strstatusLstUpd<<strstatus;
                    updatedateLstUpd<<updatedate;
                    counterLstUpd<<counter;
                    if(iLstUpd >= maxUpdHouse && bDelta){
                        //qDebug()<<"update delta house 2"<<cur<< iLstUpd;
                        //t.restart();
                        execBatchUpdHouse(qry, upd, iLstUpd);
                        //qDebug()<<"execBatchUpdHouse"<<t.elapsed()<<t.elapsed()/1000;
                    }
                    //execOneHouse(qry, upd);
                }
            }

            if(fileName=="DADDROBJ.DBF" && bDelta){
//qDebug()<<"delete "<<regioncode<<codeRegion;
                if(regioncode == codeRegion){
                    execDelOneAddrobj(qry,del);
//                    qDebug()<<"       delete"<<aoid;
                }
            }

            if(fileName=="DHOUSE.DBF" && bDelta){
                //execDelOneHouse(qry,del);

//                qDebug()<<iDelHouse<<maxDelHouse<<strDelLstHouse<<delLstHouse;

                if(iDelHouse >= maxDelHouse){
                    //qDebug()<<delLstHouse;
                    execDelLstHouse(qry,delLstHouse,strDelLstHouse);
                    iDelHouse = 0;
                    strDelLstHouse = "";
                }
            }
            if(fileName == "SOCRBASE.DBF" && !bDelta){
                  execOneSocrBase(qry,ins);
            }
//--END Построчная запись/удаление в базу


//--Batch запись в базу

            if(fileName=="ADDROBJ.DBF" && iLst>=maxLstAddrobj && !bDelta){
                execBatchAddrobj(qry, ins, iLst);
            }
            if(fileName.contains("HOUSE"+codeRegion+".DBF") && iLst>=maxLstHouse && !bDelta){
                execBatchHouse(qry, ins, iLst);
            }

//--END Batch запись в базу
            ++curRow;

            ++iLst;
            ++iDelHouse;
            ++iLstUpd;
            //qDebug()<<"iLst = "<<iLst<<"curRow = "<<curRow;
            ui->progressBar_File->setValue(curRow);
            QApplication::processEvents();
        }                                                       //--- END while(table.next())

        if(fileName=="ADDROBJ.DBF" && iLst!=0 && iLst<maxLstAddrobj && !bDelta){
            execBatchAddrobj(qry, ins, iLst);
        }

        if(fileName.contains("HOUSE"+codeRegion+".DBF") && iLst!=0 && iLst<maxLstHouse && bDelta){
            qDebug()<<"insert house after cycle"<<iLst<<houseidLst.count(),houseid;;
            //ВКЛЮЧИТЬ отключил для отладки
            execBatchHouse(qry, ins, iLst);
        }

        if(fileName.contains("HOUSE"+codeRegion+".DBF") && iLstUpd !=0 && iLstUpd < maxUpdHouse && bDelta) {
            qDebug()<<"update house after cycle"<<iLstUpd <<houseidLstUpd.count();
            execBatchUpdHouse(qry, upd, iLstUpd);
        }

        if(fileName=="DHOUSE.DBF" && iDelHouse!=0 && iDelHouse<maxLstHouse && bDelta){
            execDelLstHouse(qry, delLstHouse, strDelLstHouse);
        }
        table.close();
        ++curStep;
        ui->progressBar_Files->setValue(curStep);

        qDebug()<<"--------------------------- TIME--------------"
                <<t.elapsed()
                <<t.elapsed()/1000
                <<t.elapsed()/(1000*60)
                <<t.elapsed()/(1000*60*60);
        t.restart();

    }
    lstHouseDelta.clear();

    switchVisibleBar(false);
}

void WgtFilesFias::sltRadioDelta()
{
    refreshCmbRegions();
}

void WgtFilesFias::sltRadioFull()
{
    refreshCmbRegions();
}

void WgtFilesFias::sltChangeText(const QString &str)
{
    int idx = ui->comboBox->findText(str,Qt::MatchContains);
    qDebug()<<idx;
}

void WgtFilesFias::sltChangTextL(const QString &str)
{
    int idx = ui->comboBox_Regions->findText(str,Qt::MatchContains);
    qDebug()<<idx;
}
