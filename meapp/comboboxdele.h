#ifndef COMBOBOXDELE_H
#define COMBOBOXDELE_H

#include <QStyledItemDelegate>
#include <QComboBox>

class ComboBoxDele : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ComboBoxDele(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &,
                          const QModelIndex &) const override
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem("app has no function");
        editor->addItem("auto parts has no function");
        editor->addItem("auto parts has no action");

        return editor;
    }

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const override
    {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QComboBox *combo = static_cast<QComboBox *>(editor);
        combo->setCurrentText(value);
    }

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        QComboBox *combo = static_cast<QComboBox *>(editor);
        model->setData(index, combo->currentText(), Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &) const override
    {
        editor->setGeometry(option.rect);
    }
};

#endif // COMBOBOXDELE_H
