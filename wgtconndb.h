#ifndef WGTCONNDB_H
#define WGTCONNDB_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QStateMachine>
#include <QState>
#include <QSqlDatabase>

class MainWindow;

class WgtConnDb : public QWidget
{
    Q_OBJECT
public:
    explicit WgtConnDb(QWidget *parent = 0);
    ~WgtConnDb();

signals:
    void sgnCheckHst();
    void sgnShowExtWgtHst();

    void sgnUnCheckHst();
    void sgnHideExtWgtHst();

private:
    MainWindow* pParent;
    QString strConnName;
    QLineEdit*   lnUsr;
    QLineEdit*   lnPwd;
    QCheckBox*   chHst;   //сменить сервер

    QPushButton*  btEntr;
    QPushButton*  btExt;

    QLabel*      lbDrv;   //драйвера баз
    QComboBox*   cbDrv;

    QLabel*      lbHst;   //хост/IP
    QLineEdit*   lnHst;

    QLabel*      lbPrt;   //порт
    QLineEdit*   lnPrt;

    QLabel*      lbDbName; //имя базы
    QLineEdit*   lnDbName;
    QPushButton* btHst;


    void createWidgets();
    void createLayout();
    void createConnect();

    QList<QWidget*> extWgtS;

    QStateMachine* conStateMachine;
    QState*        conToNormState;
    QState*        conToUnNormState;

    QStateMachine* extWgtSStateMachine; //Для смены Сервера
    QState*        hideExtWgtSState;
    QState*        showExtWgtSState;

    void createStateMashines();
    void createConStates();
    void createConTransitions();

    int margin;

    QSize extSizeS() const;
    void createShowExtWgtSState();
    void createHideExtWgtSState();
    void createShowExtWgtSTransition();
    void createHideExtWgtSTransition();
    void createCmmnExtWgtSTransition(QSignalTransition* tr);

    void readSettings();
    void saveSettings();
    void saveSettingsHst();

    QSqlDatabase m_db;
    QString usr;
    QString pwd;
    QString hst;
    int port;
    QString dbName;
    QString drvName;
    int idx;

private slots:
    void sltChgHst(int state);
    void sltUpdHst();
    void sltEnter();


};

#endif // WGTCONNDB_H
