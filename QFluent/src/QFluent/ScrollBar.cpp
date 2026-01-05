#include "ScrollBar.h"
#include "Private/ScrollBarPrivate.h"
#include "Private/ScrollBarStyle.h"

#include <QAbstractScrollArea>
#include <QContextMenuEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWheelEvent>

// 属性实现
void ScrollBar::setAnimationEnabled(bool enabled)
{
    Q_D(ScrollBar);
    if (d->m_isAnimationEnabled != enabled)
    {
        d->m_isAnimationEnabled = enabled;
    }
}

bool ScrollBar::isAnimationEnabled() const
{
    Q_D(const ScrollBar);
    return d->m_isAnimationEnabled;
}

ScrollBar::ScrollBar(QWidget* parent)
    : QScrollBar(parent)
    , d_ptr(new ScrollBarPrivate(this))
{
    Q_D(ScrollBar);

    setSingleStep(1);
    setObjectName(QStringLiteral("ScrollBar"));
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    // 初始化成员变量
    d->m_speedLimit = 20.0;
    d->m_targetMaximum = 0;
    d->m_isAnimationEnabled = false;
    d->m_scrollValue = -1;
    d->m_isExpand = false;

    // 连接信号
    connect(this, &ScrollBar::rangeChanged, d, &ScrollBarPrivate::onRangeChanged);

    // 设置自定义样式
    ScrollBarStyle* scrollBarStyle = new ScrollBarStyle(style());
    scrollBarStyle->setScrollBar(this);
    setStyle(scrollBarStyle);

    // 创建滑动动画
    d->m_slideSmoothAnimation = new QPropertyAnimation(this, "value", this);
    d->m_slideSmoothAnimation->setEasingCurve(QEasingCurve::OutSine);
    d->m_slideSmoothAnimation->setDuration(300);
    connect(d->m_slideSmoothAnimation, &QPropertyAnimation::finished, this, [d]() {
        d->m_scrollValue = d->q_ptr->value();
    });

    // 创建展开定时器
    d->m_expandTimer = new QTimer(this);
    connect(d->m_expandTimer, &QTimer::timeout, this, [this, d, scrollBarStyle]() {
        d->m_expandTimer->stop();
        d->m_isExpand = underMouse();
        scrollBarStyle->startExpandAnimation(d->m_isExpand);
    });
}

ScrollBar::ScrollBar(Qt::Orientation orientation, QWidget* parent)
    : ScrollBar(parent)
{
    setOrientation(orientation);
}

ScrollBar::ScrollBar(QScrollBar* scrollBar, QAbstractScrollArea* parent)
    : ScrollBar(parent)
{
    Q_D(ScrollBar);

    if (!scrollBar || !parent)
    {
        qCritical() << "ScrollBar: Invalid scrollBar or parent pointer!";
        return;
    }

    d->m_originScrollArea = parent;
    d->m_originScrollBar = scrollBar;

    Qt::Orientation orientation = scrollBar->orientation();
    setOrientation(orientation);

    // 隐藏原始滚动条
    if (orientation == Qt::Horizontal)
    {
        parent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else
    {
        parent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    parent->installEventFilter(this);
    d->initAllConfig();

    // 连接原始滚动条信号
    connect(scrollBar, &QScrollBar::valueChanged, this, [d](int value) {
        d->handleScrollBarValueChanged(d->q_ptr, value);
    });

    connect(this, &QScrollBar::valueChanged, this, [d](int value) {
        d->handleScrollBarValueChanged(d->m_originScrollBar, value);
    });

    connect(scrollBar, &QScrollBar::rangeChanged, this, [d](int min, int max) {
        d->handleScrollBarRangeChanged(min, max);
    });
}

ScrollBar::~ScrollBar()
{
    // QScopedPointer会自动删除d_ptr
    // 样式对象由Qt的父子关系管理，会自动删除
    QStyle* currentStyle = style();
    if (currentStyle && currentStyle->parent() == this)
    {
        delete currentStyle;
    }
}

bool ScrollBar::event(QEvent* event)
{
    Q_D(ScrollBar);

    switch (event->type())
    {
    case QEvent::Enter:
        d->m_expandTimer->stop();
        if (!d->m_isExpand)
        {
            d->m_expandTimer->start(350);
        }
        break;

    case QEvent::Leave:
        d->m_expandTimer->stop();
        if (d->m_isExpand)
        {
            d->m_expandTimer->start(350);
        }
        break;

    default:
        break;
    }

    return QScrollBar::event(event);
}

bool ScrollBar::eventFilter(QObject* watched, QEvent* event)
{
    Q_D(ScrollBar);

    switch (event->type())
    {
    case QEvent::Show:
    case QEvent::Resize:
    case QEvent::LayoutRequest:
        d->handleScrollBarGeometry();
        break;

    default:
        break;
    }

    return QScrollBar::eventFilter(watched, event);
}

void ScrollBar::mousePressEvent(QMouseEvent* event)
{
    Q_D(ScrollBar);
    d->m_slideSmoothAnimation->stop();
    QScrollBar::mousePressEvent(event);
    d->m_scrollValue = value();
}

void ScrollBar::mouseReleaseEvent(QMouseEvent* event)
{
    Q_D(ScrollBar);
    d->m_slideSmoothAnimation->stop();
    QScrollBar::mouseReleaseEvent(event);
    d->m_scrollValue = value();
}

void ScrollBar::mouseMoveEvent(QMouseEvent* event)
{
    Q_D(ScrollBar);
    d->m_slideSmoothAnimation->stop();
    QScrollBar::mouseMoveEvent(event);
    d->m_scrollValue = value();
}

void ScrollBar::wheelEvent(QWheelEvent* event)
{
    Q_D(ScrollBar);

    int verticalDelta = event->angleDelta().y();

    if (d->m_slideSmoothAnimation->state() == QAbstractAnimation::Stopped)
    {
        d->m_scrollValue = value();
    }

    if (verticalDelta != 0)
    {
        // 检查是否已经到达边界
        if ((value() == minimum() && verticalDelta > 0) ||
            (value() == maximum() && verticalDelta < 0))
        {
            QScrollBar::wheelEvent(event);
            return;
        }
        d->scroll(event->modifiers(), verticalDelta);
    }
    else
    {
        int horizontalDelta = event->angleDelta().x();
        if ((value() == minimum() && horizontalDelta > 0) ||
            (value() == maximum() && horizontalDelta < 0))
        {
            QScrollBar::wheelEvent(event);
            return;
        }
        d->scroll(event->modifiers(), horizontalDelta);
    }

    event->accept();
}

void ScrollBar::contextMenuEvent(QContextMenuEvent* event)
{
    // 禁用上下文菜单
    event->accept();
}
