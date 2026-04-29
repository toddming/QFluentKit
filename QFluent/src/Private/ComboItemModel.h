#pragma once

#include <QAbstractListModel>
#include <QIcon>
#include <QVariant>
#include <QVector>

#include "ComboItem.h"

class ComboItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CustomRole
    {
        SeparatorRole = Qt::UserRole + 1
    };

    explicit ComboItemModel(QObject *parent = nullptr);
    ~ComboItemModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Convenience API
    void addItem(const QString &text, const QVariant &userData = QVariant());
    void addItem(const QIcon &icon, const QString &text, const QVariant &userData = QVariant());
    void addItems(const QStringList &texts);
    void insertItem(int index, const QString &text, const QVariant &userData = QVariant());
    void insertItem(int index, const QIcon &icon, const QString &text, const QVariant &userData = QVariant());
    void insertItems(int index, const QStringList &texts);
    void insertSeparator(int index);
    void removeItem(int index);
    void clear();

    int count() const;
    QString itemText(int index) const;
    QIcon itemIcon(int index) const;
    QVariant itemData(int index) const;
    bool isSeparator(int index) const;

    void setItemText(int index, const QString &text);
    void setItemIcon(int index, const QIcon &icon);
    void setItemData(int index, const QVariant &value);

    int findText(const QString &text, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;
    int findData(const QVariant &data, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive) const;

private:
    using ComboItem = QFluent::ComboItem;
    QVector<ComboItem> m_items;
};
