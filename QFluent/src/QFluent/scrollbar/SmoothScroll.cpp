#include "SmoothScroll.h"

#include <QAbstractScrollArea>
#include <QCoreApplication>
#include <QDateTime>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>
#include <cmath>

SmoothScroll::SmoothScroll(QAbstractScrollArea* widget, Qt::Orientation orientation)
    : QObject(widget)
    , m_widget(widget)
    , m_orientation(orientation)
{
    m_smoothMoveTimer = new QTimer(this);
    connect(m_smoothMoveTimer, &QTimer::timeout, this, &SmoothScroll::smoothMove);
}

SmoothScroll::~SmoothScroll() {
    // 清理最后一次滚轮事件
    delete m_lastWheelEvent;
    m_lastWheelEvent = nullptr;
}

void SmoothScroll::setSmoothMode(Fluent::SmoothMode smoothMode) {
    m_smoothMode = smoothMode;
}

void SmoothScroll::wheelEvent(QWheelEvent* event) {
    if (!m_widget) return;
    
    qreal delta = (event->angleDelta().y() != 0) 
                 ? event->angleDelta().y() 
                 : event->angleDelta().x();
                 
    // 无平滑模式或非标准滚轮增量,直接传递事件
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
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    m_scrollTimestamps.push_back(currentTime);
    
    // 移除500ms之前的时间戳
    while (!m_scrollTimestamps.empty() && 
           currentTime - m_scrollTimestamps.front() > 500) {
        m_scrollTimestamps.pop_front();
    }

    // 计算加速度比例
    qreal accelerationRatio = qMin(
        static_cast<qreal>(m_scrollTimestamps.size()) / 15.0, 
        1.0
    );
    
    // 保存最后一次滚轮事件的副本
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

    // 计算总步数
    m_stepsTotal = m_fps * m_duration / 1000;

    // 应用步长比例和加速度
    delta *= m_stepRatio;
    if (m_acceleration > 0) {
        delta += delta * m_acceleration * accelerationRatio;
    }

    // 添加到步数队列
    m_stepsLeftQueue.push_back({delta, m_stepsTotal});

    // 启动平滑移动定时器
    if (!m_smoothMoveTimer->isActive()) {
        m_smoothMoveTimer->start(1000 / m_fps);
    }
}

void SmoothScroll::smoothMove() {
    if (!m_widget || !m_lastWheelEvent) {
        m_smoothMoveTimer->stop();
        return;
    }
    
    qreal totalDelta = 0.0;

    // 计算所有队列中的增量总和
    for (auto& step : m_stepsLeftQueue) {
        totalDelta += calculateSubDelta(step.first, step.second);
        step.second--;
    }

    // 移除已完成的步数
    while (!m_stepsLeftQueue.empty() && m_stepsLeftQueue.front().second == 0) {
        m_stepsLeftQueue.pop_front();
    }

    // 构造新的滚轮事件
    QPoint pixelDelta;
    QPoint angleDelta = (m_orientation == Qt::Vertical) 
                       ? QPoint(0, static_cast<int>(std::round(totalDelta)))
                       : QPoint(static_cast<int>(std::round(totalDelta)), 0);
                       
    QScrollBar* scrollBar = (m_orientation == Qt::Vertical) 
                           ? m_widget->verticalScrollBar() 
                           : m_widget->horizontalScrollBar();

    if (!scrollBar) {
        m_smoothMoveTimer->stop();
        return;
    }

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

    // 如果队列为空,停止定时器
    if (m_stepsLeftQueue.empty()) {
        m_smoothMoveTimer->stop();
    }
}

qreal SmoothScroll::calculateSubDelta(qreal delta, int stepsLeft) const {
    if (m_stepsTotal <= 0) return 0.0;
    
    qreal midpoint = m_stepsTotal / 2.0;
    qreal x = std::abs(m_stepsTotal - stepsLeft - midpoint);
    qreal result = 0.0;
    static const qreal PI = std::acos(-1.0);
    
    switch (m_smoothMode) {
    case Fluent::SmoothMode::NO_SMOOTH:
        result = 0.0;
        break;
        
    case Fluent::SmoothMode::CONSTANT:
        // 恒定速度
        result = delta / m_stepsTotal;
        break;
        
    case Fluent::SmoothMode::LINEAR:
        // 线性变化(先加速后减速)
        if (midpoint > 0) {
            result = 2.0 * delta / m_stepsTotal * (midpoint - x) / midpoint;
        }
        break;
        
    case Fluent::SmoothMode::QUADRATI:
        // 二次曲线
        if (midpoint > 0) {
            result = 3.0 / 4.0 / midpoint * 
                    (1.0 - x * x / midpoint / midpoint) * delta;
        }
        break;
        
    case Fluent::SmoothMode::COSINE:
        // 余弦曲线
        if (midpoint > 0) {
            result = (std::cos(x * PI / midpoint) + 1.0) / (2.0 * midpoint) * delta;
        }
        break;
    }

    return result;
}
