#pragma once

#include <QStackedWidget>
#include <QFrame>
#include <QEasingCurve>

#include "FluentGlobal.h"

// 前置声明
QT_BEGIN_NAMESPACE
class QPropertyAnimation;
class QGraphicsOpacityEffect;
class QHBoxLayout;
class QParallelAnimationGroup;
class QSequentialAnimationGroup;
class QLabel;
QT_END_NAMESPACE


// ==================== OpacityAniStackedWidget ====================
/**
 * @brief 带淡入淡出动画的堆叠控件
 */
class OpacityAniStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit OpacityAniStackedWidget(QWidget *parent = nullptr);
    ~OpacityAniStackedWidget() override = default;

    void addWidget(QWidget *widget);
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);

private slots:
    void onAnimationFinished();

private:
    int m_nextIndex;                                // 下一个要显示的索引
    QList<QGraphicsOpacityEffect*> m_effects;       // 透明度效果列表
    QList<QPropertyAnimation*> m_animations;        // 动画列表
};


// ==================== PopUpAniInfo ====================
/**
 * @brief 弹出动画信息结构体
 */
struct PopUpAniInfo
{
    QWidget *widget;                // 控件指针
    int deltaX;                     // X轴偏移量
    int deltaY;                     // Y轴偏移量
    QPropertyAnimation *animation;  // 动画对象

    PopUpAniInfo(QWidget *w, int dx, int dy, QPropertyAnimation *ani)
        : widget(w), deltaX(dx), deltaY(dy), animation(ani) {}
};


// ==================== PopUpAniStackedWidget ====================
/**
 * @brief 带弹出动画的堆叠控件
 */
class PopUpAniStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit PopUpAniStackedWidget(QWidget *parent = nullptr);
    ~PopUpAniStackedWidget() override = default;

    void addWidget(QWidget *widget, int deltaX = 0, int deltaY = 76);
    void removeWidget(QWidget *widget);

    void setAnimationEnabled(bool isEnabled);
    bool isAnimationEnabled() const { return m_isAnimationEnabled; }

    void setCurrentIndex(int index, bool needPopOut = false,
                         bool showNextWidgetDirectly = true,
                         int duration = 250,
                         QEasingCurve easingCurve = QEasingCurve::OutQuad);
    void setCurrentWidget(QWidget *widget, bool needPopOut = false,
                          bool showNextWidgetDirectly = true,
                          int duration = 250,
                          QEasingCurve easingCurve = QEasingCurve::OutQuad);

signals:
    void aniFinished();
    void aniStart();

private slots:
    void onAnimationFinished();

private:
    void setAnimation(QPropertyAnimation *ani, const QPoint &startValue,
                      const QPoint &endValue, int duration,
                      QEasingCurve easingCurve);

    QList<PopUpAniInfo> m_animationInfos;   // 动画信息列表
    bool m_isAnimationEnabled;              // 是否启用动画
    int m_nextIndex;                        // 下一个索引
    QPropertyAnimation *m_currentAnimation; // 当前运行的动画
};


// ==================== TransitionStackedWidget ====================
/**
 * @brief 带过渡动画的堆叠控件基类
 */
class TransitionStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit TransitionStackedWidget(QWidget *parent = nullptr);
    ~TransitionStackedWidget() override = default;

    void setAnimationEnabled(bool isEnabled);
    bool isAnimationEnabled() const { return m_isAnimationEnabled; }

    void setCurrentWidget(QWidget *widget, int duration = -1, bool isBack = false);
    void setCurrentIndex(int index, int duration = -1, bool isBack = false);

signals:
    void aniFinished();
    void aniStart();

protected:
    /**
     * @brief 设置过渡动画 - 必须由子类实现
     * @param nextIndex 目标控件索引
     * @param duration 动画时长（毫秒），-1 表示使用默认值
     * @param isBack 是否为后退导航
     */
    virtual void setUpTransitionAnimation(int nextIndex, int duration, bool isBack) = 0;

    void stopAnimation();
    void hideSnapshots();
    QLabel* createSnapshotLabel();
    void renderSnapshot(QWidget *widget, QLabel *label);

    QParallelAnimationGroup *m_animationGroup;  // 并行动画组
    QLabel *m_currentSnapshot;                  // 当前页面快照
    QLabel *m_nextSnapshot;                     // 下一页面快照
    int m_nextIndex;                            // 下一个索引
    bool m_isAnimationEnabled;                  // 是否启用动画

private slots:
    void onAnimationFinished();
};


// ==================== EntranceTransitionStackedWidget ====================
/**
 * @brief 带入场过渡动画的堆叠控件
 */
class EntranceTransitionStackedWidget : public TransitionStackedWidget
{
    Q_OBJECT

public:
    explicit EntranceTransitionStackedWidget(QWidget *parent = nullptr);
    ~EntranceTransitionStackedWidget() override = default;

protected:
    void setUpTransitionAnimation(int nextIndex, int duration, bool isBack) override;
};


// ==================== DrillInTransitionStackedWidget ====================
/**
 * @brief 带钻入过渡动画的堆叠控件
 */
class DrillInTransitionStackedWidget : public TransitionStackedWidget
{
    Q_OBJECT

public:
    explicit DrillInTransitionStackedWidget(QWidget *parent = nullptr);
    ~DrillInTransitionStackedWidget() override = default;

protected:
    void setUpTransitionAnimation(int nextIndex, int duration, bool isBack) override;
};


// ==================== StackedWidget ====================
/**
 * @brief 动画类型枚举
 */
enum class AnimationType
{
    Opacity,            // 淡入淡出动画
    PopUp,              // 弹出动画
    EntranceTransition, // 入场过渡动画
    DrillInTransition   // 钻入过渡动画
};

/**
 * @brief 高级堆叠控件 - 支持多种动画类型
 *
 * 这是一个统一的高级封装类，支持以下动画类型：
 * 1. Opacity: 淡入淡出动画，适用于简单的页面切换
 * 2. PopUp: 弹出动画，适用于对话框、抽屉菜单等
 * 3. EntranceTransition: 入场过渡动画，适用于向导流程
 * 4. DrillInTransition: 钻入过渡动画，适用于层级导航
 */
class QFLUENT_EXPORT StackedWidget : public QFrame
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父控件
     * @param type 动画类型，默认为弹出动画
     */
    explicit StackedWidget(QWidget *parent = nullptr,
                          AnimationType type = AnimationType::PopUp);
    ~StackedWidget() override = default;

    /**
     * @brief 获取动画类型
     */
    AnimationType animationType() const { return m_animationType; }

    /**
     * @brief 设置动画类型（注意：会清空所有已添加的控件）
     * @param type 新的动画类型
     */
    void setAnimationType(AnimationType type);

    /**
     * @brief 是否启用动画
     */
    bool isAnimationEnabled() const;

    /**
     * @brief 设置是否启用动画
     */
    void setAnimationEnabled(bool isEnabled);

    /**
     * @brief 添加控件
     * @param widget 要添加的控件
     * @param deltaX X轴偏移量（仅对 PopUp 类型有效）
     * @param deltaY Y轴偏移量（仅对 PopUp 类型有效）
     */
    void addWidget(QWidget *widget, int deltaX = 0, int deltaY = 76);

    /**
     * @brief 移除控件
     */
    void removeWidget(QWidget *widget);

    /**
     * @brief 获取指定索引的控件
     */
    QWidget* widget(int index) const;

    /**
     * @brief 设置当前显示的控件
     * @param widget 要显示的控件
     * @param popOut 是否使用弹出动画（仅对 PopUp 类型有效）
     * @param duration 动画时长（毫秒），-1 表示使用默认值
     * @param isBack 是否为后退导航（仅对 Transition 类型有效）
     */
    void setCurrentWidget(QWidget *widget, bool popOut = true,
                         int duration = -1, bool isBack = false);

    /**
     * @brief 设置当前显示的索引
     */
    void setCurrentIndex(int index, bool popOut = true,
                        int duration = -1, bool isBack = false);

    /**
     * @brief 获取当前索引
     */
    int currentIndex() const;

    /**
     * @brief 获取当前控件
     */
    QWidget* currentWidget() const;

    /**
     * @brief 获取控件的索引
     */
    int indexOf(QWidget *widget) const;

    /**
     * @brief 获取控件数量
     */
    int count() const;

signals:
    /**
     * @brief 当前索引改变信号
     */
    void currentChanged(int index);

    /**
     * @brief 动画开始信号（仅部分类型支持）
     */
    void aniStart();

    /**
     * @brief 动画结束信号（仅部分类型支持）
     */
    void aniFinished();

private:
    /**
     * @brief 创建指定类型的堆叠控件
     */
    QStackedWidget* createStackedWidget(AnimationType type);

    /**
     * @brief 连接堆叠控件的信号
     */
    void connectSignals();

    /**
     * @brief 断开堆叠控件的信号
     */
    void disconnectSignals();

    /**
     * @brief 重置滚动条到顶部
     */
    void resetScrollBars(QWidget *widget);

    QHBoxLayout *m_hBoxLayout;              // 水平布局
    QStackedWidget *m_view;                 // 实际的堆叠控件
    AnimationType m_animationType;          // 动画类型
};
