#ifndef ROUNDMENU_H
#define ROUNDMENU_H

#include <QMenu>
#include <QListWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QHBoxLayout>

#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QApplication>

#include "Theme.h"
#include "Icon.h"
#include "MenuAnimation.h"

#include "Property.h"


/////////////////MenuItemDelegate//////////////////
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

///////////////////ShortcutMenuItemDelegate///////////////////
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


///////////////////IndicatorMenuItemDelegate///////////////////
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

class MenuActionListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit MenuActionListWidget(QWidget* parent = nullptr);
    void setItemHeight(int height);
    void setMaxVisibleItems(int num);
    int maxVisibleItems() const;
    void adjustSize(const QPoint& pos = QPoint(), MenuAnimationType aniType = MenuAnimationType::NONE);
    int itemHeight() const;
    int itemsHeight() const;

    int heightForAnimation(const QPoint& pos, MenuAnimationType aniType = MenuAnimationType::NONE);
    QPoint availableViewSize(const QPoint& pos, MenuAnimationType aniType = MenuAnimationType::NONE);

protected:
    void wheelEvent(QWheelEvent* e) override;

private:
    int m_itemHeight = 28;
    int m_maxVisibleItems = -1;
};


class QFLUENT_EXPORT RoundMenu : public QMenu {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
public:
    explicit RoundMenu(const QString& title = "", QWidget* parent = nullptr);
    ~RoundMenu();

    // 核心功能
    void setItemHeight(int height);
    void setMaxVisibleItems(int num);
    void addAction(QAction* action);
    void insertAction(QAction* before, QAction* action);
    void removeAction(QAction* action);
    void addMenu(RoundMenu* menu);
    void addSeparator();
    void clear();
    QList<QAction *> menuActions();
    void setDefaultAction(QAction *action);

    void setIcon(const QIcon& icon);

    MenuActionListWidget* view();
    void adjustMenuSize();

    void hideMenu(bool isHideBySystem = false);

    // 动画控制
    qreal opacity() const { return m_opacity; }
    void setOpacity(qreal opacity);
    void exec(const QPoint& pos, bool animate = true,
              MenuAnimationType aniType = MenuAnimationType::DROP_DOWN);

    void addWidget(QWidget *widget, bool selectable = false);

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent* e) override;
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;

private slots:
    void handleItemClicked(QListWidgetItem* item);
    void handleItemEntered(QListWidgetItem* item);
    void onShowMenuTimeout();
    void onActionChanged();

private:
    void initWidgets();
    QListWidgetItem* createActionItem(QAction* action, QAction* before = nullptr);
    void createSubMenuItem(RoundMenu* menu);
    void closeParentMenu();


    int adjustItemText(QListWidgetItem *item, QAction *action);
    bool hasItemIcon() const;
    QIcon createItemIcon(QAction *action) const;
    int longestShortcutWidth() const;


    MenuActionListWidget* m_view;
    QHBoxLayout* m_layout;
    QTimer* m_showTimer;
    qreal m_opacity = 1.0;
    bool m_isSubMenu = false;
    RoundMenu* m_parentMenu = nullptr;
    QListWidgetItem* m_menuItem = nullptr;
    QListWidgetItem* m_lastHoverItem = nullptr;
    MenuAnimationManager* m_animationManager;

    QIcon _icon;
    QList<QAction*> m_actions;
};


#endif // ROUNDMENU_H
