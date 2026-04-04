#pragma once

#include "FlowLayout.h"
#include "FluentGlobal.h"

QT_BEGIN_NAMESPACE

/**
 * @brief 自适应流式布局类，控件宽度自动填充整行
 *
 * AdaptiveFlowLayout 继承自 FlowLayout，会自动计算每行可容纳的控件数量，
 * 并调整控件宽度使其均匀填满整行。
 */
class QFLUENT_EXPORT AdaptiveFlowLayout : public FlowLayout
{
    Q_OBJECT
    Q_DISABLE_COPY(AdaptiveFlowLayout)

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件
     * @param enableAnimation 是否启用位置变化动画
     * @param tightMode 紧凑模式，是否隐藏不可见的子控件
     */
    explicit AdaptiveFlowLayout(QWidget *parent = nullptr,
                                bool enableAnimation = false,
                                bool tightMode = false);

    /**
     * @brief 设置控件最小宽度
     * @param width 最小宽度值
     */
    void setWidgetMinimumWidth(int width);

    /**
     * @brief 获取控件最小宽度
     * @return 最小宽度值
     */
    int widgetMinimumWidth() const;

    /**
     * @brief 设置控件最大宽度
     * @param width 最大宽度值，-1 表示不限制
     */
    void setWidgetMaximumWidth(int width);

    /**
     * @brief 获取控件最大宽度
     * @return 最大宽度值，-1 表示不限制
     */
    int widgetMaximumWidth() const;

protected:
    /**
     * @brief 计算给定宽度下布局所需的高度（重写父类方法）
     * @param rect 布局区域
     * @return 计算得到的高度
     */
    int calculateHeight(const QRect &rect) const override;

    /**
     * @brief 执行实际的布局操作（重写父类方法）
     * @param rect 布局区域
     * @param applyGeometry 是否应用几何变化
     * @return 计算得到的布局高度
     */
    int doLayout(const QRect &rect, bool applyGeometry) override;

private:
    int m_widgetMinimumWidth;   // 控件最小宽度
    int m_widgetMaximumWidth;   // 控件最大宽度，-1 表示不限制
};

QT_END_NAMESPACE
