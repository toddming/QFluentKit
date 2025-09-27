#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>

#include "Property.h"

class QFLUENT_EXPORT ScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit ScrollArea(Qt::Orientation orientation = Qt::Vertical, QWidget *parent = nullptr);

    void enableTransparentBackground();
};

#endif // SCROLLAREA_H
