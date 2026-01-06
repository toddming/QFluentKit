#include "ScrollArea.h"
#include "ScrollBar.h"


#include <QWheelEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QScroller>
#include <QScrollerProperties>

namespace {
// 滚动器配置常量
constexpr qreal DEFAULT_OVERSHOOT_DRAG_RESISTANCE = 0.35;
constexpr qreal DEFAULT_OVERSHOOT_SCROLL_TIME = 0.5;

/**
     * @brief 安全地将QScrollBar转换为ScrollBar
     * @param scrollBar 待转换的滚动条指针
     * @return 转换成功返回ScrollBar指针，失败返回nullptr
     */
ScrollBar* safeScrollBarCast(QScrollBar* scrollBar)
{
    if (!scrollBar) {
        return nullptr;
    }
    return qobject_cast<ScrollBar*>(scrollBar);
}
}

// ===================== ScrollArea =====================
ScrollArea::ScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    setHorizontalScrollBar(new ScrollBar(this));
    setVerticalScrollBar(new ScrollBar(this));

    // 初始状态下隐藏滚动条（通过自定义样式控制显示）
    QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setViewportMargins(0, 0, 0, 20);
}

void ScrollArea::enableTransparentBackground()
{
    setStyleSheet("QScrollArea{border: none; background: transparent}");
    if (widget()) {
        widget()->setStyleSheet("QWidget{background: transparent}");
    }
}

void ScrollArea::setViewportMargins(int left, int top, int right, int bottom)
{
    QScrollArea::setViewportMargins(left, top, right, bottom);
}

void ScrollArea::setGrabGestureEnabled(bool isEnable)
{
    if (isEnable)
    {
        QScroller::grabGesture(this->viewport(), QScroller::LeftMouseButtonGesture);
        QScrollerProperties properties = QScroller::scroller(this->viewport())->scrollerProperties();
        configureScrollerProperties(properties, 0.5);
        QScroller::scroller(this->viewport())->setScrollerProperties(properties);
    }
    else
    {
        QScroller::ungrabGesture(this->viewport());
    }
}

void ScrollArea::configureScrollerProperties(QScrollerProperties& properties, qreal gestureRecognitionTime)
{
    // 设置鼠标按下事件延迟（秒）
    // 延迟时间越短，滚动响应越快，但可能误触
    properties.setScrollMetric(
                QScrollerProperties::MousePressEventDelay,
                gestureRecognitionTime
                );

    // 设置过度滚动的阻力系数（0-1，越小阻力越小）
    properties.setScrollMetric(
                QScrollerProperties::OvershootDragResistanceFactor,
                DEFAULT_OVERSHOOT_DRAG_RESISTANCE
                );

    // 设置过度滚动回弹时间（秒）
    properties.setScrollMetric(
                QScrollerProperties::OvershootScrollTime,
                DEFAULT_OVERSHOOT_SCROLL_TIME
                );

    // 设置帧率为60fps，提供流畅的滚动体验
    properties.setScrollMetric(
                QScrollerProperties::FrameRate,
                QScrollerProperties::Fps60
                );
}

void ScrollArea::setOvershootEnabled(Qt::Orientation orientation, bool isEnable)
{
    QWidget* viewportWidget = viewport();
    if (!viewportWidget) {
        return;
    }

    QScroller* scroller = QScroller::scroller(viewportWidget);
    if (!scroller) {
        return;
    }

    // 根据方向选择对应的策略属性
    const QScrollerProperties::ScrollMetric metric =
            (orientation == Qt::Horizontal)
            ? QScrollerProperties::HorizontalOvershootPolicy
            : QScrollerProperties::VerticalOvershootPolicy;

    // 设置过度滚动策略
    const QScrollerProperties::OvershootPolicy policy =
            isEnable
            ? QScrollerProperties::OvershootAlwaysOn
            : QScrollerProperties::OvershootAlwaysOff;

    QScrollerProperties properties = scroller->scrollerProperties();
    properties.setScrollMetric(metric, policy);
    scroller->setScrollerProperties(properties);
}

bool ScrollArea::isOvershootEnabled(Qt::Orientation orientation) const
{
    QWidget* viewportWidget = viewport();
    if (!viewportWidget) {
        return false;
    }

    QScroller* scroller = QScroller::scroller(viewportWidget);
    if (!scroller) {
        return false;
    }

    // 根据方向选择对应的策略属性
    const QScrollerProperties::ScrollMetric metric =
            (orientation == Qt::Horizontal)
            ? QScrollerProperties::HorizontalOvershootPolicy
            : QScrollerProperties::VerticalOvershootPolicy;

    QScrollerProperties properties = scroller->scrollerProperties();
    QVariant value = properties.scrollMetric(metric);

    // 检查策略是否为启用状态
    return value.toInt() == QScrollerProperties::OvershootAlwaysOn;
}

void ScrollArea::setAnimationEnabled(Qt::Orientation orientation, bool isEnable)
{
    ScrollBar* scrollBar = getScrollBar(orientation);
    if (scrollBar) {
        scrollBar->setAnimationEnabled(isEnable);
    }
}

bool ScrollArea::isAnimationEnabled(Qt::Orientation orientation) const
{
    ScrollBar* scrollBar = getScrollBar(orientation);
    if (scrollBar) {
        return scrollBar->isAnimationEnabled();
    }
    return false;
}

ScrollBar* ScrollArea::getScrollBar(Qt::Orientation orientation) const
{
    QScrollBar* scrollBar = (orientation == Qt::Horizontal)
            ? horizontalScrollBar()
            : verticalScrollBar();

    return safeScrollBarCast(scrollBar);
}

// ===================== SingleDirectionScrollArea =====================
SingleDirectionScrollArea::SingleDirectionScrollArea(QWidget *parent, Qt::Orientation orient)
    : ScrollArea(parent)
    , m_orient(orient)
{
    setWidgetResizable(true);
}

void SingleDirectionScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    if (m_orient == Qt::Vertical) {
        QScrollArea::setVerticalScrollBarPolicy(policy);
    } else {
        QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

void SingleDirectionScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    if (m_orient == Qt::Horizontal) {
        QScrollArea::setHorizontalScrollBarPolicy(policy);
    } else {
        QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

void SingleDirectionScrollArea::wheelEvent(QWheelEvent *e)
{
    if (m_orient == Qt::Vertical) {
        if (e->angleDelta().x() != 0) {
            e->ignore();
            return;
        }
        QScrollArea::wheelEvent(e);
    } else { // Horizontal
        if (e->angleDelta().x() != 0) {
            QScrollArea::wheelEvent(e);
            return;
        } else if (e->angleDelta().y() != 0) {
            QPoint angleDelta(-e->angleDelta().y(), 0);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QWheelEvent horizontalEvent(
                        e->position(),
                        e->globalPosition(),
                        e->pixelDelta(),
                        angleDelta,
                        e->buttons(),
                        e->modifiers(),
                        e->phase(),
                        e->inverted()
                        );
#else
            QWheelEvent horizontalEvent(
                        e->pos(),                // QPointF (Qt5 的 pos() 返回 QPoint,会自动转换)
                        e->globalPos(),          // QPointF
                        e->pixelDelta(),         // QPoint
                        angleDelta,              // QPoint
                        e->buttons(),            // Qt::MouseButtons
                        e->modifiers(),          // Qt::KeyboardModifiers
                        e->phase(),              // Qt::ScrollPhase
                        e->inverted(),           // bool
                        e->source()              // Qt::MouseEventSource (Qt5 特有,且在 inverted 之后)
                        );
#endif
            QScrollArea::wheelEvent(&horizontalEvent);
            e->accept();
            return;
        } else {
            e->ignore();
            return;
        }
    }
}

void SingleDirectionScrollArea::keyPressEvent(QKeyEvent *e)
{
    if (m_orient == Qt::Vertical &&
            (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right)) {
        return;  // 忽略水平键
    }
    if (m_orient == Qt::Horizontal &&
            (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)) {
        return;  // 忽略垂直键
    }

    QScrollArea::keyPressEvent(e);
}

