#pragma once

#include <QStyledItemDelegate>

#include "FluentGlobal.h"

class QPainter;
class QModelIndex;
class QStyleOptionViewItem;
class QAction;

class QFLUENT_EXPORT MenuItemDelegate : public QStyledItemDelegate
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

class QFLUENT_EXPORT ShortcutMenuItemDelegate : public MenuItemDelegate
{
    Q_OBJECT

public:
    explicit ShortcutMenuItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

class QFLUENT_EXPORT IndicatorMenuItemDelegate : public MenuItemDelegate
{
    Q_OBJECT

public:
    explicit IndicatorMenuItemDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

class QFLUENT_EXPORT CheckableMenuItemDelegate : public ShortcutMenuItemDelegate
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
                               const QModelIndex &index, bool checked) const = 0;
};

class QFLUENT_EXPORT RadioIndicatorMenuItemDelegate : public CheckableMenuItemDelegate
{
    Q_OBJECT

public:
    explicit RadioIndicatorMenuItemDelegate(QObject *parent = nullptr);

protected:
    void drawIndicator(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index, bool checked) const override;
};

class QFLUENT_EXPORT CheckIndicatorMenuItemDelegate : public CheckableMenuItemDelegate
{
    Q_OBJECT

public:
    explicit CheckIndicatorMenuItemDelegate(QObject *parent = nullptr);

protected:
    void drawIndicator(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index, bool checked) const override;
};
