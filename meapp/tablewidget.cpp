#include "tablewidget.h"
#include "tablewidget.h"
#include <QTableWidgetItem>

TableWidget::TableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    m_headers = QStringList {
        "电磁阀位置", "连喷时长",
        "点喷循环次数", "点喷开时长", "点喷关时长",
        "附带雨刮工作",
        "去污次数", "去污频率", "去污延时",
        "去水痕次数", "去水痕频率", "去水痕延时"
    };
}

void TableWidget::initTable(const QStringList &valvePositions, const QStringList &wiperWorks)
{
    Q_ASSERT(valvePositions.size() == wiperWorks.size());

    setColumnCount(m_headers.size());
    setRowCount(valvePositions.size());
    setHorizontalHeaderLabels(m_headers);

    for (int row = 0; row < valvePositions.size(); ++row) {
        // 电磁阀位置
        QTableWidgetItem *valveItem = new QTableWidgetItem(valvePositions[row]);
        valveItem->setFlags(valveItem->flags() & ~Qt::ItemIsEditable);
        setItem(row, 0, valveItem);

        // 附带雨刮工作
        QTableWidgetItem *wiperItem = new QTableWidgetItem(wiperWorks[row]);
        wiperItem->setFlags(wiperItem->flags() & ~Qt::ItemIsEditable);
        setItem(row, 5, wiperItem);

        // 其他列（默认 0）
        for (int col = 1; col < m_headers.size(); ++col) {
            if (col == 5) continue; // 已填充
            QTableWidgetItem *item = new QTableWidgetItem("0");
            item->setTextAlignment(Qt::AlignCenter);
            setItem(row, col, item);
        }
    }

    resizeColumnsToContents();
    resizeRowsToContents();
}

QVector<TableRowData> TableWidget::getTableData() const
{
    QVector<TableRowData> rows;

    for (int row = 0; row < rowCount(); ++row) {
        TableRowData data;
        data.valvePosition = item(row, 0)->text();
        data.sprayDuration = item(row, 1)->text().toInt();
        data.cycleCount    = item(row, 2)->text().toInt();
        data.openDuration  = item(row, 3)->text().toInt();
        data.closeDuration = item(row, 4)->text().toInt();

        data.wiperWork     = item(row, 5)->text();  // 附带雨刮工作

        data.cleanCount    = item(row, 6)->text().toInt();
        data.cleanFrequency= item(row, 7)->text().toInt();
        data.cleanDelay    = item(row, 8)->text().toInt();

        data.waterCount    = item(row, 9)->text().toInt();
        data.waterFrequency= item(row, 10)->text().toInt();
        data.waterDelay    = item(row, 11)->text().toInt();

        rows.append(data);
    }

    return rows;
}

void TableWidget::setTableDataToZero()
{
    for (int row = 0; row < rowCount(); ++row) {
        // 从第1列到最后一列循环（第0列阀门位置，第5列雨刮工作不改）
        for (int col = 1; col < columnCount(); ++col) {
            if (col == 5) continue; // 跳过雨刮工作列

            QTableWidgetItem *cell = item(row, col);
            if (cell) {
                cell->setText("0");
            }
        }
    }
}




