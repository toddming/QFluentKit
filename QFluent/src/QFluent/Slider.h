#pragma once

#include <QSlider>
#include <QWidget>
#include <QProxyStyle>
#include <QColor>
#include <QMap>

#include "FluentGlobal.h"

class QPropertyAnimation;
class QPainter;
class QMouseEvent;
class QResizeEvent;
class QPaintEvent;
class QEnterEvent;

/**
 * @brief 滑块手柄类
 *
 * 提供带动画效果的自定义滑块手柄
 */
class SliderHandle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    explicit SliderHandle(QSlider *parent);
    ~SliderHandle() override;

    /**
     * @brief 获取当前半径
     */
    qreal radius() const;

    /**
     * @brief 设置手柄半径
     */
    void setRadius(qreal r);

    /**
     * @brief 设置手柄颜色
     * @param light 浅色主题颜色
     * @param dark 深色主题颜色
     */
    void setHandleColor(const QColor &light, const QColor &dark);

signals:
    void pressed();
    void released();

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void startAnimation(qreal targetRadius);

    qreal m_radius;
    QColor m_lightHandleColor;
    QColor m_darkHandleColor;
    QPropertyAnimation *m_radiusAnimation;
};

/**
 * @brief 可点击的滑块类
 *
 * 支持点击定位和自定义样式的滑块控件
 */
class QFLUENT_EXPORT Slider : public QSlider
{
    Q_OBJECT

public:
    explicit Slider(QWidget *parent = nullptr);
    explicit Slider(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~Slider() override;

    /**
     * @brief 设置主题颜色
     * @param light 浅色主题颜色
     * @param dark 深色主题颜色
     */
    void setThemeColor(const QColor &light, const QColor &dark);

    /**
     * @brief 设置滑块方向
     */
    void setOrientation(Qt::Orientation orientation);

    /**
     * @brief 获取滑槽长度
     */
    int grooveLength() const;

signals:
    void clicked(int value);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void init();
    void adjustHandlePosition();
    int posToValue(const QPoint &pos) const;
    void drawHorizontalGroove(QPainter *painter);
    void drawVerticalGroove(QPainter *painter);
    void drawHorizontalTick(QPainter *painter);
    void drawVerticalTick(QPainter *painter);

    SliderHandle *m_handle;
    QPoint m_pressedPos;
    QColor m_lightGrooveColor;
    QColor m_darkGrooveColor;
};

/**
 * @brief 简单的可点击滑块类
 *
 * 提供基础的点击定位功能
 */
class QFLUENT_EXPORT ClickableSlider : public QSlider
{
    Q_OBJECT

public:
    explicit ClickableSlider(QWidget *parent = nullptr);
    explicit ClickableSlider(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~ClickableSlider() override = default;

signals:
    void clicked(int value);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

/**
 * @brief 空心手柄样式类
 *
 * 提供自定义的空心圆环手柄样式
 */
class HollowHandleStyle : public QProxyStyle
{
    Q_OBJECT

public:
    explicit HollowHandleStyle(const QMap<QString, QVariant> &config = QMap<QString, QVariant>());
    ~HollowHandleStyle() override = default;

    QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                        SubControl subControl, const QWidget *widget = nullptr) const override;

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                           QPainter *painter, const QWidget *widget = nullptr) const override;

private:
    void initConfig(const QMap<QString, QVariant> &customConfig);

    QMap<QString, QVariant> m_config;
};


