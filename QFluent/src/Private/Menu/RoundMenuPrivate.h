#pragma once

#include <QObject>
#include <QColor>
#include <QPointF>
#include <QListWidgetItem>

#include "FluentGlobal.h"
#include "QFluent/Menu/RoundMenu.h"

Q_DECLARE_METATYPE(QListWidgetItem *)

class QTimer;
class QAction;
class QHBoxLayout;
class QListWidgetItem;
class MenuActionListWidget;
class QGraphicsDropShadowEffect;

class RoundMenuPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(RoundMenu)

public:
    explicit RoundMenuPrivate(RoundMenu *parent);
    ~RoundMenuPrivate() override = default;

    bool hasItemIcon() const;
    int longestShortcutWidth() const;

    void createSubMenuItem(RoundMenu *menu);
    QListWidgetItem *createActionItem(QAction *action, QAction *before = nullptr);

    void setShadowEffect(int blurRadius = 30, const QPointF &offset = QPointF(0, 8),
                        const QColor &color = QColor(0, 0, 0, 30));
    void setParentMenu(RoundMenu *parent, QListWidgetItem *item);
    void removeItem(QListWidgetItem *item);
    void showSubMenu(QListWidgetItem *item);

public slots:
    void onActionChanged();
    void onShowMenuTimeout();

public:
    RoundMenu *q_ptr = nullptr;

    bool m_isSubMenu;
    bool m_isHideBySystem;
    bool m_isHideByClick;

    MenuActionListWidget *m_view;
    QHBoxLayout *m_layout;
    QTimer *m_showTimer;

    QList<QAction *> m_actions;
    QList<RoundMenu *> m_subMenus;

    QGraphicsDropShadowEffect *m_shadowEffect;

    QListWidgetItem *m_lastHoverSubMenuItem;
    QListWidgetItem *m_lastHoverItem;
    QListWidgetItem *m_menuItem;

    RoundMenu *m_parentMenu;
};
