#ifndef WGTREGIONS_H
#define WGTREGIONS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSortFilterProxyModel>
#include <QAction>

class ModelRegions;
namespace Ui {
class WgtRegions;
}

class WgtRegions : public QWidget
{
    Q_OBJECT

public:
    explicit WgtRegions(const QSqlDatabase& db,QWidget *parent = 0);
    ~WgtRegions();
    QAction* getMenuAction() const {return act; }
signals:
    void sgnExit();
private:
    Ui::WgtRegions *ui;
    QSqlDatabase m_db;
    void createConnect();

    ModelRegions* mRegions;
    QSortFilterProxyModel* mProxy;
    QAction* act;


    void refresh();
    void resizeView();
private slots:
    void sltExit();
};

#endif // WGTREGIONS_H
