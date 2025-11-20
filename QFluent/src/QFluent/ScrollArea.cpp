#include "ScrollArea.h"
#include "scrollbar/ScrollBar.h"

#include <QTimer>

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
        m_scrollDelegate->getVScrollBar()->setSmoothMode(smoothMode);
    }
    if (orientation & Qt::Vertical) {
        m_scrollDelegate->getHScrollBar()->setSmoothMode(smoothMode);
    }
}

void ScrollArea::enableTransparentBackground()
{
    setStyleSheet("QScrollArea{border: none; background: transparent}");
    QWidget *widget = this->widget();
    if (widget) {
        widget->setStyleSheet("QWidget{background: transparent}");
    }
}

void ScrollArea::setViewportMargins(int left, int top, int right, int bottom)
{
    QScrollArea::setViewportMargins(left, top, right, bottom);
}
