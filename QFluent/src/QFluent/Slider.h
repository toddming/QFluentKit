#pragma once
#include <QWidget>
#include <QSlider>
#include <QColor>
#include <QPoint>
#include <QProxyStyle>
#include <QVariant>

#include "FluentGlobal.h"

class QPropertyAnimation;
class QPainter;
class QMouseEvent;

/**
 * @brief 滑块手柄组件
 */
class SliderHandle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    explicit SliderHandle(QSlider* parent);

    qreal radius() const { return m_radius; }
    void setRadius(qreal r);

    void setHandleColor(const QColor& light, const QColor& dark);

signals:
    void pressed();
    void released();

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *e) override;
#else
    void enterEvent(QEvent *e) override;
#endif
    void leaveEvent(QEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    void startAni(qreal radius);

    qreal m_radius;
    QColor m_lightHandleColor;
    QColor m_darkHandleColor;
    QPropertyAnimation* m_radiusAni;
};


/**
 * @brief 可点击的滑块组件
 */
class QFLUENT_EXPORT Slider : public QSlider
{
    Q_OBJECT

public:
    explicit Slider(QWidget* parent = nullptr);
    explicit Slider(Qt::Orientation orientation, QWidget* parent = nullptr);

    void setThemeColor(const QColor& light, const QColor& dark);
    void setOrientation(Qt::Orientation orientation);

    int grooveLength() const;

signals:
    void clicked(int value);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    void postInit();
    void adjustHandlePos();
    int posToValue(const QPoint& pos) const;

    void drawHorizonTick(QPainter* painter);
    void drawVerticalTick(QPainter* painter);
    void drawHorizonGroove(QPainter* painter);
    void drawVerticalGroove(QPainter* painter);

    SliderHandle* m_handle;
    QPoint m_pressedPos;
    QColor m_lightGrooveColor;
    QColor m_darkGrooveColor;
};


/**
 * @brief 简单的可点击滑块
 */
class QFLUENT_EXPORT ClickableSlider : public QSlider
{
    Q_OBJECT

public:
    explicit ClickableSlider(QWidget* parent = nullptr);
    explicit ClickableSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

signals:
    void clicked(int value);

protected:
    void mousePressEvent(QMouseEvent* e) override;
};


/**
 * @brief 空心手柄样式
 */
class HollowHandleStyle : public QProxyStyle
{
    Q_OBJECT

public:
    explicit HollowHandleStyle(const QMap<QString, QVariant>& config = QMap<QString, QVariant>());

    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex* opt,
                        SubControl sc, const QWidget* widget = nullptr) const override;

    void drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt,
                           QPainter* painter, const QWidget* widget = nullptr) const override;

private:
    QMap<QString, QVariant> m_config;
};
