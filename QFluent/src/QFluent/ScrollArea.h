#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QEasingCurve>
#include <QScrollArea>
#include "FluentGlobal.h"

class SmoothScrollDelegate;
class SmoothScroll;
class SmoothScrollBar;

class QFLUENT_EXPORT ScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit ScrollArea(QWidget *parent = nullptr);

    void setSmoothMode(Fluent::SmoothMode smoothMode, Qt::Orientation orientation = Qt::Vertical);
    void enableTransparentBackground();
    void setViewportMargins(int left, int top, int right, int bottom);

private:
    SmoothScrollDelegate *m_scrollDelegate;
};

// ===================== SingleDirectionScrollArea =====================
class QFLUENT_EXPORT SingleDirectionScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit SingleDirectionScrollArea(QWidget *parent = nullptr, Qt::Orientation orient = Qt::Vertical);

    void setSmoothMode(Fluent::SmoothMode mode);
    void enableTransparentBackground();

    // 重写策略，强制另一方向永远不显示滚动条
    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);

protected:
    void wheelEvent(QWheelEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    Qt::Orientation m_orient;
    SmoothScroll *m_smoothScroll;
    SmoothScrollBar *m_vScrollBar;                 // 自定义垂直滚动条
    SmoothScrollBar *m_hScrollBar;                 // 自定义水平滚动条
};

// ===================== SmoothScrollArea =====================
class QFLUENT_EXPORT SmoothScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit SmoothScrollArea(QWidget *parent = nullptr);

    void setScrollAnimation(Qt::Orientation orient, int duration, QEasingCurve::Type easing = QEasingCurve::OutCubic);
    void enableTransparentBackground();

private:
    SmoothScrollDelegate *m_delegate;
};

#endif // SCROLLAREA_H
