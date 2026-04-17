#pragma once

#include <QColor>

#include "FluentGlobal.h"

class QPainter;
class QPainterPath;
class AcrylicBrush;

/**
 * @brief 亚克力效果窗口部件基类
 *
 * 提供亚克力毛玻璃效果的基础实现，包括：
 * - 自动管理 AcrylicBrush 实例
 * - 根据主题自动更新亚克力颜色
 * - 支持自定义裁剪路径
 * - 处理亚克力效果的绘制
 */
class QFLUENT_EXPORT AcrylicWidget
{
public:
    /**
     * @brief 构造函数
     * @param blurRadius 模糊半径，默认为 30
     */
    explicit AcrylicWidget(int blurRadius = 30);

    virtual ~AcrylicWidget();

    /**
     * @brief 获取亚克力画刷实例
     * @return 亚克力画刷指针
     */
    AcrylicBrush* acrylicBrush() const;

    /**
     * @brief 设置模糊半径
     * @param radius 新的模糊半径值
     */
    void setBlurRadius(int radius);

    /**
     * @brief 获取当前模糊半径
     * @return 当前模糊半径值
     */
    int blurRadius() const;

protected:
    /**
     * @brief 初始化亚克力画刷
     *
     * 为指定的设备创建亚克力画刷实例
     * @param device 绘制设备（通常是 this）
     * @param tintColor 色调颜色，默认为浅灰色半透明
     * @param luminosityColor 亮度颜色，默认为白色微透明
     * @param noiseOpacity 噪声不透明度，默认为 0.02
     */
    void initializeAcrylicBrush(QWidget *device,
                               const QColor &tintColor = QColor(242, 242, 242, 150),
                               const QColor &luminosityColor = QColor(255, 255, 255, 10),
                               double noiseOpacity = 0.02);

    /**
     * @brief 更新亚克力颜色
     *
     * 根据当前主题（深色/浅色）自动设置合适的色调颜色和亮度颜色
     * - 深色主题：深灰色调，无亮度层
     * - 浅色主题：白色调，无亮度层
     */
    void updateAcrylicColor();

    /**
     * @brief 获取亚克力裁剪路径
     *
     * 子类可以重写此方法以提供自定义的裁剪路径
     * @return 裁剪路径，默认返回空路径（不裁剪）
     */
    virtual QPainterPath acrylicClipPath() const;

    /**
     * @brief 绘制亚克力效果
     *
     * 在子类的 paintEvent 中调用此方法以绘制亚克力背景
     * @param painter 绘图器指针
     */
    void drawAcrylic(QPainter *painter);

private:
    AcrylicBrush *m_acrylicBrush;  ///< 亚克力画刷实例
    int m_blurRadius;               ///< 模糊半径
    QColor m_cachedTintColor;       ///< 缓存的色调颜色，避免重复设置触发重绘
    QColor m_cachedLuminosityColor; ///< 缓存的亮度颜色

    // 禁用拷贝构造和赋值运算符
    AcrylicWidget(const AcrylicWidget &) = delete;
    AcrylicWidget& operator=(const AcrylicWidget &) = delete;
};
