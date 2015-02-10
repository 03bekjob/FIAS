#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSettings>

class QMdiArea;
class QMdiSubWindow;
class QAction;
class QActionGroup;
class QMenu;
class QToolBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setDb(const QString& connName);
private:
    QSqlDatabase m_db;
    void createAction();
    void createMenu();
    void createToolBar();

    void createConnect();

    QMdiArea* pMdiA;

    QAction* p_aExit;
    QAction* p_aConnDb;
    QAction* p_aTB;
    QAction* p_aRegions;
    QAction* p_aFilesFias;
    QAction* p_aFilesDeltaDbf;
    QAction* p_aFilesDbf;
    QAction* p_aAddrObj;
    QAction* p_aTestDelegate;

    QAction*      p_aAllClsW;    // Закрыть все окна
    QAction*      p_aClsW;       // Закрыть активное окно
    QAction*      p_aCsdWin;     // Сделать каскад
    QAction*      p_aTilWin;     // Сделать мозаику
    QAction*      p_aPopWin;     // Сделать активным окно
    QActionGroup* p_aWinGrp;

    QMenu* p_mConnDb;
    QMenu* p_mTab;
    QMenu* p_mService;
    QMenu* p_mWin;
    QToolBar* pTB;
    bool     isHiddenToolBar;

    void readSettings();
    void saveSettings();

    void addWgt(QWidget* w);
    QWidget* activeWgt();      // возвращает, хранящейся в активном дочернем окне
private slots:
    void sltExit();
    void sltConnDb();
    void sltUpdActions();
    void sltTB();
    void sltPopWin();

    void sltRegions();
    void sltAddrObj();
    void sltFilesFias();
    void sltFilesDeltaDbf();
    void sltFilesDbf();
    void sltTestDelegate();
};

#endif // MAINWINDOW_H
