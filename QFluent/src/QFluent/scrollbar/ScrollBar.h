#ifndef SCROLL_BAR_H
#define SCROLL_BAR_H

#include <QWidget>
#include <QToolButton>
#include <QEasingCurve>

#include "FluentIcon.h"
#include "SmoothScroll.h"

class QScrollBar;
class QPaintEvent;
class ArrowButton : public QToolButton {
    Q_OBJECT
public:
    ArrowButton(const FluentIconBase &icon, QWidget* parent = nullptr);
    void setOpacity(qreal opacity);
    void setLightColor(const QColor& color);
    void setDarkColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QColor lightColor = QColor(0, 0, 0, 114);
    QColor darkColor = QColor(255, 255, 255, 139);
    qreal m_opacity = 1.0;
    std::unique_ptr<FluentIconBase> m_fluentIcon;
};

class QPropertyAnimation;
class ScrollBarGroove : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)
public:
    ScrollBarGroove(Qt::Orientation orient, QWidget* parent = nullptr);
    void setLightBackgroundColor(const QColor& color);
    void setDarkBackgroundColor(const QColor& color);
    void fadeIn();
    void fadeOut();
    void setOpacity(qreal opacity);
    qreal getOpacity() const;
    ArrowButton* getUpButton();
    ArrowButton* getDownButton();
    QPropertyAnimation* getOpacityAni();

protected:
    void paintEvent(QPaintEvent* e) override;

signals:
    void opacityChanged(qreal);  // 如果需要外部监听

public slots:
    void onOpacityAniValueChanged(const QVariant& value);

private:
    qreal m_opacity = 1.0;
    QColor lightBackgroundColor = QColor(252, 252, 252, 217);
    QColor darkBackgroundColor = QColor(44, 44, 44, 245);
    ArrowButton* upButton;
    ArrowButton* downButton;
    QPropertyAnimation* opacityAni;
};

class ScrollBarHandle : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)
public:
    ScrollBarHandle(Qt::Orientation orient, QWidget* parent = nullptr);
    void setLightColor(const QColor& color);
    void setDarkColor(const QColor& color);
    void fadeIn();
    void fadeOut();
    void setOpacity(qreal opacity);
    qreal getOpacity() const;

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    qreal m_opacity = 1.0;
    QPropertyAnimation* opacityAni;
    QColor lightColor = QColor(0, 0, 0, 114);
    QColor darkColor = QColor(255, 255, 255, 139);
    Qt::Orientation m_orient;
};

class QFLUENT_EXPORT ScrollBar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int val READ value WRITE setVal NOTIFY valueChanged)
public:
    ScrollBar(Qt::Orientation orient, QAbstractScrollArea* parent = nullptr);
    int value() const;
    int minimum() const;
    int maximum() const;
    Qt::Orientation orientation() const;
    int pageStep() const;
    int singleStep() const;
    bool isSliderDown() const;
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
    void expand();
    void collapse();
    void setForceHidden(bool isHidden);

signals:
    void rangeChanged(const QPair<int, int>&);
    void valueChanged(int);
    void sliderPressed();
    void sliderReleased();
    void sliderMoved();

protected:
    void enterEvent(QEnterEvent* e) override;
    void leaveEvent(QEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

public slots:
    void setVal(int value);

public slots:
    void onPageUp();
    void onPageDown();
    void onValueChanged(int value);
    void onOpacityAniValueChanged(const QVariant& value);

private:
    void initWidget(QAbstractScrollArea* parent);
    void adjustPos(const QSize& size);
    void adjustHandleSize();
    void adjustHandlePos();
    int grooveLength() const;
    int slideLength() const;
    bool isSlideRegion(const QPoint& pos) const;

    ScrollBarGroove* groove;
    ScrollBarHandle* handle;
    Qt::Orientation m_orientation;
    int m_singleStep = 1;
    int m_pageStep = 50;
    int m_padding = 14;
    int m_minimum = 0;
    int m_maximum = 0;
    int m_value = 0;
    bool m_isPressed = false;
    bool m_isEnter = false;
    bool m_isExpanded = false;
    QPoint m_pressedPos;
    bool m_isForceHidden = false;
    Fluent::ScrollBarHandleDisplayMode handleDisplayMode = Fluent::ScrollBarHandleDisplayMode::ALWAYS;
    QScrollBar* partnerBar;
};

class QFLUENT_EXPORT SmoothScrollBar : public ScrollBar {
    Q_OBJECT
public:
    SmoothScrollBar(Qt::Orientation orient, QAbstractScrollArea* parent = nullptr);
    void setValue(int value, bool useAni = true);
    void scrollValue(int value, bool useAni = true);
    void scrollTo(int value, bool useAni = true);
    void resetValue(int value);
    void setScrollAnimation(int duration, QEasingCurve::Type easing = QEasingCurve::OutCubic);

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

private:
    int duration = 500;
    QPropertyAnimation* ani;
    int m_valueInternal = 0;
};

class QFLUENT_EXPORT SmoothScrollDelegate : public QObject {
    Q_OBJECT
public:
    SmoothScrollDelegate(QAbstractScrollArea* parent, bool useAni = false);
    bool eventFilter(QObject* obj, QEvent* e) override;

    SmoothScroll* getVScroll();
    SmoothScroll* getHScroll();
    SmoothScrollBar* getVScrollBar();
    SmoothScrollBar* getHScrollBar();


    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);

private:
    bool useAni;
    SmoothScrollBar* vScrollBar;
    SmoothScrollBar* hScrollBar;
    SmoothScroll* verticalSmoothScroll;
    SmoothScroll* horizonSmoothScroll;
};

#endif // SCROLL_BAR_H
