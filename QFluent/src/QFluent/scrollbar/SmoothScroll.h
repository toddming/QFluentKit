#ifndef SMOOTH_SCROLL_H
#define SMOOTH_SCROLL_H

#include <QObject>
#include <deque>

#include "FluentGlobal.h"

// 前置声明
class QTimer;
class QWheelEvent;
class QAbstractScrollArea;

/**
 * @brief 平滑滚动处理类
 * 
 * 该类负责处理滚动事件并实现各种平滑滚动效果
 */
class QFLUENT_EXPORT SmoothScroll : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param widget 关联的滚动区域控件
     * @param orientation 滚动方向（垂直或水平）
     */
    explicit SmoothScroll(QAbstractScrollArea* widget, 
                         Qt::Orientation orientation = Qt::Vertical);
    ~SmoothScroll() override;

    /**
     * @brief 设置平滑模式
     * @param smoothMode 平滑模式
     */
    void setSmoothMode(Fluent::SmoothMode smoothMode);

    /**
     * @brief 处理滚轮事件
     * @param event 滚轮事件
     */
    void wheelEvent(QWheelEvent* event);

private slots:
    void smoothMove();

private:
    /**
     * @brief 计算子增量
     * @param delta 总增量
     * @param stepsLeft 剩余步数
     * @return 当前步的增量
     */
    qreal calculateSubDelta(qreal delta, int stepsLeft) const;

    QAbstractScrollArea* m_widget{nullptr};
    Qt::Orientation m_orientation;
    int m_fps{60};                          // 帧率
    int m_duration{400};                    // 持续时间(毫秒)
    int m_stepsTotal{0};                    // 总步数
    qreal m_stepRatio{1.5};                 // 步长比例
    qreal m_acceleration{1.0};              // 加速度
    QWheelEvent* m_lastWheelEvent{nullptr}; // 最后一次滚轮事件
    std::deque<qint64> m_scrollTimestamps;  // 滚动时间戳队列
    std::deque<std::pair<qreal, int>> m_stepsLeftQueue; // 剩余步数队列
    QTimer* m_smoothMoveTimer{nullptr};     // 平滑移动定时器
    Fluent::SmoothMode m_smoothMode{Fluent::SmoothMode::LINEAR};
};

#endif // SMOOTH_SCROLL_H
