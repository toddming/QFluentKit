#include "ScrollArea.h"
#include "Scrollbar/ScrollBar.h"
#include "Scrollbar/SmoothScroll.h"

#include <QWheelEvent>
#include <QKeyEvent>

// ===================== ScrollArea =====================
ScrollArea::ScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    m_scrollDelegate = new SmoothScrollDelegate(this);

    setWidgetResizable(true);
    setViewportMargins(0, 0, 0, 20);
}

void ScrollArea::setSmoothMode(Fluent::SmoothMode smoothMode, Qt::Orientation orientation)
{
    if (orientation & Qt::Horizontal) {
        m_scrollDelegate->getHScroll()->setSmoothMode(smoothMode);
    }
    if (orientation & Qt::Vertical) {
        m_scrollDelegate->getVScroll()->setSmoothMode(smoothMode);
    }
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

// ===================== SingleDirectionScrollArea =====================
SingleDirectionScrollArea::SingleDirectionScrollArea(QWidget *parent, Qt::Orientation orient)
    : QScrollArea(parent)
    , m_orient(orient)
{
    // 1. 创建独立方向的 SmoothScroll（负责滚轮事件平滑）
    m_smoothScroll = new SmoothScroll(this, orient);

    // 2. 创建两个自定义滚动条（但只用一个方向的）
    m_vScrollBar = new SmoothScrollBar(Qt::Vertical, this);
    m_hScrollBar = new SmoothScrollBar(Qt::Horizontal, this);

    // 强制隐藏系统滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setWidgetResizable(true);
}

void SingleDirectionScrollArea::setSmoothMode(Fluent::SmoothMode mode)
{
    m_smoothScroll->setSmoothMode(mode);
}

void SingleDirectionScrollArea::enableTransparentBackground()
{
    setStyleSheet("QScrollArea{border: none; background: transparent}");
    if (widget()) {
        widget()->setStyleSheet("QWidget{background: transparent}");
    }
}

void SingleDirectionScrollArea::setViewportMargins(int left, int top, int right, int bottom)
{
    QScrollArea::setViewportMargins(left, top, right, bottom);
}

void SingleDirectionScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    QScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (m_orient == Qt::Vertical) {
        m_vScrollBar->setForceHidden(policy == Qt::ScrollBarAlwaysOff);
    }
}

void SingleDirectionScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
    QScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (m_orient == Qt::Horizontal) {
        m_hScrollBar->setForceHidden(policy == Qt::ScrollBarAlwaysOff);
    }
}

void SingleDirectionScrollArea::wheelEvent(QWheelEvent *e)
{
    // 水平滚动时忽略垂直滚轮，垂直滚动时忽略水平滚轮
    if ((m_orient == Qt::Vertical && e->angleDelta().x() != 0) ||
        (m_orient == Qt::Horizontal && e->angleDelta().y() == 0)) {
        return;
    }

    m_smoothScroll->wheelEvent(e);
    e->accept();  // 阻止进一步传播
}

void SingleDirectionScrollArea::keyPressEvent(QKeyEvent *e)
{
    // 水平方向时屏蔽左右箭头键
    if (m_orient == Qt::Horizontal &&
        (e->key() == Qt::Key_Left || e->key() == Qt::Key_Right)) {
        return;
    }
    QScrollArea::keyPressEvent(e);
}

// ===================== SmoothScrollArea =====================
SmoothScrollArea::SmoothScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    m_delegate = new SmoothScrollDelegate(this, true);
    setWidgetResizable(true);
}

void SmoothScrollArea::setScrollAnimation(Qt::Orientation orient, int duration, QEasingCurve::Type easing)
{
    SmoothScrollBar *bar = (orient == Qt::Horizontal)
        ? m_delegate->getHScrollBar()
        : m_delegate->getVScrollBar();

    if (bar) {
        bar->setScrollAnimation(duration, easing);
    }
}

void SmoothScrollArea::enableTransparentBackground()
{
    setStyleSheet("QScrollArea{border: none; background: transparent}");
    if (widget()) {
        widget()->setStyleSheet("QWidget{background: transparent}");
    }
}
