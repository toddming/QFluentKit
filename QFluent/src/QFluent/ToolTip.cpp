#include "ToolTip.h"
#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QApplication>
#include <QScreen>
#include <QAbstractItemView>
#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include <QStyleOptionViewItem>
#include <QHelpEvent>
#include <QColor>

#include "Screen.h"
#include "StyleSheet.h"

// ============================================================================
// ToolTip 实现
// ============================================================================

ToolTip::ToolTip(const QString &text, QWidget *parent, QFrame *container)
    : QFrame(parent)
    , m_text(text)
    , m_duration(1000)
    , m_container(container)
    , m_timer(nullptr)
    , m_containerLayout(nullptr)
    , m_label(nullptr)
    , m_opacityAnimation(nullptr)
    , m_shadowEffect(nullptr)
{
    if (!m_container) {
        m_container = new QFrame(this);
    }

    setupUi();
}

ToolTip::~ToolTip()
{
    if (m_timer) {
        m_timer->stop();
    }
}

void ToolTip::setupUi()
{
    // 创建定时器
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ToolTip::hide);

    // 设置布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 8, 12, 12);
    mainLayout->addWidget(m_container);

    m_containerLayout = new QHBoxLayout(m_container);
    m_containerLayout->setContentsMargins(8, 6, 8, 6);

    m_label = new QLabel(m_text, this);
    m_containerLayout->addWidget(m_label);

    // 添加透明度动画
    m_opacityAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    m_opacityAnimation->setDuration(150);

    // 添加阴影效果
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(25);
    m_shadowEffect->setColor(QColor(0, 0, 0, 50));
    m_shadowEffect->setOffset(0, 5);
    m_container->setGraphicsEffect(m_shadowEffect);

    // 设置属性
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // 设置样式
    setStyleSheet();
}

void ToolTip::setStyleSheet()
{
    m_container->setObjectName("container");
    m_label->setObjectName("contentLabel");
    StyleSheet::registerWidget(this, Fluent::ThemeStyle::TOOL_TIP);
    m_label->adjustSize();
    adjustSize();
}

QFrame* ToolTip::container() const
{
    return m_container;
}

QString ToolTip::text() const
{
    return m_text;
}

void ToolTip::setText(const QString &text)
{
    m_text = text;
    m_label->setText(text);
    m_container->adjustSize();
    adjustSize();
}

int ToolTip::duration() const
{
    return m_duration;
}

void ToolTip::setDuration(int duration)
{
    m_duration = duration;
}

void ToolTip::showEvent(QShowEvent *event)
{
    m_opacityAnimation->setStartValue(0.0);
    m_opacityAnimation->setEndValue(1.0);
    m_opacityAnimation->start();

    m_timer->stop();
    if (m_duration > 0) {
        m_timer->start(m_duration + m_opacityAnimation->duration());
    }

    QFrame::showEvent(event);
}

void ToolTip::hideEvent(QHideEvent *event)
{
    m_timer->stop();
    QFrame::hideEvent(event);
}

void ToolTip::adjustPos(QWidget *widget, ToolTipPosition position)
{
    if (!widget) {
        return;
    }

    ToolTipPositionManager *manager = ToolTipPositionManager::make(position);
    if (manager) {
        move(manager->position(this, widget));
        delete manager;
    }
}

// ============================================================================
// ToolTipPositionManager 实现
// ============================================================================

QPoint ToolTipPositionManager::position(ToolTip *tooltip, QWidget *parent)
{
    if (!tooltip || !parent) {
        return QPoint();
    }

    QPoint pos = calculatePosition(tooltip, parent);
    int x = pos.x();
    int y = pos.y();

    QRect screenRect = Screen::currentScreenGeometry();
    x = qMax(screenRect.left(), qMin(x, screenRect.right() - tooltip->width() - 4));
    y = qMax(screenRect.top(), qMin(y, screenRect.bottom() - tooltip->height() - 4));

    return QPoint(x, y);
}

ToolTipPositionManager* ToolTipPositionManager::make(ToolTipPosition position)
{
    switch (position) {
        case ToolTipPosition::Top:
            return new TopToolTipManager();
        case ToolTipPosition::Bottom:
            return new BottomToolTipManager();
        case ToolTipPosition::Left:
            return new LeftToolTipManager();
        case ToolTipPosition::Right:
            return new RightToolTipManager();
        case ToolTipPosition::TopRight:
            return new TopRightToolTipManager();
        case ToolTipPosition::BottomRight:
            return new BottomRightToolTipManager();
        case ToolTipPosition::TopLeft:
            return new TopLeftToolTipManager();
        case ToolTipPosition::BottomLeft:
            return new BottomLeftToolTipManager();
        default:
            return nullptr;
    }
}

// ============================================================================
// 各个位置管理器实现
// ============================================================================

QPoint TopToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QPoint pos = parent->mapToGlobal(QPoint());
    int x = pos.x() + parent->width() / 2 - tooltip->width() / 2;
    int y = pos.y() - tooltip->height();
    return QPoint(x, y);
}

QPoint BottomToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QPoint pos = parent->mapToGlobal(QPoint());
    int x = pos.x() + parent->width() / 2 - tooltip->width() / 2;
    int y = pos.y() + parent->height();
    return QPoint(x, y);
}

QPoint LeftToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QPoint pos = parent->mapToGlobal(QPoint());
    int x = pos.x() - tooltip->width();
    int y = pos.y() + (parent->height() - tooltip->height()) / 2;
    return QPoint(x, y);
}

QPoint RightToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QPoint pos = parent->mapToGlobal(QPoint());
    int x = pos.x() + parent->width();
    int y = pos.y() + (parent->height() - tooltip->height()) / 2;
    return QPoint(x, y);
}

QPoint TopRightToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QPoint pos = parent->mapToGlobal(QPoint());
    int x = pos.x() + parent->width() - tooltip->width() +
            tooltip->layout()->contentsMargins().right();
    int y = pos.y() - tooltip->height();
    return QPoint(x, y);
}

QPoint TopLeftToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QPoint pos = parent->mapToGlobal(QPoint());
    int x = pos.x() - tooltip->layout()->contentsMargins().left();
    int y = pos.y() - tooltip->height();
    return QPoint(x, y);
}

QPoint BottomRightToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QPoint pos = parent->mapToGlobal(QPoint());
    int x = pos.x() + parent->width() - tooltip->width() +
            tooltip->layout()->contentsMargins().right();
    int y = pos.y() + parent->height();
    return QPoint(x, y);
}

QPoint BottomLeftToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QPoint pos = parent->mapToGlobal(QPoint());
    int x = pos.x() - tooltip->layout()->contentsMargins().left();
    int y = pos.y() + parent->height();
    return QPoint(x, y);
}

// ============================================================================
// ItemViewToolTipManager 实现
// ============================================================================

ItemViewToolTipManager::ItemViewToolTipManager(const QRect &itemRect)
    : m_itemRect(itemRect)
{
}

QPoint ItemViewToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QAbstractItemView *view = qobject_cast<QAbstractItemView*>(parent);
    if (!view) {
        return QPoint();
    }

    QPoint pos = view->mapToGlobal(m_itemRect.topLeft());
    int x = pos.x();
    int y = pos.y() - tooltip->height() + 10;
    return QPoint(x, y);
}

ItemViewToolTipManager* ItemViewToolTipManager::make(ItemViewToolTipType tipType,
                                                     const QRect &itemRect)
{
    switch (tipType) {
        case ItemViewToolTipType::List:
            return new ItemViewToolTipManager(itemRect);
        case ItemViewToolTipType::Table:
            return new TableItemToolTipManager(itemRect);
        default:
            return nullptr;
    }
}

// ============================================================================
// TableItemToolTipManager 实现
// ============================================================================

TableItemToolTipManager::TableItemToolTipManager(const QRect &itemRect)
    : ItemViewToolTipManager(itemRect)
{
}

QPoint TableItemToolTipManager::calculatePosition(ToolTip *tooltip, QWidget *parent)
{
    QTableView *tableView = qobject_cast<QTableView*>(parent);
    if (!tableView) {
        return ItemViewToolTipManager::calculatePosition(tooltip, parent);
    }

    QPoint pos = tableView->mapToGlobal(m_itemRect.topLeft());
    int x = pos.x();
    int y = pos.y() - tooltip->height();
    return QPoint(x, y);
}

// ============================================================================
// ToolTipFilter 实现
// ============================================================================

ToolTipFilter::ToolTipFilter(QWidget *parent, int showDelay, ToolTipPosition position)
    : QObject(parent)
    , m_isEnter(false)
    , m_tooltip(nullptr)
    , m_tooltipDelay(showDelay)
    , m_position(position)
    , m_timer(nullptr)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &ToolTipFilter::showToolTip);
}

ToolTipFilter::~ToolTipFilter()
{
    if (m_timer) {
        m_timer->stop();
    }

    if (m_tooltip) {
        m_tooltip->hide();
        m_tooltip->deleteLater();
        m_tooltip = nullptr;
    }
}

bool ToolTipFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (!event) {
        return QObject::eventFilter(obj, event);
    }

    switch (event->type()) {
        case QEvent::ToolTip:
            return true;

        case QEvent::Hide:
        case QEvent::Leave:
            hideToolTip();
            break;

        case QEvent::Enter:
            m_isEnter = true;
            if (canShowToolTip()) {
                QWidget *parentWidget = qobject_cast<QWidget*>(parent());
                if (!parentWidget) {
                    break;
                }

                if (!m_tooltip) {
                    m_tooltip = createToolTip();
                }

                int duration = parentWidget->toolTipDuration();
                if (duration > 0) {
                    m_tooltip->setDuration(duration);
                } else {
                    m_tooltip->setDuration(-1);
                }

                // 延迟显示工具提示
                m_timer->start(m_tooltipDelay);
            }
            break;

        case QEvent::MouseButtonPress:
            hideToolTip();
            break;

        default:
            break;
    }

    return QObject::eventFilter(obj, event);
}

ToolTip* ToolTipFilter::createToolTip()
{
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) {
        return nullptr;
    }

    return new ToolTip(parentWidget->toolTip(), parentWidget->window());
}

void ToolTipFilter::hideToolTip()
{
    m_isEnter = false;
    m_timer->stop();
    if (m_tooltip) {
        m_tooltip->hide();
    }
}

void ToolTipFilter::showToolTip()
{
    if (!m_isEnter || !m_tooltip) {
        return;
    }

    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) {
        return;
    }

    m_tooltip->setText(parentWidget->toolTip());
    m_tooltip->adjustPos(parentWidget, m_position);
    m_tooltip->show();
}

void ToolTipFilter::setToolTipDelay(int delay)
{
    m_tooltipDelay = delay;
}

bool ToolTipFilter::canShowToolTip() const
{
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) {
        return false;
    }

    return parentWidget->isWidgetType() &&
           !parentWidget->toolTip().isEmpty() &&
           parentWidget->isEnabled();
}

// ============================================================================
// ItemViewToolTip 实现
// ============================================================================

ItemViewToolTip::ItemViewToolTip(const QString &text, QWidget *parent)
    : ToolTip(text, parent)
{
}

void ItemViewToolTip::adjustPos(QAbstractItemView *view, const QRect &itemRect,
                               ItemViewToolTipType tooltipType)
{
    if (!view) {
        return;
    }

    ItemViewToolTipManager *manager = ItemViewToolTipManager::make(tooltipType, itemRect);
    if (manager) {
        move(manager->position(this, view));
        delete manager;
    }
}

// ============================================================================
// ItemViewToolTipDelegate 实现
// ============================================================================

ItemViewToolTipDelegate::ItemViewToolTipDelegate(QAbstractItemView *parent,
                                                 int showDelay,
                                                 ItemViewToolTipType tooltipType)
    : ToolTipFilter(parent, showDelay, ToolTipPosition::Top)
    , m_text("")
    , m_currentIndex()
    , m_tooltipDuration(-1)
    , m_tooltipType(tooltipType)
    , m_viewport(nullptr)
{
    if (!parent) {
        return;
    }

    m_viewport = parent->viewport();

    parent->installEventFilter(this);
    if (m_viewport) {
        m_viewport->installEventFilter(this);
    }

    connect(parent->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, &ItemViewToolTipDelegate::hideToolTip);
    connect(parent->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &ItemViewToolTipDelegate::hideToolTip);
}

bool ItemViewToolTipDelegate::eventFilter(QObject *obj, QEvent *event)
{
    if (!event) {
        return QObject::eventFilter(obj, event);
    }

    if (obj == parent()) {
        switch (event->type()) {
            case QEvent::Hide:
            case QEvent::Leave:
                hideToolTip();
                break;
            case QEvent::Enter:
                m_isEnter = true;
                break;
            default:
                break;
        }
    } else if (obj == m_viewport) {
        if (event->type() == QEvent::MouseButtonPress) {
            hideToolTip();
        }
    }

    return QObject::eventFilter(obj, event);
}

ToolTip* ItemViewToolTipDelegate::createToolTip()
{
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) {
        return nullptr;
    }

    return new ItemViewToolTip(m_text, parentWidget->window());
}

void ItemViewToolTipDelegate::showToolTip()
{
    if (!m_tooltip) {
        m_tooltip = createToolTip();
    }

    QAbstractItemView *view = qobject_cast<QAbstractItemView*>(parent());
    if (!view) {
        return;
    }

    ItemViewToolTip *itemTooltip = qobject_cast<ItemViewToolTip*>(m_tooltip);
    if (!itemTooltip) {
        return;
    }

    itemTooltip->setText(m_text);

    QRect rect;
    if (m_currentIndex.isValid()) {
        rect = view->visualRect(m_currentIndex);
    }

    itemTooltip->adjustPos(view, rect, m_tooltipType);
    itemTooltip->show();
}

bool ItemViewToolTipDelegate::canShowToolTip() const
{
    return true;
}

void ItemViewToolTipDelegate::setText(const QString &text)
{
    m_text = text;
    if (m_tooltip) {
        m_tooltip->setText(text);
    }
}

void ItemViewToolTipDelegate::setToolTipDuration(int duration)
{
    m_tooltipDuration = duration;
    if (m_tooltip) {
        m_tooltip->setDuration(duration);
    }
}

bool ItemViewToolTipDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view,
                                       QStyleOptionViewItem *option, const QModelIndex &index)
{
    Q_UNUSED(option);

    if (!event || !view) {
        return false;
    }

    if (event->type() == QEvent::ToolTip) {
        QString text = index.data(Qt::ToolTipRole).toString();
        if (text.isEmpty()) {
            hideToolTip();
            return false;
        }

        m_text = text;
        m_currentIndex = index;

        if (!m_tooltip) {
            m_tooltip = createToolTip();
            m_tooltip->setDuration(m_tooltipDuration);
        }

        // 延迟显示工具提示
        m_timer->start(m_tooltipDelay);
    }

    return true;
}
