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

RoundMenu::RoundMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
    , dPtr(new RoundMenuPrivate(this))
{
    Q_D(RoundMenu);

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);

    d->isSubMenu = false;
    d->layout = new QHBoxLayout(this);
    d->view = new MenuActionListWidget(this);
    d->showTimer = new QTimer(this);
    d->shadowEffect = new QGraphicsDropShadowEffect(d->view);

    d->layout->addWidget(d->view);
    d->layout->setContentsMargins(12, 8, 12, 20);
    setLayout(d->layout);

    d->setShadowEffect();

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::MENU);

    connect(d->view, &QListWidget::itemClicked, this, &RoundMenu::onItemClicked);
    connect(d->view, &QListWidget::itemEntered, this, &RoundMenu::onItemEntered);

    d->showTimer->setSingleShot(true);
    d->showTimer->setInterval(400);
    connect(d->showTimer, &QTimer::timeout, d, &RoundMenuPrivate::onShowMenuTimeout);
}

RoundMenu::~RoundMenu()
{
    Q_D(RoundMenu);

    const QList<QListWidgetItem *> items = d->view->findItems(QString(), Qt::MatchContains);
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
    d->view->setItemHeight(height);
}

void RoundMenu::setMaxVisibleItems(int num)
{
    Q_D(RoundMenu);
    d->view->setMaxVisibleItems(num);
}

void RoundMenu::addAction(QAction *action)
{
    Q_D(RoundMenu);

    if (!action)
        return;

    QListWidgetItem *item = d->createActionItem(action);
    d->view->addItem(item);
    adjustMenuSize();
}

void RoundMenu::insertAction(QAction *before, QAction *action)
{
    Q_D(RoundMenu);

    if (!action || !before)
        return;

    for (int i = 0; i < d->view->count(); ++i) {
        QListWidgetItem *item = d->view->item(i);
        QVariant data = item->data(Qt::UserRole);
        if (data.canConvert<QAction *>() && data.value<QAction *>() == before) {
            QListWidgetItem *newItem = d->createActionItem(action, before);
            d->view->insertItem(i, newItem);
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

    for (int i = 0; i < d->view->count(); ++i) {
        QListWidgetItem *item = d->view->item(i);
        QVariant data = item->data(Qt::UserRole);
        if (data.canConvert<QAction *>() && data.value<QAction *>() == action) {
            delete d->view->takeItem(i);
            d->actions.removeOne(action);
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

    if (d->parentMenu) {
        d->parentMenu->removeMenu(menu);
    }

    d->createSubMenuItem(menu);
    adjustMenuSize();
}

void RoundMenu::insertMenu(QAction *before, RoundMenu *menu)
{
    Q_D(RoundMenu);

    if (!menu || !before)
        return;

    if (!d->actions.contains(before))
        return;

    d->createSubMenuItem(menu);

    QVariant itemData = before->property("item");
    if (!itemData.canConvert<QListWidgetItem *>())
        return;

    QListWidgetItem *beforeItem = itemData.value<QListWidgetItem *>();
    int index = d->view->row(beforeItem);

    if (index >= 0 && menu->dPtr->menuItem) {
        d->view->insertItem(index, menu->dPtr->menuItem);
        adjustMenuSize();
    }
}

void RoundMenu::removeMenu(RoundMenu *menu)
{
    Q_D(RoundMenu);

    if (!menu || !d->subMenus.contains(menu))
        return;

    QListWidgetItem *item = menu->dPtr->menuItem;
    d->subMenus.removeOne(menu);
    d->removeItem(item);
    adjustMenuSize();
}

void RoundMenu::addSeparator()
{
    Q_D(RoundMenu);

    QMargins margins = d->view->contentsMargins();
    int width = d->view->width() - margins.left() - margins.right();

    QListWidgetItem *separator = new QListWidgetItem;
    separator->setFlags(Qt::NoItemFlags);
    separator->setSizeHint(QSize(width, 9));
    separator->setData(Qt::DecorationRole, QStringLiteral("separator"));

    d->view->addItem(separator);
    adjustMenuSize();
}

void RoundMenu::clear()
{
    Q_D(RoundMenu);

    d->actions.clear();
    d->subMenus.clear();
    d->view->clear();
}

void RoundMenu::exec(const QPoint &pos, bool animate, Fluent::MenuAnimation aniType)
{
    Q_D(RoundMenu);

    d->view->adjustSize(pos, aniType);

    if (!animate)
        aniType = Fluent::MenuAnimation::NONE;

    adjustMenuSize();

    MenuAnimationManager::make(this, aniType)->exec(pos);

    show();

    if (d->isSubMenu && d->menuItem) {
        d->menuItem->setSelected(true);
    }
}

QHBoxLayout* RoundMenu::hBoxLayout() const
{
    Q_D(const RoundMenu);
    return d->layout;
}

void RoundMenu::adjustMenuSize()
{
    Q_D(RoundMenu);

    QSize viewSize = d->view->size();
    QMargins margins = d->layout->contentsMargins();

    setFixedSize(viewSize.width() + margins.left() + margins.right(),
                 viewSize.height() + margins.top() + margins.bottom() + 2);
}

int RoundMenu::itemHeight() const
{
    Q_D(const RoundMenu);
    return d->view->itemHeight();
}

MenuActionListWidget *RoundMenu::view() const
{
    Q_D(const RoundMenu);
    return d->view;
}

void RoundMenu::setView(MenuActionListWidget *view)
{
    Q_D(RoundMenu);
    d->view = view;
}

void RoundMenu::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    Q_D(RoundMenu);

    emit closed();

    if (d->parentMenu) {
        d->parentMenu->close();
    }

    d->view->clearSelection();
    d->view->setCurrentItem(nullptr);
    d->view->clearFocus();
    d->isHideBySystem = true;
}

QList<QAction *> RoundMenu::menuActions() const
{
    Q_D(const RoundMenu);
    return d->actions;
}

void RoundMenu::setDefaultAction(QAction *action)
{
    Q_D(RoundMenu);

    if (!action || !d->actions.contains(action))
        return;

    QVariant itemData = action->property("item");
    if (itemData.canConvert<QListWidgetItem *>()) {
        QListWidgetItem *item = itemData.value<QListWidgetItem *>();
        if (item) {
            d->view->setCurrentItem(item);
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

    d->isHideBySystem = isHideBySystem;
    d->view->clearSelection();

    if (d->isSubMenu) {
        hide();
    } else {
        close();
    }
}

void RoundMenu::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(RoundMenu);

    if (!d->isSubMenu || !d->parentMenu)
        return;

    QPoint globalPos = event->globalPos();

    MenuActionListWidget *parentView = d->parentMenu->view();
    if (!parentView || !d->menuItem)
        return;

    QMargins margins = parentView->contentsMargins();
    QRect itemRect = parentView->visualItemRect(d->menuItem);
    itemRect.translate(parentView->mapToGlobal(QPoint(0, 0)));
    itemRect.translate(margins.left(), margins.top() + 2);

    if (d->parentMenu->geometry().contains(globalPos) &&
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
    if (widget != d->view && !d->view->isAncestorOf(widget)) {
        hideMenu(true);
    }
}

void RoundMenu::addWidget(QWidget *widget, bool selectable)
{
    Q_D(RoundMenu);

    if (!widget)
        return;

    QAction *action = new QAction(this);
    action->setSeparator(selectable);

    QListWidgetItem *item = d->createActionItem(action);
    item->setSizeHint(widget->size());

    d->view->addItem(item);
    d->view->setItemWidget(item, widget);
    d->view->adjustSize();

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
    d->isHideByClick = enabled;
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

            if (d->view) {
                QRect itemRect = d->view->visualItemRect(item);
                QPointF localPos = itemRect.center();
                QPointF globalPos = d->view->mapToGlobal(localPos.toPoint());

                QHoverEvent hoverLeave(QEvent::HoverLeave, localPos, globalPos, Qt::NoModifier);
                QApplication::sendEvent(d->view->viewport(), &hoverLeave);
                d->view->update();
            }
            if (d->isHideByClick) {
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

    d->lastHoverItem = item;

    QVariant data = item->data(Qt::UserRole);
    if (data.canConvert<RoundMenu *>()) {
        d->lastHoverSubMenuItem = item;
        d->showTimer->start();
    }
}
