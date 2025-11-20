#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>

#include "FluentGlobal.h"

class SmoothScrollDelegate;
class QFLUENT_EXPORT ScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit ScrollArea(QWidget *parent = nullptr);

    void setSmoothMode(Fluent::SmoothMode smoothMode, Qt::Orientation orientation);

    void enableTransparentBackground();

    void setViewportMargins(int left, int top, int right, int bottom);

private:
    SmoothScrollDelegate *m_scrollDelegate;
};

#endif // SCROLLAREA_H
