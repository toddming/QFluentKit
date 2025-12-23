#ifndef SMOOTH_SCROLL_H
#define SMOOTH_SCROLL_H

#include <QObject>
#include <deque>

#include "FluentGlobal.h"

class QTimer;
class QWheelEvent;
class QAbstractScrollArea;
class QFLUENT_EXPORT SmoothScroll : public QObject {
    Q_OBJECT
public:
    SmoothScroll(QAbstractScrollArea* widget, Qt::Orientation orient = Qt::Vertical);
    void setSmoothMode(Fluent::SmoothMode smoothMode);
    void wheelEvent(QWheelEvent* e);

private slots:
    void smoothMove();

private:
    qreal subDelta(qreal delta, int stepsLeft);

    QAbstractScrollArea* widget;
    Qt::Orientation orient;
    int fps = 60;
    int duration = 400;
    int stepsTotal = 0;
    qreal stepRatio = 1.5;
    qreal acceleration = 1;
    QWheelEvent* lastWheelEvent = nullptr;
    std::deque<qint64> scrollStamps;
    std::deque<std::pair<qreal, int>> stepsLeftQueue;
    QTimer* smoothMoveTimer;
    Fluent::SmoothMode smoothMode = Fluent::SmoothMode::LINEAR;
};

#endif // SMOOTH_SCROLL_H
