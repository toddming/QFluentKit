#pragma once

#include <QObject>
#include <deque>

// 前置声明
class QTimer;
class QWheelEvent;
class QAbstractScrollArea;

namespace Fluent {
    enum class SmoothMode;
}

/**
 * @brief 平滑滚动类，提供平滑的滚动动画效果
 *
 * 该类通过拦截鼠标滚轮事件，并将滚动动作分解为多个小步骤，
 * 从而实现平滑的滚动效果。支持多种滚动模式。
 */
class SmoothScroll : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param widget 关联的滚动区域部件
     * @param orientation 滚动方向（垂直或水平）
     */
    explicit SmoothScroll(QAbstractScrollArea* widget,
                         Qt::Orientation orientation = Qt::Vertical);

    ~SmoothScroll() override;

    /**
     * @brief 设置平滑滚动模式
     * @param smoothMode 滚动模式
     */
    void setSmoothMode(Fluent::SmoothMode smoothMode);

    /**
     * @brief 处理鼠标滚轮事件
     * @param event 滚轮事件指针
     */
    void wheelEvent(QWheelEvent* event);

private slots:
    /**
     * @brief 执行平滑移动的定时器槽函数
     */
    void smoothMove();

private:
    /**
     * @brief 计算子步骤的增量值
     * @param delta 总增量
     * @param stepsLeft 剩余步骤数
     * @return 当前步骤的增量值
     */
    qreal calculateSubDelta(qreal delta, int stepsLeft) const;

    QAbstractScrollArea* m_widget{nullptr};
    Qt::Orientation m_orientation;
    QTimer* m_smoothMoveTimer{nullptr};
    QWheelEvent* m_lastWheelEvent{nullptr};

    // 滚动参数
    int m_fps{60};
    int m_duration{400};
    int m_stepsTotal{0};
    qreal m_stepRatio{1.5};
    qreal m_acceleration{1.0};

    // 滚动状态
    std::deque<qint64> m_scrollTimestamps;
    std::deque<std::pair<qreal, int>> m_stepsLeftQueue;
    Fluent::SmoothMode m_smoothMode;
};

