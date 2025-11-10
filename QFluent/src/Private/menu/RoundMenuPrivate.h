#pragma once

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QListWidgetItem>

#include "Define.h"

Q_DECLARE_METATYPE(QListWidgetItem*)

class QTimer;
class QAction;
class RoundMenu;
class QHBoxLayout;
class QListWidgetItem;
class MenuActionListWidget;
class QGraphicsDropShadowEffect;
class RoundMenuPrivate : public QObject
{
    Q_OBJECT
    Q_D_CREATE(RoundMenu)
    Q_PROPERTY_CREATE_D(bool, IsSubMenu)

public:
    explicit RoundMenuPrivate(QObject* parent = nullptr);

    void handleItemClicked(QListWidgetItem* item);

    void onActionChanged();

    void onShowMenuTimeout();

    void handleItemEntered(QListWidgetItem* item);

private:
    MenuActionListWidget* _view;
    QHBoxLayout* _layout;
    QTimer* _showTimer;

    QList<QAction*> _actions;

    QGraphicsDropShadowEffect *_shadowEffect = nullptr;

    QListWidgetItem* _lastHoverSubMenuItem = nullptr;

    QListWidgetItem* _lastHoverItem = nullptr;

    QListWidgetItem* _menuItem = nullptr;

    RoundMenu* _parentMenu = nullptr;

    bool _isHideBySystem = true;

    QList<RoundMenu *> _subMenus;

    bool hasItemIcon() const;

    int longestShortcutWidth() const;

    void createSubMenuItem(RoundMenu* menu);

    QListWidgetItem* createActionItem(QAction* action, QAction* before = nullptr);

    void setShadowEffect(int blurRadius = 30, QPointF offset = QPointF(0, 8), QColor color = QColor(0, 0, 0, 30));

    void setParentMenu(RoundMenu *parent, QListWidgetItem *item);

    void removeItem(QListWidgetItem *item);

    void showSubMenu(QListWidgetItem *item);
};
