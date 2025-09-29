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

#include "QFluent/menu/RoundMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"

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


int RoundMenuPrivate::adjustItemText(QListWidgetItem *item, QAction *action)
{
    if (!item || !action) return 0;

    QString text = action->text();
    text.remove('&');

    QFontMetrics fm(item->font());
    int shortcutWidth = longestShortcutWidth();
    if (shortcutWidth > 0) {
        shortcutWidth += 22; // 保留空间
    }

    int width;
    bool hasIcon = hasItemIcon();

    if (!hasIcon) {
        width = 40 + fm.horizontalAdvance(text) + shortcutWidth;
    } else {
        // 添加空格以分隔图标和文本
        text = " " + text;
        int space = 4 - fm.horizontalAdvance(" ");
        width = 60 + fm.horizontalAdvance(text) + shortcutWidth + space;
    }


    return width;
}

void RoundMenuPrivate::createSubMenuItem(RoundMenu* menu) {

    QListWidgetItem* item = new QListWidgetItem(menu->icon(), menu->title());
    item->setData(Qt::UserRole, QVariant::fromValue(menu));
    // 同样使用固定的项目高度
    item->setSizeHint(QSize(120, _view->itemHeight()));
    _view->addItem(item);

    // 设置父子关系
    // menu->m_parentMenu = this;
    // menu->m_menuItem = item;
    // menu->m_isSubMenu = true;
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
                QHoverEvent hoverLeave(QEvent::HoverLeave, localPos, globalPos, localPos);
                QApplication::sendEvent(_view->viewport(), &hoverLeave);
                _view->update();
            }
            q->close();
        }
    } else if (v.canConvert<RoundMenu*>()) {
        RoundMenu* subMenu = v.value<RoundMenu*>();
        if (subMenu && !subMenu->isVisible()) {
            QPoint globalPos = _view->visualItemRect(item)
                    .bottomRight() + QPoint(5, 0);
            globalPos = _view->mapToGlobal(globalPos);
            subMenu->exec(globalPos);
        }
    }
}


QListWidgetItem* RoundMenuPrivate::createActionItem(QAction* action, QAction* before) {
    if (!before) {
        _actions.append(action);
    } else if (_actions.contains(before)) {
        int index = _actions.indexOf(before);
        _actions.insert(index, action);
    } else {
        qDebug() << "`before` is not in the action list";
    }
    QListWidgetItem* item = new QListWidgetItem(action->icon(), action->text());

    item->setSizeHint(QSize(adjustItemText(item, action), _view->itemHeight()));
    if (!action->isEnabled()) {
        item->setFlags(Qt::NoItemFlags);
    }

    item->setData(Qt::UserRole, QVariant::fromValue(action));
    action->setProperty("item", QVariant::fromValue(item));
    action->setProperty("index", QVariant::fromValue(_actions.indexOf(action)));

    connect(action, &QAction::changed, this, &RoundMenuPrivate::onActionChanged);

    return item;
}

void RoundMenuPrivate::onActionChanged()
{
    Q_Q(RoundMenu);
    QAction *action = qobject_cast<QAction *>(sender());
    QListWidgetItem* item = qvariant_cast<QListWidgetItem*>(action->property("item"));

    adjustItemText(item, action);

    if (action->isEnabled()) {
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    } else {
        item->setFlags(Qt::NoItemFlags);
    }
    _view->adjustSize();
    q->adjustSize();
}

void RoundMenuPrivate::onShowMenuTimeout() {
    if (_lastHoverItem) return;

    if (QVariant v = _lastHoverItem->data(Qt::UserRole);
            v.canConvert<RoundMenu*>()) {
        RoundMenu* subMenu = v.value<RoundMenu*>();
        if (subMenu && !subMenu->isVisible()) {
            QRect itemRect = _view->visualItemRect(_lastHoverItem);
            QPoint globalPos = itemRect.topRight() + QPoint(5, 0);
            globalPos = _view->mapToGlobal(globalPos);
            subMenu->exec(globalPos);
        }
    }
}

void RoundMenuPrivate::handleItemEntered(QListWidgetItem* item) {
    _lastHoverItem = item;
    if (item->data(Qt::UserRole).canConvert<RoundMenu*>()) {
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
    _isSubMenu = (parent != nullptr);
}

void RoundMenuPrivate::removeItem(QListWidgetItem *item)
{
    _view->takeItem(_view->row(item));
    item->setData(Qt::UserRole, QVariant(NULL));
    QWidget *widget = _view->itemWidget(item);
    if (widget)
        widget->deleteLater();
}
