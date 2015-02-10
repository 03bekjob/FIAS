#ifndef WGTFILESDBF_H
#define WGTFILESDBF_H

#include "qdbftablemodel.h"         //qdbf

#include <QWidget>
#include <QAction>

namespace Ui {
class WgtFilesDBF;
}

class WgtFilesDBF : public QWidget
{
    Q_OBJECT

public:
    explicit WgtFilesDBF(const QString &t, QWidget *parent = 0);
    ~WgtFilesDBF();
    QAction* getMenuAction() const { return act; }

signals:
    void sgnExit();
private:
    Ui::WgtFilesDBF *ui;
    QString ttl;
    void createConnect();
    void readSettings();
    void saveSettings();

    QAction* act;

    QDbf::QDbfTableModel *const m_model;          //qdbf
//    void openFile();                            //qdbf убрал в sltSelectDirFilesDBF()
private slots:
    void sltExit();
    void sltSelectDirFilesDBF();

};

#endif // WGTFILESDBF_H
