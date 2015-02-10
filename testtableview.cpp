#include "testtableview.h"
#include <QHeaderView>
#include "dateitemdelegate.h"

TestTableView::TestTableView(QWidget *parent)
    :QTableView(parent)
{
    DateItemDelegate* d = new DateItemDelegate(this);
    setItemDelegateForColumn(0,d);
}

TestTableView::~TestTableView()
{

}

void TestTableView::resizeEvent(QResizeEvent *event)
{
    this->resizeColumnsToContents();
    this->horizontalHeader()->setStretchLastSection(true);
    QTableView::resizeEvent(event);
}

