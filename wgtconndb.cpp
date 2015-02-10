#include "wgtconndb.h"
#include "mainwindow.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QSettings>
#include <QSqlError>

#include <QDebug>
#include <QMessageBox>

namespace {
static const int Duration = 500;//300;//750;//1500;

QVariant fontInterpolator(const QFont &start, const QFont &end,  qreal progress)
{
    qreal startSize = start.pointSizeF();
    qreal endSize = end.pointSizeF();
    qreal newSize = startSize + ((endSize - startSize) *
                                qBound(0.0, progress, 1.0));
    QFont font(start);
    font.setPointSizeF(newSize);
    return font;
}

}

WgtConnDb::WgtConnDb(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::Window|Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Соединение");
    setObjectName("conn");
    strConnName = tr("pgConn");


    pParent = qobject_cast<MainWindow*>(parent);
    createWidgets();
    createLayout();

    createConnect();

    createStateMashines();
    readSettings();

}

WgtConnDb::~WgtConnDb()
{
//qDebug()<<"WgtConnDb::~WgtConnDb()";
    saveSettings();
}

void WgtConnDb::createWidgets()
{

    lnUsr = new QLineEdit(this);
    lnPwd = new QLineEdit(this);
    lnPwd->setEchoMode(QLineEdit::Password);

    lnUsr->setText(tr("03bek"));
    lnPwd->setText(tr("Fyufhf03"));

    chHst = new QCheckBox(tr("Сменить сервер"),this);   //сменить сервер
    chHst->setCheckable(true);
    chHst->setCheckState(Qt::Unchecked);


    btEntr = new QPushButton(tr("Войти"),this);
    btExt  = new QPushButton(tr("Выйти"),this);

    lbDrv = new QLabel(tr("Драйвера"),this);
    cbDrv = new QComboBox(this);
    lbDrv->setBuddy(cbDrv);
    QStringList drv = QSqlDatabase::drivers();
//    drv.removeAll("QMYSQL3");
//    drv.removeAll("QOCI8");
//    drv.removeAll("QODBC");
//    drv.removeAll("QODBC3");
//    drv.removeAll("QPSQL7");
//    drv.removeAll("QPSQL");
//    drv.removeAll("QTDS7");
//    drv.removeAll("QSQLITE");
    cbDrv->addItems(drv);


    lbHst = new QLabel(tr("Хост(IP)"),this);
    lnHst = new QLineEdit(this);
    lbHst->setBuddy(lbHst);

    lbPrt = new QLabel(tr("Порт"),this);
    lnPrt = new QLineEdit(this);
    lbPrt->setBuddy(lnPrt);

    lbDbName = new QLabel(tr("Имя базы"),this);
    lnDbName = new QLineEdit(this);
    lbDbName->setBuddy(lnDbName);

    btHst = new QPushButton(tr("Сменить"),this);

    extWgtS<<lbDrv
           <<cbDrv
           <<lbHst
           <<lnHst
           <<lbPrt
           <<lnPrt
           <<lbDbName
           <<lnDbName
           <<btHst;

    foreach (QWidget *wgt, extWgtS) {
         QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;
         effect->setOpacity(1.0);
         wgt->setGraphicsEffect(effect);
//         qDebug()<<wgt;
    }

}

void WgtConnDb::createLayout()
{
    QFormLayout* frmLayout = new QFormLayout;
    frmLayout->addRow(tr("Логин:"),lnUsr);
    frmLayout->addRow(tr("Пароль"),lnPwd);
    frmLayout->addRow(chHst);

    QVBoxLayout* layoutV = new QVBoxLayout;
    layoutV->addWidget(btEntr);
    layoutV->addStretch();
    layoutV->addWidget(btExt);

    QHBoxLayout* layoutH = new QHBoxLayout;
    layoutH->addLayout(frmLayout);
    layoutH->addLayout(layoutV);

    setLayout(layoutH);

    int left, top, right, bottom;
    layoutH->getContentsMargins(&left, &top, &right, &bottom);
    margin = qMax(qMax(left, right), qMax(top, bottom));

}

void WgtConnDb::createConnect()
{
    connect(btExt,SIGNAL(clicked()),this,SLOT(close()));
    connect(chHst,SIGNAL(stateChanged(int)),this,SLOT(sltChgHst(int)));

    connect(btHst,SIGNAL(clicked()),this,SLOT(sltUpdHst()));

    connect(btEntr,SIGNAL(clicked()),this,SLOT(sltEnter()));

}

void WgtConnDb::createStateMashines()
{
    conStateMachine = new QStateMachine(this);
//qDebug()<<"WgtConnDb::createStateMashines()";
    createConStates();
    createConTransitions();
    conStateMachine->setInitialState(conToNormState);
    conStateMachine->start();

    extWgtSStateMachine = new QStateMachine(this);
    createShowExtWgtSState();
    createHideExtWgtSState();
    createHideExtWgtSTransition();
    createShowExtWgtSTransition();

    extWgtSStateMachine->setInitialState(hideExtWgtSState);
    extWgtSStateMachine->start();


}

void WgtConnDb::createConStates()
{
    conToNormState = new QState(conStateMachine);
    conToNormState->assignProperty(btEntr,"enabled",true); //"enabled" "visible"
    conToNormState->assignProperty(btExt,"enabled",true);  //"enabled" "visible"

    conToUnNormState = new QState(conStateMachine);
    conToUnNormState->assignProperty(btEntr,"enabled",false); //"enabled" "visible"
    conToUnNormState->assignProperty(btExt,"enabled",false);  //"enabled" "visible"
//qDebug()<<"WgtConnDb::createConStates()";
}

void WgtConnDb::createConTransitions()
{
    conToNormState->addTransition(this,SIGNAL(sgnCheckHst()),conToUnNormState);
    conToUnNormState->addTransition(this,SIGNAL(sgnUnCheckHst()),conToNormState);
//qDebug()<<"WgtConnDb::createConTransitions()";
}

QSize WgtConnDb::extSizeS() const
{
    const int Width  = qMax( (lbDrv->sizeHint().width() + cbDrv->sizeHint().width()),
                             (lbDbName->sizeHint().width()+ lnDbName->sizeHint().width()) )+ (2 * margin);

    const int Height = qMax(lbDrv->sizeHint().height(), cbDrv->sizeHint().height())+ margin +
                       qMax(lbHst->sizeHint().height(), lnHst->sizeHint().height())+ margin +
                       qMax(lbPrt->sizeHint().height(), lnPrt->sizeHint().height())+ margin +
                       qMax(lbDbName->sizeHint().height(),lnDbName->sizeHint().height())+ margin +
                       btHst->sizeHint().height() + margin;
    return QSize(Width, Height);

}

void WgtConnDb::createShowExtWgtSState()
{
    QSize size = extSizeS();
    size.rheight() += minimumSizeHint().height();
    size.setWidth(qMax(size.width(),minimumSizeHint().width()));

    QList<QRectF> rcts;
    int height;
    int width;

    int y = sizeHint().height() - margin; //+ btEntr->size().height();
    width  = qMin(lbDrv->sizeHint().width(),cbDrv->sizeHint().width());
    height = qMax(lbDrv->sizeHint().height(),cbDrv->sizeHint().height());
    rcts << QRectF(margin, y, width, height );                              //1

    int x =  margin +width+margin;
    width = qMin(sizeHint().width(),size.width())-(x+margin);
    rcts << QRectF(x,y+margin/2,width,height);                              //2

    y += (height + margin);

    width =  qMin(lbDrv->sizeHint().width(),lnHst->sizeHint().width());
    height = qMax(lbHst->sizeHint().height(),lnHst->sizeHint().height());
    rcts << QRectF(margin, y, width, height);                               //3

    x = margin+width+margin;
    width = qMin(sizeHint().width(),size.width())-(x+margin);
    height = qMax(lbHst->sizeHint().height(),lnHst->sizeHint().height());
    rcts << QRectF(x,y,width,height);                                       //4

    y += (height + margin);
    height = qMax(lbPrt->sizeHint().height(),lnPrt->sizeHint().height());
    rcts << QRectF(margin, y, width, height);                               //5

    height = qMax(lbPrt->sizeHint().height(),lnPrt->sizeHint().height());
    rcts << QRectF(x,y,width,height);                                       //6

    y += (height + margin);
    width = qMin(sizeHint().width(),size.width())-(x+margin);
    height = qMax(lbDbName->sizeHint().height(),lnDbName->sizeHint().height());
    rcts << QRectF(margin,y,width,height);                                  //7
    height = qMax(lbDbName->sizeHint().height(),lnDbName->sizeHint().height());
    rcts << QRectF(x,y,width,height);                                       //8

    y += (height + margin);
    height = btHst->sizeHint().height();
    width  = btHst->sizeHint().width();
    rcts << QRectF(margin, y, width, height);                               //9

    showExtWgtSState = new QState(extWgtSStateMachine);

    foreach (QWidget *wgt, extWgtS) {
        showExtWgtSState->assignProperty( wgt, "geometry", rcts.takeFirst());
        showExtWgtSState->assignProperty( wgt, "font", font());
        showExtWgtSState->assignProperty( wgt->graphicsEffect(), "opacity", 1.0);
    }

    showExtWgtSState->assignProperty(this, "minimumSize", size);

}

void WgtConnDb::createHideExtWgtSState()
{
    QRectF rct = QRectF(btExt->x(), btExt->height(), 1, 1);

    QFont smallFont(font());
    smallFont.setPointSizeF(1.0);

    hideExtWgtSState = new QState(extWgtSStateMachine);

    foreach (QWidget *wgt, extWgtS) {
        hideExtWgtSState->assignProperty( wgt, "geometry", rct);
        hideExtWgtSState->assignProperty( wgt, "font", smallFont);
        hideExtWgtSState->assignProperty( wgt->graphicsEffect(), "opacity", 0.0);
    }

    hideExtWgtSState->assignProperty( this, "minimumSize", minimumSizeHint());

}

void WgtConnDb::createShowExtWgtSTransition()
{
    QSignalTransition *tr = hideExtWgtSState->addTransition(this, SIGNAL(sgnShowExtWgtHst()), showExtWgtSState);

    createCmmnExtWgtSTransition(tr);

    QPropertyAnimation *ani;
    ani = new QPropertyAnimation(this, "minimumSize");
    ani->setDuration(Duration / 3);
    tr->addAnimation(ani);

    ani = new QPropertyAnimation(this, "size");
    ani->setDuration(Duration / 3);
    QSize size = extSizeS();
    size = QSize(qMax(size.width(), sizeHint().width()), sizeHint().height() + size.height());
    ani->setEndValue(size);
    tr->addAnimation(ani);
}

void WgtConnDb::createHideExtWgtSTransition()
{
    QSignalTransition *tr = showExtWgtSState->addTransition(this, SIGNAL(sgnHideExtWgtHst()), hideExtWgtSState );

    createCmmnExtWgtSTransition(tr);

    QPropertyAnimation *ani = new QPropertyAnimation(this,  "size");

    ani->setDuration(Duration);
    ani->setEndValue(sizeHint());
    tr->addAnimation(ani);

}

void WgtConnDb::createCmmnExtWgtSTransition(QSignalTransition *tr)
{
    QPropertyAnimation *ani;
    foreach (QWidget *wgt, extWgtS) {
        ani = new QPropertyAnimation(wgt, "geometry");
        ani->setDuration(Duration);
        tr->addAnimation(ani);
        ani = new QPropertyAnimation(wgt, "font");
        ani->setDuration(Duration);
        tr->addAnimation(ani);
        if (QGraphicsOpacityEffect *effect = static_cast<QGraphicsOpacityEffect*>(wgt->graphicsEffect()) ) {
            ani = new QPropertyAnimation(effect, "opacity");
            ani->setDuration(Duration);
            ani->setEasingCurve(QEasingCurve::OutInCirc);
            tr->addAnimation(ani);
        }
    }

}

void WgtConnDb::readSettings()
{
    QSettings st;
    QString key;
    key = tr("WgtConnDb/geometry");
    if(st.value(key).isNull()){
        return;
    }
    QRect rct;
    rct = st.value(key).toRect();
    rct.setY(rct.y()-28);
    rct.setHeight(rct.height()-28);
//qDebug()<<"readSetting"<<rct;
    this->setGeometry(rct);

    QString curKey;
    key = tr("WgtConnDb/Conection");

    curKey = key+tr("/host");
    if(!st.value(curKey).isNull()){
        hst = st.value(curKey).toString();
    }
    curKey = key+tr("/port");
    if(!st.value(curKey).isNull()){
        port = st.value(curKey).toInt();
    }
    curKey = key+tr("/dbName");
    if(!st.value(curKey).isNull()){
        dbName = st.value(curKey).toString();
    }
    curKey = key+tr("/drvName");
    if(!st.value(curKey).isNull()){
        drvName = st.value(curKey).toString();
    }
    curKey = key+tr("/idx");
    if(!st.value(curKey).isNull()){
        idx = st.value(curKey).toInt();
    }

}

void WgtConnDb::saveSettings()
{
    QSettings st;
    QString key;
    key = tr("WgtConnDb/geometry");
    QRect rct = this->geometry();
    st.setValue(key,rct);
//qDebug()<<"saveSettings"<<rct; //st.fileName();
}

void WgtConnDb::saveSettingsHst()
{
    QSettings st;
    QString key;
    QString curKey;
    key = tr("WgtConnDb/Conection");

    curKey = key+tr("/drvName");
    st.setValue(curKey,drvName);

    curKey = key+tr("/idx");
    st.setValue(curKey,idx);

    curKey = key+tr("/host");
    st.setValue(curKey,hst);

    curKey = key+tr("/port");
    st.setValue(curKey,port);

    curKey = key+tr("/dbName");
    st.setValue(curKey,dbName);

    //qDebug()<<st.fileName();

}

void WgtConnDb::sltChgHst(int state)
{
    switch(state){
    case Qt::Checked:
        lnHst->setText(hst);
        lnPrt->setText(QString::number(port));
        lnDbName->setText(dbName);
        cbDrv->setCurrentIndex(idx);
        emit sgnCheckHst();
        emit sgnShowExtWgtHst();
        break;
    case Qt::Unchecked:
        emit sgnUnCheckHst();
        emit sgnHideExtWgtHst();
        break;
    }

}

void WgtConnDb::sltUpdHst()
{
/*
    m_db = QSqlDatabase::addDatabase("QOCI",strConName);
    int prt = strPrt.toInt();
    m_db.setHostName(strHstName);
    m_db.setPort(prt);
    m_db.setDatabaseName(strDbName);
    m_db.setUserName(strUsrName);
    m_db.setPassword(strPwdS);
    if(!m_db.open()){
        QMessageBox::critical(NULL,"Ошибка",m_db.lastError().text());
        return;
    }
*/
    bool ok=false;
    if(cbDrv->currentIndex()==-1){
        QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Отстутвуют драйвера"));
        return;
    }
    if(lnHst->text().trimmed().isEmpty()){
        QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Пожалуйста, укажите хост"));
        return;
    }
    if(lnPrt->text().trimmed().isEmpty()){
        QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Пожалуйста, укажите порт"));
        return;
    }
    if(lnDbName->text().trimmed().isEmpty()){

        QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Пожалуйста, укажите Имя базы"));
        return;
    }

    drvName = cbDrv->currentText();
    idx = cbDrv->currentIndex();
    hst =  lnHst->text().trimmed();
    port = lnPrt->text().trimmed().toInt(&ok);
    if(!ok){
        QMessageBox::information(NULL,tr("ВНИМАНЕ"),tr("Значением порта, могут быть только цифры"));
        return;
    }
    dbName = lnDbName->text().trimmed();

    saveSettingsHst();
    emit sgnUnCheckHst();
    emit sgnHideExtWgtHst();
    chHst->setChecked(Qt::Unchecked);

}

void WgtConnDb::sltEnter()
{
    usr = lnUsr->text().trimmed();
    pwd = lnPwd->text().trimmed();
    if(usr.isEmpty()){
        QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Введите, пожалуйста, логин"));
        return;
    }
    if(pwd.isEmpty()){
        QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Введите, пожалуйста, пароль"));
        return;
    }
//qDebug()<<drvName;
    if(drvName==tr("QPSQL7") || drvName==tr("QPSQL7")){
        m_db = QSqlDatabase::addDatabase(drvName,strConnName);
        m_db.setHostName(hst);
        m_db.setPort(port);
        m_db.setDatabaseName(dbName);
        m_db.setUserName(usr);
        m_db.setPassword(pwd);
    }
    else if(drvName==tr("QODBC") || drvName==tr("QODBC3")){
//qDebug()<<drvName;
        m_db = QSqlDatabase::addDatabase(drvName,strConnName+"_dbf");
//qDebug()<<m_db.connectionName()<<m_db.connectOptions();
        m_db.setDatabaseName("Driver={Microsoft dbase (*.dbf)};datasource=/home/bek03/Projects/build-Fias-Desktop_Qt_5_3_0_GCC_64bit-Debug/Source_FIAS/fias_delta_dbf/addrobj.dbf");
    }
    if(m_db.open()){
        //QMessageBox::information(NULL,tr("ВНИМАНИЕ"),tr("Ok"));
//qDebug()<<"MainWindow::setDb"<<m_db.isOpen()<<m_db.isValid()<<m_db.isOpenError()<<dbName<<m_db.databaseName();
        pParent->setDb(m_db.connectionName());
        close();
    }
    else{
        QString msg= QString("Ошибка %1").arg(m_db.lastError().text());

        QMessageBox::warning(NULL,tr("ВНИМАНИЕ"),msg);

    }
}
