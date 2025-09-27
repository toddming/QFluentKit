#pragma once

#include <QStyledItemDelegate>


class QPainter;
class QModelIndex;
class QStyleOptionViewItem;
class MenuItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit MenuItemDelegate(QObject *parent = nullptr);

    bool isSeparator(const QModelIndex &index) const;

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

private:

};

class ShortcutMenuItemDelegate : public MenuItemDelegate
{
    Q_OBJECT

public:
    explicit ShortcutMenuItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

};


class IndicatorMenuItemDelegate : public MenuItemDelegate
{
    Q_OBJECT

public:
    explicit IndicatorMenuItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

};
