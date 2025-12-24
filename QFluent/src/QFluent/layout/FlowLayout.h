#pragma once

#include <QLayout>
#include <QEasingCurve>
#include "FluentGlobal.h"

QT_BEGIN_NAMESPACE

// 前置声明，减少头文件依赖
class QTimer;
class QPropertyAnimation;
class QParallelAnimationGroup;
class QWidgetItem;

/**
 * @brief 流式布局类，支持自动换行和可选的动画效果
 *
 * FlowLayout 会自动将子控件从左到右排列，当一行空间不足时自动换行。
 * 支持可选的位置变化动画效果。
 */
class QFLUENT_EXPORT FlowLayout : public QLayout
{
    Q_OBJECT
    Q_DISABLE_COPY(FlowLayout)

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件
     * @param enableAnimation 是否启用位置变化动画
     * @param tightMode 紧凑模式，是否隐藏不可见的子控件
     */
    explicit FlowLayout(QWidget *parent = nullptr,
                       bool enableAnimation = false,
                       bool tightMode = false);

    /**
     * @brief 析构函数，清理所有资源
     */
    ~FlowLayout() override;

    // QLayout 接口实现
    void addItem(QLayoutItem *item) override;
    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    QSize sizeHint() const override;
    QSize minimumSize() const override;

    // 扩展接口
    /**
     * @brief 在指定位置插入布局项
     */
    void insertItem(int index, QLayoutItem *item);

    /**
     * @brief 添加子控件
     */
    void addWidget(QWidget *widget);

    /**
     * @brief 在指定位置插入子控件
     */
    void insertWidget(int index, QWidget *widget);

    /**
     * @brief 移除指定的子控件
     */
    void removeWidget(QWidget *widget);

    /**
     * @brief 移除所有子控件（但不删除控件对象）
     */
    void removeAllWidgets();

    /**
     * @brief 移除并删除所有子控件
     */
    void takeAllWidgets();

    // 动画设置
    /**
     * @brief 设置动画参数
     * @param duration 动画持续时间（毫秒）
     * @param easing 缓动曲线类型
     */
    void setAnimation(int duration, QEasingCurve::Type easing = QEasingCurve::Linear);

    // 间距设置
    void setVerticalSpacing(int spacing);
    int verticalSpacing() const;
    void setHorizontalSpacing(int spacing);
    int horizontalSpacing() const;

protected:
    /**
     * @brief 事件过滤器，处理父窗口变化和显示事件
     */
    bool eventFilter(QObject *object, QEvent *event) override;

    /**
     * @brief 设置布局几何区域
     */
    void setGeometry(const QRect &rect) override;

private:
    // 布局项和动画
    QVector<QLayoutItem *> m_items;
    QVector<QPropertyAnimation *> m_animations;
    QParallelAnimationGroup *m_animationGroup;

    // 布局参数
    int m_verticalSpacing;
    int m_horizontalSpacing;
    int m_animationDuration;
    QEasingCurve::Type m_easingCurve;

    // 状态标志
    bool m_animationEnabled;
    bool m_tightMode;
    bool m_eventFilterInstalled;

    // 辅助对象
    QTimer *m_layoutDebounceTimer;
    QWidget *m_parentWidget;

    // 内部方法
    /**
     * @brief 当新控件添加时的处理
     * @param widget 新添加的控件
     * @param index 插入位置，-1 表示追加
     */
    void onWidgetAdded(QWidget *widget, int index = -1);

    /**
     * @brief 计算给定宽度下布局所需的高度（const 方法）
     */
    int calculateHeight(const QRect &rect) const;

    /**
     * @brief 执行实际的布局操作
     * @param rect 布局区域
     * @param applyGeometry 是否应用几何变化
     * @return 计算得到的布局高度
     */
    int doLayout(const QRect &rect, bool applyGeometry);

    /**
     * @brief 清理控件关联的动画
     */
    void cleanupAnimation(QWidget *widget);
};

QT_END_NAMESPACE
