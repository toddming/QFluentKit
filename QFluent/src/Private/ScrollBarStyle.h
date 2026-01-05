#pragma once

#include <QProxyStyle>

// 前置声明，避免不必要的头文件包含
class ScrollBar;
class QStyleOptionSlider;
class QPropertyAnimation;

/**
 * @brief 自定义滚动条样式类
 *
 * 提供带有展开/收起动画效果的滚动条样式
 * 支持深色/浅色主题切换
 */
class ScrollBarStyle : public QProxyStyle
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)
    Q_PROPERTY(qreal sliderExtent READ getSliderExtent WRITE setSliderExtent)

public:
    explicit ScrollBarStyle(QStyle* style = nullptr);
    ~ScrollBarStyle() override;

    // 禁用拷贝和移动
    ScrollBarStyle(const ScrollBarStyle&) = delete;
    ScrollBarStyle& operator=(const ScrollBarStyle&) = delete;
    ScrollBarStyle(ScrollBarStyle&&) = delete;
    ScrollBarStyle& operator=(ScrollBarStyle&&) = delete;

    // Getter/Setter方法（遵循Qt命名规范）
    qreal getOpacity() const { return m_opacity; }
    void setOpacity(qreal value);

    qreal getSliderExtent() const { return m_sliderExtent; }
    void setSliderExtent(qreal value);

    ScrollBar* getScrollBar() const { return m_scrollBar; }
    void setScrollBar(ScrollBar* scrollBar);

    // 重写的Qt样式方法
    void drawComplexControl(
        ComplexControl control,
        const QStyleOptionComplex* option,
        QPainter* painter,
        const QWidget* widget = nullptr
    ) const override;

    int pixelMetric(
        PixelMetric metric,
        const QStyleOption* option = nullptr,
        const QWidget* widget = nullptr
    ) const override;

    int styleHint(
        StyleHint hint,
        const QStyleOption* option = nullptr,
        const QWidget* widget = nullptr,
        QStyleHintReturn* returnData = nullptr
    ) const override;

    /**
     * @brief 启动展开/收起动画
     * @param isExpand true为展开，false为收起
     */
    void startExpandAnimation(bool isExpand);

private:
    /**
     * @brief 绘制水平方向的指示器（左右三角）
     */
    void drawHorizontalIndicators(
        QPainter* painter,
        const QStyleOptionSlider* option,
        ComplexControl control,
        const QWidget* widget
    ) const;

    /**
     * @brief 绘制垂直方向的指示器（上下三角）
     */
    void drawVerticalIndicators(
        QPainter* painter,
        const QStyleOptionSlider* option,
        ComplexControl control,
        const QWidget* widget
    ) const;

    /**
     * @brief 绘制滑块
     */
    void drawSlider(
        QPainter* painter,
        const QStyleOptionSlider* option,
        ComplexControl control,
        const QWidget* widget
    ) const;

    /**
     * @brief 创建并配置动画对象
     */
    QPropertyAnimation* createAnimation(
        const QByteArray& propertyName,
        qreal startValue,
        qreal endValue,
        int duration = 250
    );

private:
    // 成员变量（使用m_前缀遵循Qt命名规范）
    bool m_isExpanded;                          // 是否处于展开状态
    qreal m_opacity;                            // 背景透明度
    qreal m_sliderExtent;                       // 滑块宽度/高度
    ScrollBar* m_scrollBar;                     // 关联的滚动条指针（弱引用，不负责释放）

    // 常量配置
    static constexpr qreal SLIDER_MARGIN = 2.5;         // 滑块边距
    static constexpr int SCROLLBAR_EXTENT = 10;         // 滚动条宽度
    static constexpr qreal COLLAPSED_SLIDER_EXTENT = 2.5; // 收起状态滑块宽度
    static constexpr int ANIMATION_DURATION = 240;      // 动画时长（毫秒）
    static constexpr int INDICATOR_SIZE = 8;            // 指示器大小
    static constexpr qreal INDICATOR_ANGLE = 15.0;      // 指示器角度
    static constexpr qreal BACKGROUND_RADIUS = 6.0;     // 背景圆角半径
};
