#pragma once

#include <QWidget>
#include <QToolButton>
#include <QEasingCurve>
#include <QColor>
#include <memory>

#include "FluentGlobal.h"

// 前置声明
class QScrollBar;
class QPaintEvent;
class QPropertyAnimation;
class QAbstractScrollArea;
class FluentIconBase;

/**
 * @brief 箭头按钮类，用于滚动条的上/下或左/右按钮
 */
class ArrowButton : public QToolButton {
    Q_OBJECT
public:
    explicit ArrowButton(const FluentIconBase &icon, QWidget* parent = nullptr);
    ~ArrowButton() override = default;

    void setOpacity(qreal opacity);
    void setLightColor(const QColor& color);
    void setDarkColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor m_lightColor{0, 0, 0, 114};
    QColor m_darkColor{255, 255, 255, 139};
    qreal m_opacity{1.0};
    std::unique_ptr<FluentIconBase> m_fluentIcon;
};

/**
 * @brief 滚动条槽类，包含背景和箭头按钮
 */
class ScrollBarGroove : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit ScrollBarGroove(Qt::Orientation orientation, QWidget* parent = nullptr);
    ~ScrollBarGroove() override = default;

    void setLightBackgroundColor(const QColor& color);
    void setDarkBackgroundColor(const QColor& color);
    void fadeIn();
    void fadeOut();

    void setOpacity(qreal opacity);
    qreal opacity() const;

    ArrowButton* upButton() const;
    ArrowButton* downButton() const;
    QPropertyAnimation* opacityAnimation() const;

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
    void opacityChanged(qreal opacity);

private:
    qreal m_opacity{0.0};
    QColor m_lightBackgroundColor{252, 252, 252, 217};
    QColor m_darkBackgroundColor{44, 44, 44, 245};

    ArrowButton* m_upButton{nullptr};
    ArrowButton* m_downButton{nullptr};
    QPropertyAnimation* m_opacityAnimation{nullptr};
};

/**
 * @brief 滚动条手柄类，用户可拖动的滑块
 */
class ScrollBarHandle : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit ScrollBarHandle(Qt::Orientation orientation, QWidget* parent = nullptr);
    ~ScrollBarHandle() override = default;

    void setLightColor(const QColor& color);
    void setDarkColor(const QColor& color);
    void fadeIn();
    void fadeOut();

    void setOpacity(qreal opacity);
    qreal opacity() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    qreal m_opacity{1.0};
    QPropertyAnimation* m_opacityAnimation{nullptr};
    QColor m_lightColor{0, 0, 0, 114};
    QColor m_darkColor{255, 255, 255, 139};
    Qt::Orientation m_orientation;
};

/**
 * @brief 自定义滚动条类
 */
class ScrollBar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit ScrollBar(Qt::Orientation orientation, QAbstractScrollArea* parent = nullptr);
    ~ScrollBar() override = default;

    // Getters
    int value() const;
    int minimum() const;
    int maximum() const;
    Qt::Orientation orientation() const;
    int pageStep() const;
    int singleStep() const;
    bool isSliderDown() const;

    // Setters
    void setValue(int value);
    void setMinimum(int min);
    void setMaximum(int max);
    void setRange(int min, int max);
    void setPageStep(int step);
    void setSingleStep(int step);
    void setSliderDown(bool isDown);
    void setHandleColor(const QColor& light, const QColor& dark);
    void setArrowColor(const QColor& light, const QColor& dark);
    void setGrooveColor(const QColor& light, const QColor& dark);
    void setHandleDisplayMode(Fluent::ScrollBarHandleDisplayMode mode);
    void setForceHidden(bool isHidden);

    // 展开/收起
    void expand();
    void collapse();

signals:
    void rangeChanged(int min, int max);
    void valueChanged(int value);
    void sliderPressed();
    void sliderReleased();
    void sliderMoved();

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void onPageUp();
    void onPageDown();
    void onValueChanged(int value);
    void onOpacityAnimationValueChanged(const QVariant& value);

private:
    void initWidget(QAbstractScrollArea* parent);
    void adjustPosition(const QSize& size);
    void adjustHandleSize();
    void adjustHandlePosition();

    int grooveLength() const;
    int slideLength() const;
    bool isSlideRegion(const QPoint& pos) const;

    ScrollBarGroove* m_groove{nullptr};
    ScrollBarHandle* m_handle{nullptr};
    QScrollBar* m_partnerScrollBar{nullptr};

    Qt::Orientation m_orientation;
    int m_singleStep{1};
    int m_pageStep{50};
    int m_padding{14};
    int m_minimum{0};
    int m_maximum{0};
    int m_value{0};

    bool m_isPressed{false};
    bool m_isEnter{false};
    bool m_isExpanded{false};
    bool m_isForceHidden{false};

    QPoint m_pressedPos;
    Fluent::ScrollBarHandleDisplayMode m_handleDisplayMode{Fluent::ScrollBarHandleDisplayMode::ALWAYS};
};

/**
 * @brief 平滑滚动条类，支持动画效果
 */
class SmoothScrollBar : public ScrollBar {
    Q_OBJECT

public:
    explicit SmoothScrollBar(Qt::Orientation orientation, QAbstractScrollArea* parent = nullptr);
    ~SmoothScrollBar() override = default;

    void setValue(int value, bool useAnimation = true);
    void scrollValue(int value, bool useAnimation = true);
    void scrollTo(int value, bool useAnimation = true);
    void resetValue(int value);
    void setScrollAnimation(int duration, QEasingCurve::Type easing = QEasingCurve::OutCubic);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    int m_animationDuration{500};
    int m_internalValue{0};
    QPropertyAnimation* m_animation{nullptr};
};

/**
 * @brief 平滑滚动代理类，管理滚动区域的平滑滚动行为
 */
class SmoothScrollDelegate : public QObject {
    Q_OBJECT

public:
    explicit SmoothScrollDelegate(QAbstractScrollArea* parent, bool useAnimation = false);
    ~SmoothScrollDelegate() override = default;

    bool eventFilter(QObject* obj, QEvent* event) override;

    // Getters
    class SmoothScroll* verticalSmoothScroll() const;
    class SmoothScroll* horizontalSmoothScroll() const;
    SmoothScrollBar* verticalScrollBar() const;
    SmoothScrollBar* horizontalScrollBar() const;

    // 滚动条策略设置
    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);

private:
    bool m_useAnimation;
    SmoothScrollBar* m_verticalScrollBar{nullptr};
    SmoothScrollBar* m_horizontalScrollBar{nullptr};
    SmoothScroll* m_verticalSmoothScroll{nullptr};
    SmoothScroll* m_horizontalSmoothScroll{nullptr};
};

