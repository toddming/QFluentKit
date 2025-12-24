#include "SmoothScroll.h"
#include "FluentGlobal.h"

#include <QCoreApplication>
#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QWheelEvent>
#include <QTimer>
#include <QDateTime>
#include <cmath>

SmoothScroll::SmoothScroll(QAbstractScrollArea* widget, Qt::Orientation orientation)
    : QObject(widget)
    , m_widget(widget)
    , m_orientation(orientation)
    , m_smoothMode(Fluent::SmoothMode::LINEAR)
{
    m_smoothMoveTimer = new QTimer(this);
    connect(m_smoothMoveTimer, &QTimer::timeout,
            this, &SmoothScroll::smoothMove);
}

SmoothScroll::~SmoothScroll() {
    if (m_smoothMoveTimer) {
        m_smoothMoveTimer->stop();
    }

    // 清理最后的滚轮事件
    delete m_lastWheelEvent;
    m_lastWheelEvent = nullptr;
}

void SmoothScroll::setSmoothMode(Fluent::SmoothMode smoothMode) {
    m_smoothMode = smoothMode;
}

void SmoothScroll::wheelEvent(QWheelEvent* event) {
    if (!event || !m_widget) return;

    qreal delta = event->angleDelta().y() != 0
                  ? event->angleDelta().y()
                  : event->angleDelta().x();

    // 如果不需要平滑滚动或 delta 不是 120 的倍数，直接发送事件
    if (m_smoothMode == Fluent::SmoothMode::NO_SMOOTH ||
        std::fmod(std::abs(delta), 120.0) != 0) {
        QScrollBar* scrollBar = (m_orientation == Qt::Vertical)
                                ? m_widget->verticalScrollBar()
                                : m_widget->horizontalScrollBar();
        if (scrollBar) {
            QCoreApplication::sendEvent(scrollBar, event);
        }
        return;
    }

    // 记录滚动时间戳用于加速度计算
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    m_scrollTimestamps.push_back(now);

    // 移除 500ms 之前的时间戳
    while (!m_scrollTimestamps.empty() &&
           now - m_scrollTimestamps.front() > 500) {
        m_scrollTimestamps.pop_front();
    }

    // 计算加速度比率
    const qreal accelerationRatio = qMin(
        static_cast<qreal>(m_scrollTimestamps.size()) / 15.0,
        1.0
    );

    // 保存当前滚轮事件的副本
    delete m_lastWheelEvent;
    m_lastWheelEvent = nullptr;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_lastWheelEvent = new QWheelEvent(
        event->position(),
        event->globalPosition(),
        event->pixelDelta(),
        event->angleDelta(),
        event->buttons(),
        event->modifiers(),
        event->phase(),
        event->inverted(),
        event->source(),
        event->pointingDevice()
    );
#else
    m_lastWheelEvent = new QWheelEvent(
        event->pos(),
        event->globalPos(),
        event->pixelDelta(),
        event->angleDelta(),
        event->buttons(),
        event->modifiers(),
        event->phase(),
        event->inverted(),
        event->source()
    );
#endif

    // 计算总步骤数
    m_stepsTotal = m_fps * m_duration / 1000;

    // 应用步长比率和加速度
    delta *= m_stepRatio;
    if (m_acceleration > 0) {
        delta += delta * m_acceleration * accelerationRatio;
    }

    // 将新的滚动任务加入队列
    m_stepsLeftQueue.push_back({delta, m_stepsTotal});

    // 启动平滑移动定时器
    if (!m_smoothMoveTimer->isActive()) {
        m_smoothMoveTimer->start(1000 / m_fps);
    }
}

void SmoothScroll::smoothMove() {
    if (!m_widget || !m_lastWheelEvent) return;

    qreal totalDelta = 0.0;

    // 计算所有任务的当前步骤增量
    for (auto& task : m_stepsLeftQueue) {
        totalDelta += calculateSubDelta(task.first, task.second);
        task.second--;
    }

    // 移除已完成的任务
    while (!m_stepsLeftQueue.empty() && m_stepsLeftQueue.front().second == 0) {
        m_stepsLeftQueue.pop_front();
    }

    // 构造并发送滚轮事件
    const QPoint pixelDelta;
    const QPoint angleDelta = (m_orientation == Qt::Vertical)
        ? QPoint(0, static_cast<int>(std::round(totalDelta)))
        : QPoint(static_cast<int>(std::round(totalDelta)), 0);

    QScrollBar* scrollBar = (m_orientation == Qt::Vertical)
                            ? m_widget->verticalScrollBar()
                            : m_widget->horizontalScrollBar();

    if (!scrollBar) return;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QWheelEvent wheelEvent(
        m_lastWheelEvent->position(),
        m_lastWheelEvent->globalPosition(),
        pixelDelta,
        angleDelta,
        m_lastWheelEvent->buttons(),
        Qt::NoModifier,
        Qt::NoScrollPhase,
        false,
        m_lastWheelEvent->source(),
        m_lastWheelEvent->pointingDevice()
    );
#else
    QWheelEvent wheelEvent(
        m_lastWheelEvent->pos(),
        m_lastWheelEvent->globalPos(),
        pixelDelta,
        angleDelta,
        m_lastWheelEvent->buttons(),
        Qt::NoModifier,
        Qt::ScrollUpdate,
        false,
        m_lastWheelEvent->source()
    );
#endif

    QCoreApplication::sendEvent(scrollBar, &wheelEvent);

    // 如果所有任务都完成了，停止定时器
    if (m_stepsLeftQueue.empty()) {
        m_smoothMoveTimer->stop();
    }
}

qreal SmoothScroll::calculateSubDelta(qreal delta, int stepsLeft) const {
    const qreal m = m_stepsTotal / 2.0;
    const qreal x = std::abs(m_stepsTotal - stepsLeft - m);
    qreal result = 0.0;

    static const qreal PI = std::acos(-1.0);

    switch (m_smoothMode) {
    case Fluent::SmoothMode::NO_SMOOTH:
        result = 0.0;
        break;

    case Fluent::SmoothMode::CONSTANT:
        result = delta / m_stepsTotal;
        break;

    case Fluent::SmoothMode::LINEAR:
        result = 2.0 * delta / m_stepsTotal * (m - x) / m;
        break;

    case Fluent::SmoothMode::QUADRATI:
        result = 3.0 / 4.0 / m * (1.0 - x * x / m / m) * delta;
        break;

    case Fluent::SmoothMode::COSINE:
        result = (std::cos(x * PI / m) + 1.0) / (2.0 * m) * delta;
        break;

    default:
        result = 0.0;
        break;
    }

    return result;
}
