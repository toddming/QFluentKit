#pragma once

#include <QStyledItemDelegate>

class QPainter;
class QModelIndex;
class QStyleOptionViewItem;
class QAction;

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

class CheckableMenuItemDelegate : public ShortcutMenuItemDelegate
{
    Q_OBJECT

public:
    explicit CheckableMenuItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    virtual void drawIndicator(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const = 0;
};

class RadioIndicatorMenuItemDelegate : public CheckableMenuItemDelegate
{
    Q_OBJECT

public:
    explicit RadioIndicatorMenuItemDelegate(QObject *parent = nullptr);

protected:
    void drawIndicator(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;
};

class CheckIndicatorMenuItemDelegate : public CheckableMenuItemDelegate
{
    Q_OBJECT

public:
    explicit CheckIndicatorMenuItemDelegate(QObject *parent = nullptr);

protected:
    void drawIndicator(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const override;
};
