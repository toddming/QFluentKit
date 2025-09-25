#include "ScrollArea.h"
#include "scrollbar/ScrollBar.h"

#include <QTimer>

ScrollArea::ScrollArea(Qt::Orientation orientation, QWidget *parent)
    : QScrollArea(parent)
{
    setWidgetResizable(true);
    if (orientation & Qt::Horizontal) {
        setHorizontalScrollBar(new ScrollBar(this));
    }
    if (orientation & Qt::Vertical) {
        setVerticalScrollBar(new ScrollBar(this));
    }
    setViewportMargins(0, 0, 0, 20);
}

void ScrollArea::enableTransparentBackground()
{
    setStyleSheet("QScrollArea{border: none; background: transparent}");
    QWidget *widget = this->widget();
    if (widget) {
        widget->setStyleSheet("QWidget{background: transparent}");
    }
}
