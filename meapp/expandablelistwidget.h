#ifndef EXPANDABLELISTWIDGET_H
#define EXPANDABLELISTWIDGET_H

#include <QTreeWidget>

class ExpandableListWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ExpandableListWidget(QWidget *parent = nullptr);

    // 添加顶层项，带唯一ID
    QTreeWidgetItem* addTopItem(const QString &text, int id = -1);

    // 添加子项，带唯一ID
    QTreeWidgetItem* addChildItem(QTreeWidgetItem *parent, const QString &text, int id = -1);

    // 设置选中某项
    void selectItem(QTreeWidgetItem *item);

    // 清空所有
    void clearAll();

    // 获取当前选中项
    QTreeWidgetItem* currentSelectedItem() const;

    // 获取当前选中项的文本
    QString currentSelectedText() const;

    // 获取当前选中项的唯一ID，找不到返回 -1
    int currentSelectedId() const;

    // 根据唯一ID查找项，没找到返回 nullptr
    QTreeWidgetItem* findItemById(int id) const;

private:
    // 在QTreeWidgetItem的0列的UserRole里存id
    void setItemId(QTreeWidgetItem *item, int id);
    int getItemId(QTreeWidgetItem *item) const;
};

#endif // EXPANDABLELISTWIDGET_H
