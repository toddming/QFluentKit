#include "smoothscroll.h"
#include <QCoreApplication>
#include <QScrollBar>
#include <QScrollArea>
#include <QWheelEvent>
#include <QTimer>
#include <QDateTime>
#include <cmath>


SmoothScroll::SmoothScroll(QAbstractScrollArea* widget, Qt::Orientation orient) : QObject(widget), widget(widget), orient(orient) {
    smoothMoveTimer = new QTimer(widget);
    connect(smoothMoveTimer, &QTimer::timeout, this, &SmoothScroll::smoothMove);
}

void SmoothScroll::setSmoothMode(Fluent::SmoothMode smoothMode) {
    this->smoothMode = smoothMode;
}

void SmoothScroll::wheelEvent(QWheelEvent* e) {
    qreal delta = e->angleDelta().y() != 0 ? e->angleDelta().y() : e->angleDelta().x();
    if (smoothMode == Fluent::SmoothMode::NO_SMOOTH || std::fmod(std::abs(delta), 120.0) != 0) {
        // Do not call widget->wheelEvent(e); instead, send the event to the viewport
        QCoreApplication::sendEvent(widget->viewport(), e);
        return;
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    scrollStamps.push_back(now);
    while (now - scrollStamps.front() > 500) {
        scrollStamps.pop_front();
    }

    qreal accerationRatio = qMin(static_cast<qreal>(scrollStamps.size()) / 15.0, 1.0);
    if (lastWheelEvent) delete lastWheelEvent;
    // Reconstruct instead of copying (*e)
    lastWheelEvent = new QWheelEvent(
        e->position(),
        e->globalPosition(),
        e->pixelDelta(),
        e->angleDelta(),
        e->buttons(),
        e->modifiers(),
        e->phase(),
        e->inverted(),
        e->source(),
        e->pointingDevice()
    );

    stepsTotal = fps * duration / 1000;

    delta *= stepRatio;
    if (acceleration > 0) {
        delta += delta * acceleration * accerationRatio;
    }

    stepsLeftQueue.push_back({delta, stepsTotal});

    smoothMoveTimer->start(1000 / fps);
}

void SmoothScroll::smoothMove() {
    qreal totalDelta = 0.0;

    for (auto& i : stepsLeftQueue) {
        totalDelta += subDelta(i.first, i.second);
        i.second--;
    }

    while (!stepsLeftQueue.empty() && stepsLeftQueue.front().second == 0) {
        stepsLeftQueue.pop_front();
    }

    QPoint pixelDelta;  // Use zero or compute if needed
    QPoint angleDelta = (orient == Qt::Vertical) ? QPoint(0, static_cast<int>(std::round(totalDelta))) : QPoint(static_cast<int>(std::round(totalDelta)), 0);
    QScrollBar* bar = (orient == Qt::Vertical) ? widget->verticalScrollBar() : widget->horizontalScrollBar();

    QWheelEvent we(
        lastWheelEvent->position(),
        lastWheelEvent->globalPosition(),
        pixelDelta,  // Often QPoint() for angle-based wheels
        angleDelta,
        lastWheelEvent->buttons(),
        Qt::NoModifier,  // Or use lastWheelEvent->modifiers()
        Qt::NoScrollPhase,  // Adjust based on context; e.g., Qt::ScrollUpdate
        false,  // inverted
        lastWheelEvent->source(),
        lastWheelEvent->pointingDevice()
    );

    QCoreApplication::sendEvent(static_cast<QObject*>(bar), &we);

    if (stepsLeftQueue.empty()) {
        smoothMoveTimer->stop();
    }
}

qreal SmoothScroll::subDelta(qreal delta, int stepsLeft) {
    qreal m = stepsTotal / 2.0;
    qreal x = std::abs(stepsTotal - stepsLeft - m);
    qreal res = 0.0;

    switch (smoothMode) {
    case Fluent::SmoothMode::NO_SMOOTH:
        res = 0.0;
        break;
    case Fluent::SmoothMode::CONSTANT:
        res = delta / stepsTotal;
        break;
    case Fluent::SmoothMode::LINEAR:
        res = 2 * delta / stepsTotal * (m - x) / m;
        break;
    case Fluent::SmoothMode::QUADRATI:
        res = 3.0 / 4.0 / m * (1 - x * x / m / m) * delta;
        break;
    case Fluent::SmoothMode::COSINE:
        res = (std::cos(x * M_PI / m) + 1) / (2 * m) * delta;
        break;
    }

    return res;
}
