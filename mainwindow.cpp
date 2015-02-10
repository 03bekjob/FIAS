#include "mainwindow.h"
#include <QApplication>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMdiArea>
#include <QToolBar>
#include <QMdiSubWindow>

#include <QMessageBox>
#include "wgtconndb.h"
#include <QSqlError>
#include "wgtregions.h"
#include "wgtfilesfias.h"
#include "wgtfilesdbf.h"
#include "wgtaddrobj.h"
#include "wgttestdelegate.h"

//#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("Адреса ФИАС");


    pMdiA = new QMdiArea(this);

    readSettings();

    createAction();
    createMenu();
    createToolBar();
    createConnect();


    setCentralWidget(pMdiA);
    pMdiA->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    pMdiA->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::createAction()
{
    p_aExit = new QAction(QIcon(":/img/exit.png"),tr("Выход"),this);
    p_aConnDb = new QAction(QIcon(":/img/database.png"), tr("Соединение с базой"),this);
    p_aTB = new QAction(tr("Выкл\\Вкл ToolBar"),this);
    p_aTB->setCheckable(true);
    p_aRegions = new QAction(tr("Регионы"),this);
    p_aAddrObj = new QAction(tr("Адреса"),this);
    p_aFilesFias = new QAction(tr("Получить файлы ФИАС"),this);
    p_aFilesDeltaDbf = new QAction(tr("Файлы DBF - дельта"),this);
    p_aFilesDbf = new QAction(tr("Файлы DBF - полные"),this);
    p_aTestDelegate = new QAction(tr("Тест делегатов"),this);



    p_aAllClsW = new QAction(QIcon(":/img/closeall.png"),tr("Закрыть все окна"),this); // Закрыть все окна
    p_aClsW    = new QAction(QIcon(":/img/closeone.png"),tr("Закрыть окно"),this);     // Закрыть активное окно
    p_aCsdWin  = new QAction(QIcon(":/img/cascade.png"),tr("Каскад"),this);            // Сделать каскад
    p_aTilWin  = new QAction(QIcon(":/img/tile.png"),tr("Мозаика"),this);              // Сделать мозаику
    p_aPopWin  = new QAction(QIcon(":/img/turn.png"),tr("Одно окно"),this);            // Сделать активным окно
    p_aWinGrp  = new QActionGroup(this);

}

void MainWindow::createMenu()
{
    p_mConnDb = menuBar()->addMenu(tr("Соединение"));
    p_mConnDb->addAction(p_aConnDb);
    p_mConnDb->addSeparator();
    p_mConnDb->addAction(p_aExit);

    p_mTab = menuBar()->addMenu((tr("Таблицы")));
    p_mTab->addAction(p_aRegions);
    p_mTab->addAction(p_aAddrObj);
    p_mTab->addSeparator();
    p_mTab->addAction(p_aFilesFias);
    p_mTab->addAction(p_aFilesDeltaDbf);
    p_mTab->addAction(p_aFilesDbf);
    p_mTab->addSeparator();
    p_mTab->addAction(p_aTestDelegate);

    p_mWin = menuBar()->addMenu(tr("Окна"));
    p_mWin->addAction(p_aCsdWin);
    p_mWin->addAction(p_aTilWin);
    p_mWin->addAction(p_aPopWin);
    p_mWin->addSeparator();
    p_mWin->addAction(p_aClsW);
    p_mWin->addAction(p_aAllClsW);
    p_mWin->addSeparator();


    p_mService = menuBar()->addMenu(tr("Сервис"));
    p_mService->addAction(p_aTB);
}

void MainWindow::createToolBar()
{
    pTB = new QToolBar(this);
    pTB->setObjectName("toolBar");
    this->addToolBar(Qt::RightToolBarArea,pTB);
    pTB->addAction(p_aConnDb);
    pTB->addAction(p_aExit);

    pTB->addSeparator();
    pTB->addAction(p_aCsdWin);
    pTB->addAction(p_aTilWin);
    pTB->addAction(p_aPopWin);

    pTB->addSeparator();
    pTB->addAction(p_aClsW);
    pTB->addAction(p_aAllClsW);

    addToolBar(Qt::TopToolBarArea,pTB);
    if(isHiddenToolBar){
        pTB->hide();
    }
    else{
        pTB->show();
    }
   p_aTB->setChecked(isHiddenToolBar);

}

void MainWindow::createConnect()
{
    connect(p_aExit,SIGNAL(triggered()),SLOT(sltExit()));
    connect(p_aTB,SIGNAL(triggered()),SLOT(sltTB()));
    connect(pMdiA, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(sltUpdActions()) );
    //Соединение
    connect(p_aConnDb,SIGNAL(triggered()),this,SLOT(sltConnDb()));

    //Таблицы
    connect(p_aRegions,SIGNAL(triggered()),this,SLOT(sltRegions()));
    connect(p_aAddrObj,SIGNAL(triggered()),this,SLOT(sltAddrObj()));
    connect(p_aFilesFias,SIGNAL(triggered()),this,SLOT(sltFilesFias()));
    connect(p_aFilesDeltaDbf,SIGNAL(triggered()),this,SLOT(sltFilesDeltaDbf()));
    connect(p_aFilesDbf,SIGNAL(triggered()),this,SLOT(sltFilesDbf()));

    connect(p_aTestDelegate,SIGNAL(triggered()),this,SLOT(sltTestDelegate()));

    //Окна
    connect(p_aAllClsW,SIGNAL(triggered()),pMdiA,SLOT(closeAllSubWindows()));
    connect(p_aClsW,SIGNAL(triggered()),pMdiA,SLOT(closeActiveSubWindow()));
    connect(p_aCsdWin,SIGNAL(triggered()),pMdiA,SLOT(cascadeSubWindows()));
    connect(p_aTilWin,SIGNAL(triggered()),pMdiA,SLOT(tileSubWindows()));
    connect(p_aPopWin,SIGNAL(triggered()),this,SLOT(sltPopWin()));
}


void MainWindow::readSettings()
{
    QSettings st;
    QString key,keyH,keyTBGeometry;
    key = tr("MainWindow");
    keyH= key + tr("/ToolBar/isHidded");
    if(st.value(keyH).isNull()){
        isHiddenToolBar = false;
    }
    else {
        QVariant v;
        isHiddenToolBar = st.value(keyH,v).toBool();
    }

//      keyTBGeometry = key +tr("/ToolBar/geometry");
//      pTB->setGeometry(st.value(keyTBGeometry).toRect());

}

void MainWindow::saveSettings()
{
    QSettings st;
    QString key,keyH,keyTBGeometry;

    key = tr("MainWindow");
    keyH= key+tr("/ToolBar/isHidded");
    st.setValue(keyH,isHiddenToolBar);

//    keyTBGeometry = key +tr("/ToolBar/geometry");
//    st.setValue(keyTBGeometry,pTB->geometry());

//    qDebug()<<"pTB"<<pTB->layoutDirection()<<pTB->orientation()<<Qt::LeftDockWidgetArea;
}

void MainWindow::addWgt(QWidget *w)
{

    QMdiSubWindow* subW=NULL;
    WgtRegions*    wRgns;
    WgtFilesFias*  wFilesFias;
    WgtFilesDBF*   wFilesDBF;
    WgtAddrObj*    wAddrObj;
    WgtTestDelegate* wTestDelegate;

//    wgtBlng*       wBlng;
//qDebug()<<"MainWindow::addWgt"<<w->objectName();
    if(w->objectName()=="WgtRegions"){
        wRgns = qobject_cast<WgtRegions*> (w);
        subW = pMdiA->addSubWindow(wRgns);
        p_mWin->addAction(wRgns->getMenuAction());
        p_aWinGrp->addAction(wRgns->getMenuAction());
        connect(wRgns,SIGNAL(sgnExit()),pMdiA,SLOT(closeActiveSubWindow()));

    }
    else if(w->objectName()=="WgtFilesFias"){
        wFilesFias = qobject_cast<WgtFilesFias*> (w);
        subW = pMdiA->addSubWindow(wFilesFias);
        p_mWin->addAction(wFilesFias->getMenuAction());
        p_aWinGrp->addAction(wFilesFias->getMenuAction());
        connect(wFilesFias,SIGNAL(sgnExit()),pMdiA,SLOT(closeActiveSubWindow()));
    }
    else if(w->objectName()=="WgtFilesDBF"){
        wFilesDBF = qobject_cast<WgtFilesDBF*> (w);
        subW = pMdiA->addSubWindow(wFilesDBF);
        p_mWin->addAction(wFilesDBF->getMenuAction());
        p_aWinGrp->addAction(wFilesDBF->getMenuAction());
        connect(wFilesDBF,SIGNAL(sgnExit()),pMdiA,SLOT(closeActiveSubWindow()));
    }
    else if(w->objectName()=="WgtAddrObj"){
        wAddrObj = qobject_cast<WgtAddrObj*> (w);
        subW = pMdiA->addSubWindow(wAddrObj);
        p_mWin->addAction(wAddrObj->getMenuAction());
        p_aWinGrp->addAction(wAddrObj->getMenuAction());
        connect(wAddrObj,SIGNAL(sgnExit()),pMdiA,SLOT(closeActiveSubWindow()));
    }
    else if(w->objectName()=="WgtTestDelegate"){
        wTestDelegate = qobject_cast<WgtTestDelegate*> (w);
        subW = pMdiA->addSubWindow(wTestDelegate);
        p_mWin->addAction(wTestDelegate->getMenuAction());
        p_aWinGrp->addAction(wTestDelegate->getMenuAction());
        connect(wTestDelegate,SIGNAL(sgnExit()),pMdiA,SLOT(closeActiveSubWindow()));
    }


    subW->showMaximized();
}

QWidget *MainWindow::activeWgt()
{
    QMdiSubWindow* subW = pMdiA->activeSubWindow();
//qDebug()<<"*MainWindow::activeWgt()"<<subW->widget()->objectName();
    if(subW){
        if(subW->widget()->objectName()=="WgtRegions"){
            return qobject_cast<WgtRegions*>(subW->widget());
        }
        if(subW->widget()->objectName()=="WgtFilesFias"){
            return qobject_cast<WgtFilesFias* >(subW->widget());
        }
        if(subW->widget()->objectName()=="WgtFilesDBF"){
            return qobject_cast<WgtFilesDBF* >(subW->widget());
        }
        if(subW->widget()->objectName()=="WgtAddrObj"){
            return qobject_cast<WgtAddrObj*>(subW->widget());
        }
        if(subW->widget()->objectName()=="WgtTestDelegate"){
            return qobject_cast<WgtTestDelegate*>(subW->widget());
        }
    }
    return NULL;

}

void MainWindow::setDb(const QString &connName)
{
    m_db = QSqlDatabase::database(connName);
//qDebug()<<"MainWindow::setDb"<<m_db.isOpen()<<m_db.isValid()<<m_db.isOpenError()<<dbName<<m_db.databaseName();
}

void MainWindow::sltExit()
{
    if(pMdiA){
        pMdiA->closeAllSubWindows();
    }
    if(m_db.isOpen()){
        m_db.close();
//        db.removeDatabase(db.connectionName());
    }

    qApp->exit();

}

void MainWindow::sltConnDb()
{
    if(pMdiA){
        if(pMdiA->subWindowList().isEmpty()){
            QWidget* wgt = new WgtConnDb(this);
            wgt->showNormal();
        }
        else{
            QMessageBox::information(NULL,"Внимание","Для создания повтороноо соеденения\nпожалуйста зактройте все окна задачи");
        }
    }
}

void MainWindow::sltUpdActions()
{
    QWidget* actWgt = activeWgt();
//    bool b =  (activeWgt() != NULL);
    bool b =  (actWgt != NULL);
    p_aAllClsW->setEnabled(b);
    p_aClsW->setEnabled(b);
    p_aCsdWin->setEnabled(b);
    p_aPopWin->setEnabled(b);
    p_aTilWin->setEnabled(b);

/*    if(activeWgt()){
        if(activeWgt()->objectName() == "WgtRegions"){
            WgtRegions* wgt = qobject_cast<WgtRegions*>(activeWgt());
            wgt->getMenuAction()->setChecked(true);
        }
        if(activeWgt()->objectName() == "WgtFilesFias"){
            WgtFilesFias* wgt = qobject_cast<WgtFilesFias*>(activeWgt());
            wgt->getMenuAction()->setChecked(true);
        }
        if(activeWgt()->objectName() == "WgtFilesDBF"){
            WgtFilesDBF* wgt = qobject_cast<WgtFilesDBF*>(activeWgt());
            wgt->getMenuAction()->setChecked(true);
        }
    }
*/
    if(actWgt){
        if(actWgt->objectName() == "WgtRegions"){
            WgtRegions* wgt = qobject_cast<WgtRegions*>(actWgt);
            wgt->getMenuAction()->setChecked(true);
        }
        if(actWgt->objectName() == "WgtFilesFias"){
            WgtFilesFias* wgt = qobject_cast<WgtFilesFias*>(actWgt);
            wgt->getMenuAction()->setChecked(true);
        }
        if(actWgt->objectName() == "WgtFilesDBF"){
            WgtFilesDBF* wgt = qobject_cast<WgtFilesDBF*>(actWgt);
            wgt->getMenuAction()->setChecked(true);
        }
        if(actWgt->objectName() == "WgtAddrObj"){
            WgtAddrObj* wgt = qobject_cast<WgtAddrObj*>(actWgt);
            wgt->getMenuAction()->setChecked(true);
        }
        if(actWgt->objectName() == "WgtTestDelegate"){
            WgtTestDelegate* wgt = qobject_cast<WgtTestDelegate*>(actWgt);
            wgt->getMenuAction()->setChecked(true);
        }

    }

}

void MainWindow::sltTB()
{
    isHiddenToolBar  = isHiddenToolBar ? false:true;
    if(isHiddenToolBar){
        pTB->hide();
    }
    else{
        pTB->show();
    }
    p_aTB->setChecked(isHiddenToolBar);
}

void MainWindow::sltPopWin()
{
    if(pMdiA){
        if(!pMdiA->subWindowList().isEmpty()){
            pMdiA->activeSubWindow()->showMaximized();
        }
    }

}

void MainWindow::sltRegions()
{
//qDebug()<<m_db.isDriverAvailable("QPSQL7")<<m_db.isOpen()<<m_db.isValid()<<m_db.isOpenError()<<m_db.databaseName()<<m_db.connectionName()<<m_db.driverName();
    if(!m_db.isValid()){
        QMessageBox::critical(this,tr("MainWindow ОШИБКА"),tr("Нет соеденения!!!\n")+m_db.lastError().text());
        return;
    }
    if(!m_db.isOpen()){
        QMessageBox::critical(this,tr("MainWindow ОШИБКА"),tr("Нет соеденения!!!\n")+m_db.lastError().text());
        return;
    }
    WgtRegions* w = new WgtRegions(m_db,this);
//qDebug()<<"MainWindow::sltRegions()";
    addWgt(w);
}

void MainWindow::sltAddrObj()
{
    if(!m_db.isValid()){
        QMessageBox::critical(this,tr("MainWindow ОШИБКА"),tr("Нет соеденения!!!\n")+m_db.lastError().text());
        return;
    }
    if(!m_db.isOpen()){
        QMessageBox::critical(this,tr("MainWindow ОШИБКА"),tr("Нет соеденения!!!\n")+m_db.lastError().text());
        return;
    }
    WgtAddrObj* w = new WgtAddrObj(m_db,this);
//qDebug()<<"MainWindow::sltRegions()";
    addWgt(w);

}

void MainWindow::sltFilesFias()
{
    if(!m_db.isValid()){
        QMessageBox::critical(this,tr("MainWindow ОШИБКА"),tr("Нет соеденения!!!\n")+m_db.lastError().text());
        return;
    }
    if(!m_db.isOpen()){
        QMessageBox::critical(this,tr("MainWindow ОШИБКА"),tr("Нет соеденения!!!\n")+m_db.lastError().text());
        return;
    }
    WgtFilesFias* w = new WgtFilesFias(m_db,this);
//qDebug()<<"MainWindow::sltFilesFias()";
    addWgt(w);

}

void MainWindow::sltFilesDeltaDbf()
{
    QString sTtl = tr("Файлы дельта DBF");
    WgtFilesDBF* w = new WgtFilesDBF(sTtl,this);
    addWgt(w);
}

void MainWindow::sltFilesDbf()
{
    QString sTtl = tr("Файлы DBF");
    WgtFilesDBF* w = new WgtFilesDBF(sTtl,this);
    addWgt(w);

}

void MainWindow::sltTestDelegate()
{
    if(!m_db.isValid()){
        QMessageBox::critical(this,tr("MainWindow ОШИБКА"),tr("Нет соеденения!!!\n")+m_db.lastError().text());
        return;
    }
    if(!m_db.isOpen()){
        QMessageBox::critical(this,tr("MainWindow ОШИБКА"),tr("Нет соеденения!!!\n")+m_db.lastError().text());
        return;
    }
    WgtTestDelegate* w = new  WgtTestDelegate(m_db,this);
//qDebug()<<"MainWindow::sltFilesFias()";
    addWgt(w);

}
