#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include "TableRowData.h"

class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = nullptr);

    // 初始化表格
    void initTable(const QStringList &valvePositions, const QStringList &wiperWorks);


    // 获取所有行的数据
    QVector<TableRowData> getTableData() const;
    void setTableDataToZero();

private:
    QStringList m_headers;
};

#endif // TABLEWIDGET_H
