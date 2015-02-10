#include "wgtfilesdbf.h"
#include "ui_wgtfilesdbf.h"
#include <QDir>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QMessageBox>

//#include <QDebug>

WgtFilesDBF::WgtFilesDBF(const QString& t,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgtFilesDBF),
    ttl(t),
    m_model(new QDbf::QDbfTableModel()) //qdbf
{
//qDebug()<<"WgtFilesDBF::WgtFilesDBF";
    static int i;
    i++;

    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    QString sTtl;
    sTtl = tr("%1 %2").arg(i).arg(ttl); //this->windowTitle().trimmed()
//    sTtl = tr("%1").arg(ttl); //this->windowTitle().trimmed()
    setWindowTitle(sTtl);


    act = new QAction(sTtl,this);
    act->setCheckable(true);

    createConnect();

    ui->tableView->setModel(m_model);   //qdbf
}

WgtFilesDBF::~WgtFilesDBF()
{
    delete ui;
}

void WgtFilesDBF::createConnect()
{
    connect(act,SIGNAL(triggered()),this,SLOT(showMaximized()));
    connect(ui->pushButton_Exit,SIGNAL(clicked()),this,SLOT(sltExit()));
    connect(ui->pushButton_File,SIGNAL(clicked()),this,SLOT(sltSelectDirFilesDBF()));
}

void WgtFilesDBF::readSettings()
{

}

void WgtFilesDBF::saveSettings()
{

}

void WgtFilesDBF::sltExit()
{
    this->close();
    emit sgnExit();
}

void WgtFilesDBF::sltSelectDirFilesDBF()
{
    QString file;
    QString strPath;
    if(ttl.contains(tr("дельта"))){
        strPath = QApplication::applicationDirPath()+tr("/Source_FIAS/fias_delta_dbf");
    }
    else{
        strPath = QApplication::applicationDirPath()+tr("/Source_FIAS/fias_dbf");
    }
    QDir dirPath;
    if(!dirPath.exists(strPath)){
        dirPath.mkpath(strPath);
    }
    file =  QFileDialog::getOpenFileName(this,tr("Открыть..."),strPath,tr("Файлы (*.DBF)"));
//    strPath = QFileDialog::getExistingDirectory(this,tr("Выбирете папку..."),strPath,QFileDialog::ShowDirsOnly);
    ui->lineEdit->setText(file);
    if(file.isEmpty()){
        return;
    }

    ui->tableView->setModel(0);                                                                     //qdbf

    if (!m_model->open(file)) {                                                                     //qbbf
        const QString title = "Ошибка на открытие файла"; //QLatin1String("Ошибка на открытие файла");                            //qbbf
        const QString text = QString("Не возможно открыть файл %1").arg(file);//QString(QLatin1String("Не возможно открыть файл %1")).arg(file);       //qbbf
        QMessageBox::warning(this, title, text, QMessageBox::Ok);                                   //qbbf
        return;                                                                                     //qbbf
    }

   ui->tableView->setModel(m_model);
}
