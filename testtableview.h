#ifndef TESTTABLEVIEW_H
#define TESTTABLEVIEW_H

#include <QObject>
#include <QTableView>

class TestTableView : public QTableView
{
    Q_OBJECT
public:
    TestTableView(QWidget* parent=0);
    ~TestTableView();
    void resizeEvent(QResizeEvent *event);
};

#endif // TESTTABLEVIEW_H
