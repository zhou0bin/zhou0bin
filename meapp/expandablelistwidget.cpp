#include "expandablelistwidget.h"
#include <QFont>

ExpandableListWidget::ExpandableListWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setColumnCount(1);
    setHeaderHidden(true);
}

QTreeWidgetItem* ExpandableListWidget::addTopItem(const QString &text, int id)
{
    auto *item = new QTreeWidgetItem(this);
    item->setText(0, text);
    setItemId(item, id);
    addTopLevelItem(item);
    return item;
}

QTreeWidgetItem* ExpandableListWidget::addChildItem(QTreeWidgetItem *parent, const QString &text, int id)
{
    if (!parent) return nullptr;
    auto *item = new QTreeWidgetItem();
    item->setText(0, text);
    setItemId(item, id);
    parent->addChild(item);
    return item;
}

void ExpandableListWidget::selectItem(QTreeWidgetItem *item)
{
    if (item) setCurrentItem(item);
}

void ExpandableListWidget::clearAll()
{
    clear();
}

QTreeWidgetItem* ExpandableListWidget::currentSelectedItem() const
{
    return currentItem();
}

QString ExpandableListWidget::currentSelectedText() const
{
    auto *item = currentItem();
    return item ? item->text(0) : QString();
}

int ExpandableListWidget::currentSelectedId() const
{
    auto *item = currentItem();
    return item ? getItemId(item) : -1;
}

QTreeWidgetItem* ExpandableListWidget::findItemById(int id) const
{
    // 遍历所有顶层项和子项，找ID匹配的项
    for (int i = 0; i < topLevelItemCount(); ++i) {
        auto *topItem = topLevelItem(i);
        if (getItemId(topItem) == id)
            return topItem;

        // 遍历子项
        int childCount = topItem->childCount();
        for (int j = 0; j < childCount; ++j) {
            auto *child = topItem->child(j);
            if (getItemId(child) == id)
                return child;
        }
    }
    return nullptr;
}

void ExpandableListWidget::setItemId(QTreeWidgetItem *item, int id)
{
    if (item)
        item->setData(0, Qt::UserRole, id);
}

int ExpandableListWidget::getItemId(QTreeWidgetItem *item) const
{
    if (!item)
        return -1;
    bool ok = false;
    int id = item->data(0, Qt::UserRole).toInt(&ok);
    return ok ? id : -1;
}
