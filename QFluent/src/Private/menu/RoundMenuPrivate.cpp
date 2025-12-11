#include "RoundMenuPrivate.h"

#include <QTimer>
#include <QEvent>
#include <QAction>
#include <QVariant>
#include <QHoverEvent>
#include <QFontMetrics>
#include <QApplication>
#include <QListWidgetItem>
#include <QGraphicsDropShadowEffect>

#include "Screen.h"
#include "QFluent/menu/RoundMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"
#include "QFluent/menu/SubMenuItemWidget.h"

RoundMenuPrivate::RoundMenuPrivate(RoundMenu *parent)
    : QObject(parent)
    , q_ptr(parent)
    , isSubMenu(false)
    , isHideBySystem(true)
    , view(nullptr)
    , layout(nullptr)
    , showTimer(nullptr)
    , shadowEffect(nullptr)
    , lastHoverSubMenuItem(nullptr)
    , lastHoverItem(nullptr)
    , menuItem(nullptr)
    , parentMenu(nullptr)
{
}

bool RoundMenuPrivate::hasItemIcon() const
{
    for (QAction *action : actions) {
        if (!action->icon().isNull())
            return true;
    }
    return false;
}

int RoundMenuPrivate::longestShortcutWidth() const
{
    Q_Q(const RoundMenu);

    QFontMetrics fontMetrics(q->font());
    int maxWidth = 0;

    for (QAction *action : actions) {
        if (action->shortcut().isEmpty())
            continue;

        QString shortcutText = action->shortcut().toString(QKeySequence::NativeText);
        maxWidth = qMax(maxWidth, fontMetrics.horizontalAdvance(shortcutText));
    }

    return maxWidth;
}

void RoundMenuPrivate::onItemClicked(QListWidgetItem *item)
{
    Q_Q(RoundMenu);

    if (!item)
        return;

    QVariant data = item->data(Qt::UserRole);
    if (data.canConvert<QAction *>()) {
        QAction *action = data.value<QAction *>();
        if (action && action->isEnabled()) {
            action->trigger();

            if (view) {
                QRect itemRect = view->visualItemRect(item);
                QPointF localPos = itemRect.center();
                QPointF globalPos = view->mapToGlobal(localPos.toPoint());

                QHoverEvent hoverLeave(QEvent::HoverLeave, localPos, globalPos, Qt::NoModifier);
                QApplication::sendEvent(view->viewport(), &hoverLeave);
                view->update();
            }

            q->close();
        }
    }
}

QListWidgetItem *RoundMenuPrivate::createActionItem(QAction *action, QAction *before)
{
    Q_Q(RoundMenu);

    if (!action)
        return nullptr;

    if (!before) {
        actions.append(action);
    } else if (actions.contains(before)) {
        int index = actions.indexOf(before);
        actions.insert(index, action);
    }

    QListWidgetItem *item = new QListWidgetItem(action->icon(), action->text());

    int width = q->adjustItemText(item, action);
    item->setSizeHint(QSize(width, view->itemHeight()));

    if (!action->isEnabled()) {
        item->setFlags(Qt::NoItemFlags);
    }

    item->setData(Qt::UserRole, QVariant::fromValue(action));
    action->setProperty("item", QVariant::fromValue(item));
    action->setProperty("index", QVariant::fromValue(actions.indexOf(action)));

    connect(action, &QAction::changed, this, &RoundMenuPrivate::onActionChanged);

    return item;
}

void RoundMenuPrivate::createSubMenuItem(RoundMenu *menu)
{
    Q_Q(RoundMenu);

    if (!menu)
        return;

    subMenus.append(menu);

    QListWidgetItem *item = new QListWidgetItem(menu->icon(), menu->title());
    item->setData(Qt::UserRole, QVariant::fromValue(menu));

    QFontMetricsF fontMetrics(view->font());
    qreal width = 120;

    if (!hasItemIcon()) {
        width = 60 + fontMetrics.horizontalAdvance(menu->title());
    } else {
        QString displayText = QLatin1Char(' ') + item->text();
        item->setText(displayText);
        width = 72 + fontMetrics.horizontalAdvance(displayText);
    }

    menu->dPtr->setParentMenu(q, item);
    item->setSizeHint(QSize(width, view->itemHeight()));
    view->addItem(item);

    SubMenuItemWidget *widget = new SubMenuItemWidget(menu, item, view);
    connect(widget, &SubMenuItemWidget::showMenuSig, this, &RoundMenuPrivate::showSubMenu);
    view->setItemWidget(item, widget);
}

void RoundMenuPrivate::onActionChanged()
{
    Q_Q(RoundMenu);

    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    QVariant itemData = action->property("item");
    if (!itemData.canConvert<QListWidgetItem *>())
        return;

    QListWidgetItem *item = itemData.value<QListWidgetItem *>();
    if (!item)
        return;

    q->adjustItemText(item, action);

    if (action->isEnabled()) {
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    } else {
        item->setFlags(Qt::NoItemFlags);
    }

    view->adjustSize();
    q->adjustSize();
}

void RoundMenuPrivate::onShowMenuTimeout()
{
    if (!lastHoverSubMenuItem || lastHoverItem != lastHoverSubMenuItem)
        return;

    QVariant data = lastHoverItem->data(Qt::UserRole);
    if (!data.canConvert<RoundMenu *>())
        return;

    RoundMenu *subMenu = data.value<RoundMenu *>();
    if (!subMenu || subMenu->isVisible())
        return;

    QWidget *widget = view->itemWidget(lastHoverItem);
    if (!widget)
        return;

    QRect widgetRect = QRect(widget->mapToGlobal(QPoint(0, 0)), widget->size());

    int x = widgetRect.right() + 5;
    int y = widgetRect.top() - 5;

    QRect screenRect = Screen::getCurrentScreenGeometry();
    QSize subMenuSize = subMenu->sizeHint();

    // 右边界检测
    if ((x + subMenuSize.width()) > screenRect.right()) {
        x = qMax(widgetRect.left() - subMenuSize.width() - 5, screenRect.left());
    }

    // 下边界检测
    if ((y + subMenuSize.height()) > screenRect.bottom()) {
        y = screenRect.bottom() - subMenuSize.height();
    }

    // 上边界限制
    y = qMax(y, screenRect.top());

    subMenu->exec(QPoint(x, y));
}

void RoundMenuPrivate::onItemEntered(QListWidgetItem *item)
{
    if (!item)
        return;

    lastHoverItem = item;

    QVariant data = item->data(Qt::UserRole);
    if (data.canConvert<RoundMenu *>()) {
        lastHoverSubMenuItem = item;
        showTimer->start();
    }
}

void RoundMenuPrivate::setShadowEffect(int blurRadius, const QPointF &offset, const QColor &color)
{
    if (!shadowEffect)
        return;

    shadowEffect->setBlurRadius(blurRadius);
    shadowEffect->setOffset(offset);
    shadowEffect->setColor(color);

    view->setGraphicsEffect(nullptr);
    view->setGraphicsEffect(shadowEffect);
}

void RoundMenuPrivate::setParentMenu(RoundMenu *parent, QListWidgetItem *item)
{
    parentMenu = parent;
    menuItem = item;
    isSubMenu = (parent != nullptr);
}

void RoundMenuPrivate::removeItem(QListWidgetItem *item)
{
    if (!item)
        return;

    view->takeItem(view->row(item));
    item->setData(Qt::UserRole, QVariant());

    QWidget *widget = view->itemWidget(item);
    if (widget) {
        widget->deleteLater();
    }

    delete item;
}

void RoundMenuPrivate::showSubMenu(QListWidgetItem *item)
{
    if (!item)
        return;

    lastHoverItem = item;
    lastHoverSubMenuItem = item;
    showTimer->stop();
    showTimer->start();
}
