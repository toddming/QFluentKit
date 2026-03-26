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
#include "QFluent/Menu/RoundMenu.h"
#include "QFluent/Menu/MenuActionListWidget.h"
#include "QFluent/Menu/SubMenuItemWidget.h"

RoundMenuPrivate::RoundMenuPrivate(RoundMenu *parent)
    : QObject(parent)
    , q_ptr(parent)
    , m_isSubMenu(false)
    , m_isHideBySystem(true)
    , m_isHideByClick(true)
    , m_view(nullptr)
    , m_layout(nullptr)
    , m_showTimer(nullptr)
    , m_shadowEffect(nullptr)
    , m_lastHoverSubMenuItem(nullptr)
    , m_lastHoverItem(nullptr)
    , m_menuItem(nullptr)
    , m_parentMenu(nullptr)
{
}

bool RoundMenuPrivate::hasItemIcon() const
{
    for (QAction *action : m_actions) {
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

    for (QAction *action : m_actions) {
        if (action->shortcut().isEmpty())
            continue;

        QString shortcutText = action->shortcut().toString(QKeySequence::NativeText);
        maxWidth = qMax(maxWidth, fontMetrics.horizontalAdvance(shortcutText));
    }

    return maxWidth;
}



QListWidgetItem *RoundMenuPrivate::createActionItem(QAction *action, QAction *before)
{
    Q_Q(RoundMenu);

    if (!action)
        return nullptr;

    if (!before) {
        m_actions.append(action);
    } else if (m_actions.contains(before)) {
        int index = m_actions.indexOf(before);
        m_actions.insert(index, action);
    }

    QListWidgetItem *item = new QListWidgetItem(action->icon(), action->text());

    int width = q->adjustItemText(item, action);
    item->setSizeHint(QSize(width, m_view->itemHeight()));

    if (!action->isEnabled()) {
        item->setFlags(Qt::NoItemFlags);
    }

    item->setData(Qt::UserRole, QVariant::fromValue(action));
    action->setProperty("item", QVariant::fromValue(item));
    action->setProperty("index", QVariant::fromValue(m_actions.indexOf(action)));

    connect(action, &QAction::changed, this, &RoundMenuPrivate::onActionChanged);

    return item;
}

void RoundMenuPrivate::createSubMenuItem(RoundMenu *menu)
{
    Q_Q(RoundMenu);

    if (!menu)
        return;

    m_subMenus.append(menu);

    QListWidgetItem *item = new QListWidgetItem(menu->icon(), menu->title());
    item->setData(Qt::UserRole, QVariant::fromValue(menu));

    QFontMetricsF fontMetrics(m_view->font());
    qreal width = 120;

    if (!hasItemIcon()) {
        width = 60 + fontMetrics.horizontalAdvance(menu->title());
    } else {
        QString displayText = QLatin1Char(' ') + item->text();
        item->setText(displayText);
        width = 72 + fontMetrics.horizontalAdvance(displayText);
    }

    menu->dPtr->setParentMenu(q, item);
    item->setSizeHint(QSize(width, m_view->itemHeight()));
    m_view->addItem(item);

    SubMenuItemWidget *widget = new SubMenuItemWidget(menu, item, m_view);
    connect(widget, &SubMenuItemWidget::showMenuSig, this, &RoundMenuPrivate::showSubMenu);
    m_view->setItemWidget(item, widget);
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

    m_view->adjustSize();
    q->adjustSize();
}

void RoundMenuPrivate::onShowMenuTimeout()
{
    if (!m_lastHoverSubMenuItem || m_lastHoverItem != m_lastHoverSubMenuItem)
        return;

    QVariant data = m_lastHoverItem->data(Qt::UserRole);
    if (!data.canConvert<RoundMenu *>())
        return;

    RoundMenu *subMenu = data.value<RoundMenu *>();
    if (!subMenu || subMenu->isVisible())
        return;

    QWidget *widget = m_view->itemWidget(m_lastHoverItem);
    if (!widget)
        return;

    QRect widgetRect = QRect(widget->mapToGlobal(QPoint(0, 0)), widget->size());

    int x = widgetRect.right() + 5;
    int y = widgetRect.top() - 5;

    QRect screenRect = Screen::currentScreenGeometry();
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

void RoundMenuPrivate::setShadowEffect(int blurRadius, const QPointF &offset, const QColor &color)
{
    if (!m_shadowEffect)
        return;

    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(offset);
    m_shadowEffect->setColor(color);

    m_view->setGraphicsEffect(nullptr);
    m_view->setGraphicsEffect(m_shadowEffect);
}

void RoundMenuPrivate::setParentMenu(RoundMenu *parent, QListWidgetItem *item)
{
    m_parentMenu = parent;
    m_menuItem = item;
    m_isSubMenu = (parent != nullptr);
}

void RoundMenuPrivate::removeItem(QListWidgetItem *item)
{
    if (!item)
        return;

    m_view->takeItem(m_view->row(item));
    item->setData(Qt::UserRole, QVariant());

    QWidget *widget = m_view->itemWidget(item);
    if (widget) {
        widget->deleteLater();
    }

    delete item;
}

void RoundMenuPrivate::showSubMenu(QListWidgetItem *item)
{
    if (!item)
        return;

    m_lastHoverItem = item;
    m_lastHoverSubMenuItem = item;
    m_showTimer->stop();
    m_showTimer->start();
}
