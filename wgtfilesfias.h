#ifndef WGTFILESFIAS_H
#define WGTFILESFIAS_H

#include <QWidget>
#include <QAction>
#include <QSqlDatabase>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QProgressBar>
#include <QSqlQueryModel>

namespace Ui {
class WgtFilesFias;
}

class WgtFilesFias : public QWidget
{
    Q_OBJECT

public:
    explicit WgtFilesFias(const QSqlDatabase& db, QWidget *parent = 0);
    ~WgtFilesFias();
    QAction* getMenuAction() const { return act; }
signals:
    void sgnExit();
private:
    Ui::WgtFilesFias *ui;
    QSqlDatabase m_db;
    void createConnect();
    void readSettings();
    void saveSettings();
    void switchVisibleBar(bool b);

    QAction* act;
    QString pathFiasSrc;

    QNetworkAccessManager* networkFiasAccess;
    QNetworkAccessManager* networkFilesAccess;
    QNetworkRequest rqst;
    QNetworkReply*   replyFile;
    QUrl url;

    QSqlQueryModel* mdlCmbRegions;
    void refreshCmbRegions();

    //---- для пакетного(batch) insert
    QVariantList aoidLst, formalnameLst, regioncodeLst, autocodeLst, areacodeLst, citycodeLst, ctarcodeLst, placecodeLst, streetcodeLst;
    QVariantList extrcodeLst, sextcodeLst, offnameLst, postalcodeLst, okatoLst, oktmoLst, updatedateLst, shortnameLst;
    QVariantList aolevelLst, parentguidLst, aoguidLst, previdLst, nextidLst, codeLst, plaincodeLst;
    QVariantList actstatusLst, centstatusLst, operstatusLst, currstatusLst;

    QVariantList housenumLst, eststatusLst, buildnumLst, strucnumLst, strstatusLst, houseidLst;
    QVariantList houseguidLst, startdateLst, enddateLst, statstatusLst, counterLst;

    //---- для пакетного(batch) update
    QVariantList aoguidLstUpd, buildnumLstUpd, enddateLstUpd, eststatusLstUpd, houseguidLstUpd, houseidLstUpd;
    QVariantList housenumLstUpd, statstatusLstUpd, postalcodeLstUpd, startdateLstUpd, strucnumLstUpd;
    QVariantList strstatusLstUpd, updatedateLstUpd, counterLstUpd;

    //----для построчного insert
    QString aoid, formalname, regioncode, autocode, areacode, citycode, ctarcode, placecode, streetcode, extrcode, sextcode;
    QString offname, postalcode, okato, oktmo, shortname, parentguid, aoguid, previd, nextid, code, plaincode;

    QString houseguid, houseid, housenum, strucnum, buildnum;
    int aolevel,actstatus, centstatus, operstatus, currstatus, statstatus, eststatus, strstatus, counter;
    QDate updatedate,startdate, enddate;

    QString scname, socrname;
    int level, kod_t_st;

    QSet<QString> lstHouseDelta;

    void execBatchAddrobj( QSqlQuery& qry, const QString& strQry, int& iLst);
    void execBatchHouse( QSqlQuery& qry, const QString& strQry, int& iLst);
    void execBatchUpdHouse(QSqlQuery &qry, const QString &strQry, int &iLst);

    void execOneAddrobj( QSqlQuery &qry, const QString& strQry);
    void execDelOneAddrobj(QSqlQuery &qry, const QString& strQry);

    void execOneHouse( QSqlQuery& qry, const QString& strQry);
    void execDelOneHouse( QSqlQuery& qry, const QString& strQry);
    void execDelLstHouse( QSqlQuery& qry, const QString& strQry, const QString& strIn);

    void execOneSocrBase( QSqlQuery& qry, const QString& strQry);
private slots:
    void sltExit();
    void sltSelectDirFias();
    void sltReadFiles(QNetworkReply* rply);
    void sltDownLoderInit();
    void sltRequestFiles();
    void sltDownLoaderFiles(QNetworkReply* rply);
    void sltProgressBarFile(qint64 rcv, qint64 total);
    void sltUpdateDb();
    void sltRadioDelta();
    void sltRadioFull();
    void sltChangeText(const QString& str);
    void sltChangTextL(const QString& str);
};

#endif // WGTFILESFIAS_H
