#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>

#include "Property.h"

class QFLUENT_EXPORT ScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit ScrollArea(QWidget *parent = nullptr);

    explicit ScrollArea(Qt::Orientation orientation, QWidget *parent = nullptr);

    void enableTransparentBackground();

    void setViewportMargins(int left, int top, int right, int bottom);

};

#endif // SCROLLAREA_H
