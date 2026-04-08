#include "RoundMenu.h"

#include <QColor>
#include <QPointF>
#include <QPainter>
#include <QHoverEvent>
#include <QTimer>
#include <QApplication>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

#include "Theme.h"
#include "StyleSheet.h"
#include "MenuAnimation.h"
#include "Private/Menu/RoundMenuPrivate.h"
#include "MenuActionListWidget.h"

namespace {
    constexpr int SUBMENU_SHOW_DELAY_MS = 400;  // 子菜单显示延迟(毫秒)
}

RoundMenu::RoundMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , d_ptr(new RoundMenuPrivate(this))
{
    Q_D(RoundMenu);

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    d->m_isSubMenu = false;
    d->m_layout = new QHBoxLayout(this);
    d->m_view = new MenuActionListWidget(this);
    d->m_showTimer = new QTimer(this);
    d->m_shadowEffect = new QGraphicsDropShadowEffect(d->m_view);

    d->m_layout->addWidget(d->m_view);
    d->m_layout->setContentsMargins(12, 8, 12, 20);
    setLayout(d->m_layout);

    d->setShadowEffect();

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::MENU);

    connect(d->m_view, &QListWidget::itemClicked, this, &RoundMenu::onItemClicked);
    connect(d->m_view, &QListWidget::itemEntered, this, &RoundMenu::onItemEntered);

    d->m_showTimer->setSingleShot(true);
    d->m_showTimer->setInterval(SUBMENU_SHOW_DELAY_MS);
    connect(d->m_showTimer, &QTimer::timeout, d, &RoundMenuPrivate::onShowMenuTimeout);
}

RoundMenu::~RoundMenu()
{
    Q_D(RoundMenu);

    const QList<QListWidgetItem *> items = d->m_view->findItems(QString(), Qt::MatchContains);
    for (QListWidgetItem *item : items) {
        QVariant data = item->data(Qt::UserRole);
        if (data.canConvert<RoundMenu *>()) {
            RoundMenu *menu = data.value<RoundMenu *>();
            if (menu) {
                delete menu;
            }
        }
    }
}

void RoundMenu::setItemHeight(int height)
{
    Q_D(RoundMenu);
    d->m_view->setItemHeight(height);
}

void RoundMenu::setMaxVisibleItems(int num)
{
    Q_D(RoundMenu);
    d->m_view->setMaxVisibleItems(num);
}

void RoundMenu::addAction(QAction *action)
{
    Q_D(RoundMenu);

    if (!action)
        return;

    QListWidgetItem *item = d->createActionItem(action);
    d->m_view->addItem(item);
    adjustMenuSize();
}

void RoundMenu::insertAction(QAction *before, QAction *action)
{
    Q_D(RoundMenu);

    if (!action || !before)
        return;

    for (int i = 0; i < d->m_view->count(); ++i) {
        QListWidgetItem *item = d->m_view->item(i);
        QVariant data = item->data(Qt::UserRole);
        if (data.canConvert<QAction *>() && data.value<QAction *>() == before) {
            QListWidgetItem *newItem = d->createActionItem(action, before);
            d->m_view->insertItem(i, newItem);
            adjustMenuSize();
            return;
        }
    }

    addAction(action);
}

void RoundMenu::removeAction(QAction *action)
{
    Q_D(RoundMenu);

    if (!action)
        return;

    for (int i = 0; i < d->m_view->count(); ++i) {
        QListWidgetItem *item = d->m_view->item(i);
        QVariant data = item->data(Qt::UserRole);
        if (data.canConvert<QAction *>() && data.value<QAction *>() == action) {
            delete d->m_view->takeItem(i);
            d->m_actions.removeOne(action);
            adjustMenuSize();
            return;
        }
    }
}

void RoundMenu::addMenu(RoundMenu *menu)
{
    Q_D(RoundMenu);

    if (!menu)
        return;

    if (d->m_parentMenu) {
        d->m_parentMenu->removeMenu(menu);
    }

    d->createSubMenuItem(menu);
    adjustMenuSize();
}

void RoundMenu::insertMenu(QAction *before, RoundMenu *menu)
{
    Q_D(RoundMenu);

    if (!menu || !before)
        return;

    if (!d->m_actions.contains(before))
        return;

    d->createSubMenuItem(menu);

    QVariant itemData = before->property("item");
    if (!itemData.canConvert<QListWidgetItem *>())
        return;

    QListWidgetItem *beforeItem = itemData.value<QListWidgetItem *>();
    int index = d->m_view->row(beforeItem);

    if (index >= 0 && menu->d_ptr->m_menuItem) {
        d->m_view->insertItem(index, menu->d_ptr->m_menuItem);
        adjustMenuSize();
    }
}

void RoundMenu::removeMenu(RoundMenu *menu)
{
    Q_D(RoundMenu);

    if (!menu || !d->m_subMenus.contains(menu))
        return;

    QListWidgetItem *item = menu->d_ptr->m_menuItem;
    d->m_subMenus.removeOne(menu);
    d->removeItem(item);
    adjustMenuSize();
}

void RoundMenu::addSeparator()
{
    Q_D(RoundMenu);

    QMargins margins = d->m_view->contentsMargins();
    int width = d->m_view->width() - margins.left() - margins.right();

    QListWidgetItem *separator = new QListWidgetItem;
    separator->setFlags(Qt::NoItemFlags);
    separator->setSizeHint(QSize(width, 9));
    separator->setData(Qt::DecorationRole, QStringLiteral("separator"));

    d->m_view->addItem(separator);
    adjustMenuSize();
}

void RoundMenu::clear()
{
    Q_D(RoundMenu);

    d->m_actions.clear();
    d->m_subMenus.clear();
    d->m_view->clear();
}

void RoundMenu::exec(const QPoint &pos, bool animate, Fluent::MenuAnimation aniType)
{
    Q_D(RoundMenu);

    d->m_view->adjustSize(pos);

    if (!animate)
        aniType = Fluent::MenuAnimation::NONE;

    adjustMenuSize();

    MenuAnimationManager::make(this, aniType)->exec(pos);

    show();

    if (d->m_isSubMenu && d->m_menuItem) {
        d->m_menuItem->setSelected(true);
    }
}

QHBoxLayout* RoundMenu::hBoxLayout() const
{
    Q_D(const RoundMenu);
    return d->m_layout;
}

void RoundMenu::adjustMenuSize()
{
    Q_D(RoundMenu);

    QSize viewSize = d->m_view->size();
    QMargins margins = d->m_layout->contentsMargins();

    setFixedSize(viewSize.width() + margins.left() + margins.right(),
                 viewSize.height() + margins.top() + margins.bottom() + 2);
}

int RoundMenu::itemHeight() const
{
    Q_D(const RoundMenu);
    return d->m_view->itemHeight();
}

MenuActionListWidget *RoundMenu::view() const
{
    Q_D(const RoundMenu);
    return d->m_view;
}

void RoundMenu::setView(MenuActionListWidget *view)
{
    Q_D(RoundMenu);
    d->m_view = view;
}

void RoundMenu::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    Q_D(RoundMenu);

    emit closed();

    if (d->m_parentMenu) {
        d->m_parentMenu->close();
    }

    d->m_view->clearSelection();
    d->m_view->setCurrentItem(nullptr);
    d->m_view->clearFocus();
    d->m_isHideBySystem = true;
}

QList<QAction *> RoundMenu::menuActions() const
{
    Q_D(const RoundMenu);
    return d->m_actions;
}

void RoundMenu::setDefaultAction(QAction *action)
{
    Q_D(RoundMenu);

    if (!action || !d->m_actions.contains(action))
        return;

    QVariant itemData = action->property("item");
    if (itemData.canConvert<QListWidgetItem *>()) {
        QListWidgetItem *item = itemData.value<QListWidgetItem *>();
        if (item) {
            d->m_view->setCurrentItem(item);
        }
    }
}

void RoundMenu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}

void RoundMenu::hideMenu(bool isHideBySystem)
{
    Q_D(RoundMenu);

    d->m_isHideBySystem = isHideBySystem;
    d->m_view->clearSelection();

    if (d->m_isSubMenu) {
        hide();
    } else {
        close();
    }
}

void RoundMenu::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(RoundMenu);

    if (!d->m_isSubMenu || !d->m_parentMenu)
        return;

    QPoint globalPos = event->globalPos();

    MenuActionListWidget *parentView = d->m_parentMenu->view();
    if (!parentView || !d->m_menuItem)
        return;

    QMargins margins = parentView->contentsMargins();
    QRect itemRect = parentView->visualItemRect(d->m_menuItem);
    itemRect.translate(parentView->mapToGlobal(QPoint(0, 0)));
    itemRect.translate(margins.left(), margins.top() + 2);

    if (d->m_parentMenu->geometry().contains(globalPos) &&
            !itemRect.contains(globalPos) &&
            !geometry().contains(globalPos)) {
        parentView->clearSelection();
        hideMenu(false);
    }
}

void RoundMenu::mousePressEvent(QMouseEvent *event)
{
    Q_D(RoundMenu);

    QWidget *widget = childAt(event->pos());
    if (widget != d->m_view && !d->m_view->isAncestorOf(widget)) {
        hideMenu(true);
    }
}

void RoundMenu::addWidget(QWidget *widget, bool selectable)
{
    Q_D(RoundMenu);

    if (!widget)
        return;

    if (!widget->parent()) {
        widget->setParent(d->m_view);
    }

    QAction *action = new QAction(this);
    action->setSeparator(selectable);

    QListWidgetItem *item = d->createActionItem(action);
    item->setSizeHint(widget->size());

    d->m_view->addItem(item);
    d->m_view->setItemWidget(item, widget);
    d->m_view->adjustSize();

    if (!selectable) {
        item->setFlags(Qt::NoItemFlags);
    }

    adjustMenuSize();
}

int RoundMenu::adjustItemText(QListWidgetItem *item, QAction *action)
{
    Q_D(RoundMenu);

    if (!item || !action)
        return 0;

    QString text = action->text();
    text.remove(QLatin1Char('&'));

    QFontMetrics fontMetrics(item->font());
    int shortcutWidth = d->longestShortcutWidth();

    if (shortcutWidth > 0) {
        shortcutWidth += 22;
    }

    int width;
    bool hasIcon = d->hasItemIcon();

    if (!hasIcon) {
        width = 40 + fontMetrics.horizontalAdvance(text) + shortcutWidth;
    } else {
        text = QLatin1Char(' ') + text;
        int space = 4 - fontMetrics.horizontalAdvance(QLatin1String(" "));
        width = 60 + fontMetrics.horizontalAdvance(text) + shortcutWidth + space;
        item->setText(text);
    }

    return width;
}

void RoundMenu::setHideByClick(bool enabled)
{
    Q_D(RoundMenu);
    d->m_isHideByClick = enabled;
}

void RoundMenu::onItemClicked(QListWidgetItem *item)
{
    Q_D(RoundMenu);

    if (!item)
        return;

    QVariant data = item->data(Qt::UserRole);
    if (data.canConvert<QAction *>()) {
        QAction *action = data.value<QAction *>();
        if (action && action->isEnabled()) {
            action->trigger();

            if (d->m_view) {
                QRect itemRect = d->m_view->visualItemRect(item);
                QPointF localPos = itemRect.center();
                QPointF globalPos = d->m_view->mapToGlobal(localPos.toPoint());

                QHoverEvent hoverLeave(QEvent::HoverLeave, localPos, globalPos, Qt::NoModifier);
                QApplication::sendEvent(d->m_view->viewport(), &hoverLeave);
                d->m_view->update();
            }
            if (d->m_isHideByClick) {
                close();
            }
        }
    }
}

void RoundMenu::onItemEntered(QListWidgetItem *item)
{
    Q_D(RoundMenu);

    if (!item)
        return;

    d->m_lastHoverItem = item;

    QVariant data = item->data(Qt::UserRole);
    if (data.canConvert<RoundMenu *>()) {
        d->m_lastHoverSubMenuItem = item;
        d->m_showTimer->start();
    }
}
