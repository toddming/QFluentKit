#include "ScrollArea.h"
#include "scrollbar/ScrollBar.h"

#include <QTimer>

ScrollArea::ScrollArea(QWidget *parent)
    : QScrollArea(parent)
{
    setWidgetResizable(true);

    setViewportMargins(0, 0, 0, 20);
}


ScrollArea::ScrollArea(Qt::Orientation orientation, QWidget *parent)
    : ScrollArea(parent)
{
    if (orientation & Qt::Horizontal) {
        setHorizontalScrollBar(new ScrollBar(this));
    }
    if (orientation & Qt::Vertical) {
        setVerticalScrollBar(new ScrollBar(this));
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
