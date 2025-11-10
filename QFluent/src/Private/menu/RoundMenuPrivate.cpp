#include "RoundMenuPrivate.h"

#include <QTimer>
#include <QEvent>
#include <QAction>
#include <QVariant>
#include <QHoverEvent>
#include <QFontMetrics>
#include <QApplication>
#include <QFontMetrics>
#include <QListWidgetItem>
#include <QGraphicsDropShadowEffect>

#include "Screen.h"
#include "QFluent/menu/RoundMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"
#include "QFluent/menu/SubMenuItemWidget.h"

RoundMenuPrivate::RoundMenuPrivate(QObject* parent) : QObject{parent}
{

}

bool RoundMenuPrivate::hasItemIcon() const
{
    for (QAction *action : _actions) {
        if (!action->icon().isNull()) return true;
    }
    return false;
}


int RoundMenuPrivate::longestShortcutWidth() const
{
    Q_Q_CONST(RoundMenu);

    QFontMetrics fm(q->font());
    int maxWidth = 0;
    for (QAction *action : _actions) {
        if (action->shortcut().isEmpty()) continue;
        maxWidth = qMax(maxWidth, fm.horizontalAdvance(action->shortcut().toString(QKeySequence::NativeText)));
    }
    return maxWidth;
}

void RoundMenuPrivate::handleItemClicked(QListWidgetItem* item) {
    Q_Q(RoundMenu);

    if (QVariant v = item->data(Qt::UserRole); v.canConvert<QAction*>()) {
        QAction* action = v.value<QAction*>();
        if (action->isEnabled()) {
            action->trigger();

            if (_view) {
                QRect itemRect = _view->visualItemRect(item);
                QPointF localPos = itemRect.center();
                QPointF globalPos = _view->mapToGlobal(localPos.toPoint());
                QHoverEvent hoverLeave(QEvent::HoverLeave, localPos, globalPos);
                QApplication::sendEvent(_view->viewport(), &hoverLeave);
                _view->update();
            }
            q->close();
        }
    }
}


QListWidgetItem* RoundMenuPrivate::createActionItem(QAction* action, QAction* before) {
    Q_Q(RoundMenu);

    if (!before) {
        _actions.append(action);
    } else if (_actions.contains(before)) {
        int index = _actions.indexOf(before);
        _actions.insert(index, action);
    } else {
        qDebug() << "`before` is not in the action list";
    }
    QListWidgetItem* item = new QListWidgetItem(action->icon(), action->text());

    item->setSizeHint(QSize(q->adjustItemText(item, action), _view->itemHeight()));
    if (!action->isEnabled()) {
        item->setFlags(Qt::NoItemFlags);
    }

    item->setData(Qt::UserRole, QVariant::fromValue(action));
    action->setProperty("item", QVariant::fromValue(item));
    action->setProperty("index", QVariant::fromValue(_actions.indexOf(action)));

    connect(action, &QAction::changed, this, &RoundMenuPrivate::onActionChanged);

    return item;
}

void RoundMenuPrivate::createSubMenuItem(RoundMenu* menu)
{
    Q_Q(RoundMenu);
    _subMenus.append(menu);
    QListWidgetItem* item = new QListWidgetItem(menu->icon(), menu->title());
    item->setData(Qt::UserRole, QVariant::fromValue(menu));

    QFontMetricsF fontMetrics(_view->font());
    qreal w = 120;
    if (!hasItemIcon()) {
        w = 60 + fontMetrics.horizontalAdvance(menu->title());
    } else {
        // 添加空格增加图标和文字间距
        QString displayText = " " + item->text();
        item->setText(displayText);  // 更新显示文本
        w = 72 + fontMetrics.horizontalAdvance(displayText);
    }

    menu->d_ptr->setParentMenu(q, item);
    item->setSizeHint(QSize(w, _view->itemHeight()));
    _view->addItem(item);

    // 创建子菜单项小部件
    SubMenuItemWidget* widget = new SubMenuItemWidget(menu, item, _view);
    connect(widget, &SubMenuItemWidget::showMenuSig, this, &RoundMenuPrivate::showSubMenu);
    _view->setItemWidget(item, widget);
}

void RoundMenuPrivate::onActionChanged()
{
    Q_Q(RoundMenu);
    QAction *action = qobject_cast<QAction *>(sender());
    QListWidgetItem* item = qvariant_cast<QListWidgetItem*>(action->property("item"));

    q->adjustItemText(item, action);

    if (action->isEnabled()) {
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    } else {
        item->setFlags(Qt::NoItemFlags);
    }
    _view->adjustSize();
    q->adjustSize();
}


void RoundMenuPrivate::onShowMenuTimeout()
{
    if (_lastHoverSubMenuItem == nullptr || _lastHoverItem != _lastHoverSubMenuItem) {
        return;
    }

    if (QVariant v = _lastHoverItem->data(Qt::UserRole); v.canConvert<RoundMenu*>()) {
        RoundMenu* subMenu = v.value<RoundMenu*>();
        if (subMenu && !subMenu->isVisible()) {
            QWidget* widget = _view->itemWidget(_lastHoverItem);
            if (!widget) return;

            QRect widgetRect = QRect(widget->mapToGlobal(QPoint(0, 0)), widget->size());

            int x = widgetRect.right() + 5;
            int y = widgetRect.top() - 5;  // 对齐顶部，略上移

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
    }
}

void RoundMenuPrivate::handleItemEntered(QListWidgetItem* item) {
    _lastHoverItem = item;
    if (item->data(Qt::UserRole).canConvert<RoundMenu*>()) {
        _lastHoverSubMenuItem = item;
        _showTimer->start();
    }
}

void RoundMenuPrivate::setShadowEffect(int blurRadius, QPointF offset, QColor color)
{
    _shadowEffect->setBlurRadius(blurRadius);
    _shadowEffect->setOffset(offset);
    _shadowEffect->setColor(color);
    _view->setGraphicsEffect(nullptr);
    _view->setGraphicsEffect(_shadowEffect);
}

void RoundMenuPrivate::setParentMenu(RoundMenu *parent, QListWidgetItem *item)
{
    _parentMenu = parent;
    _menuItem = item;
    _pIsSubMenu = (parent != nullptr);

}

void RoundMenuPrivate::removeItem(QListWidgetItem *item)
{
    _view->takeItem(_view->row(item));
    item->setData(Qt::UserRole, {});
    QWidget *widget = _view->itemWidget(item);
    if (widget)
        widget->deleteLater();
}


void RoundMenuPrivate::showSubMenu(QListWidgetItem *item)
{
    _lastHoverItem = item;
    _lastHoverSubMenuItem = item;
    _showTimer->stop();
    _showTimer->start();
}
