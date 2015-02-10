#ifndef WGTADDROBJ_H
#define WGTADDROBJ_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSortFilterProxyModel>
#include <QAction>
#include <QSqlQueryModel>

#include "modeltwnstreet.h"
#include "modellcl.h"
#include "modellclstreet.h"
#include "modelhouse.h"

namespace Ui {
class WgtAddrObj;
}

class WgtAddrObj : public QWidget
{
    Q_OBJECT

public:
    explicit WgtAddrObj(const QSqlDatabase& db,QWidget *parent = 0);
    ~WgtAddrObj();
    QAction* getMenuAction() const {return act; }
signals:
    void sgnExit();

private:
    Ui::WgtAddrObj *ui;
    QSqlDatabase m_db;
    QSortFilterProxyModel* mProxy;
    QAction* act;
    int modeAolevel;
    void createConnect();

    QSqlQueryModel* mdlCmbRegions;
    QSqlQueryModel* mdlCmbArea;
    QSqlQueryModel* mdlCmbTowns;
    QSqlQueryModel* mdlCmbStreet;

    void refreshRegions();
    void refreshArea();
    void refreshTowns();
    void refreshStreet();

    ModelTwnStreet* mdlTwnStreet;
    ModelLcl* mdlLcl;
    ModelLclStreet* mdlLclStreet;
    ModelHouse* mdlHouse;
    ModelHouse* mdlHouseLcl;

    void refreshTwnStreet();
    void resizeTwnStreetTV();
    void refreshHouse();
    void resizeHouseTV3();
    void refreshLcl();
    void resizeLclTV4();
    void refreshLclStreet();
    void resizeLclStreetTV5();
    void refreshHouseLcl();
    void resizeHouseLclTV6();

    void switchTableView(const int& aolevel);
private slots:
    void sltExit();
    void sltChangedRegions(const QString& region);
    void sltChangedArea(const QString& area);

    void sltChangedTwnStreet(const QModelIndex& cur, const QModelIndex& prev);
    void sltChangedLcl(const QModelIndex& cur, const QModelIndex& prev);
    void sltChangedLclStreet(const QModelIndex& cur, const QModelIndex& prev);
};

#endif // WGTADDROBJ_H
