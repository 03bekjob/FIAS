#ifndef WGTTESTDELEGATE_H
#define WGTTESTDELEGATE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QAction>
#include "modeltestdelegate.h"

namespace Ui {
class WgtTestDelegate;
}

class WgtTestDelegate : public QWidget
{
    Q_OBJECT

public:
    explicit WgtTestDelegate(const QSqlDatabase& db, QWidget *parent = 0);
    ~WgtTestDelegate();
    QAction* getMenuAction() const {return act; }
signals:
    void sgnExit();

private:
    Ui::WgtTestDelegate *ui;
    QSqlDatabase m_db;
    QAction* act;
    ModelTestDelegate* mdlTestDelegate;
    void createConnect();
private slots:
    void sltExit();

};

#endif // WGTTESTDELEGATE_H
