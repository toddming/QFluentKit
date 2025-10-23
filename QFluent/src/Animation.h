#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>

#include "Property.h"

class QGraphicsDropShadowEffect;
class AnimationBase : public QObject {
    Q_OBJECT
public:
    explicit AnimationBase(QWidget *parent = nullptr);

protected:
    virtual void _onHover(QEnterEvent *e);
    virtual void _onLeave(QEvent *e);
    virtual void _onPress(QMouseEvent *e);
    virtual void _onRelease(QMouseEvent *e);

    bool eventFilter(QObject *obj, QEvent *e) override;
};

class QFLUENT_EXPORT TranslateYAnimation : public AnimationBase {
    Q_OBJECT
    Q_PROPERTY(float y READ y WRITE setY NOTIFY valueChanged)
public:
    explicit TranslateYAnimation(QWidget *parent, int offset = 2);

    float y() const;
    void setY(float y);

signals:
    void valueChanged(float);

protected:
    void _onPress(QMouseEvent *e) override;
    void _onRelease(QMouseEvent *e) override;

private:
    float _y = 0.0f;
    int maxOffset;
    QPropertyAnimation *ani;
};

class BackgroundColorObject;

class QFLUENT_EXPORT BackgroundAnimationWidget : public QWidget {
    Q_OBJECT
public:
    explicit BackgroundAnimationWidget(QWidget *parent = nullptr);

    QColor getBackgroundColor() const;
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const { return getBackgroundColor(); }

public:
    virtual QColor _normalBackgroundColor() const;
    virtual QColor _hoverBackgroundColor() const;
    virtual QColor _pressedBackgroundColor() const;
    virtual QColor _focusInBackgroundColor() const;
    virtual QColor _disabledBackgroundColor() const;

protected:
    void _updateBackgroundColor();

    bool eventFilter(QObject *obj, QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

    bool isHover() const;
    bool isPressed() const;
    void setHover(bool hover);
    void setPressed(bool pressed);

    BackgroundColorObject *bgColorObject;
    QPropertyAnimation *backgroundColorAni;

private:
    bool m_isHover = false;
    bool m_isPressed = false;

};

class BackgroundColorObject : public QObject {
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
public:
    explicit BackgroundColorObject(BackgroundAnimationWidget *parent);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

private:
    QColor _backgroundColor;
};

class DropShadowAnimation : public QPropertyAnimation {
    Q_OBJECT
public:
    explicit DropShadowAnimation(QWidget *parent, const QColor &normalColor = QColor(0, 0, 0, 0), const QColor &hoverColor = QColor(0, 0, 0, 75));

    void setBlurRadius(int radius);
    void setOffset(int dx, int dy);
    void setNormalColor(const QColor &color);
    void setHoverColor(const QColor &color);
    void setColor(const QColor &color);

protected:
    QGraphicsDropShadowEffect *_createShadowEffect();
    bool eventFilter(QObject *obj, QEvent *e) override;

private slots:
    void _onAniFinished();

private:
    QColor normalColor;
    QColor hoverColor;
    QPoint offset;
    int blurRadius = 38;
    bool isHover = false;
    QGraphicsDropShadowEffect *shadowEffect = nullptr;
};

enum class FluentAnimationSpeed {
    FAST = 0,
    MEDIUM = 1,
    SLOW = 2
};

enum class FluentAnimationType {
    FAST_INVOKE = 0,
    STRONG_INVOKE = 1,
    FAST_DISMISS = 2,
    SOFT_DISMISS = 3,
    POINT_TO_POINT = 4,
    FADE_IN_OUT = 5
};

enum class FluentAnimationProperty {
    POSITION,
    SCALE,
    ANGLE,
    OPACITY
};

class FluentAnimationProperObject : public QObject {
    Q_OBJECT
public:
    explicit FluentAnimationProperObject(QObject *parent = nullptr);

    virtual QVariant getValue() const = 0;
    virtual void setValue(const QVariant &value) = 0;

    static void registerObject(FluentAnimationProperty name, std::function<FluentAnimationProperObject*(QObject*)> creator);
    static FluentAnimationProperObject *create(FluentAnimationProperty propertyType, QObject *parent = nullptr);

private:
    static QMap<FluentAnimationProperty, std::function<FluentAnimationProperObject*(QObject*)>> objects;
};

class PositionObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant position READ getValue WRITE setValue)  // 改为 QVariant 以匹配函数签名
public:
    explicit PositionObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &pos) override;

private:
    QPoint _position;
};

class ScaleObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant scale READ getValue WRITE setValue)  // 改为 QVariant
public:
    explicit ScaleObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &scale) override;

private:
    float _scale = 1.0f;
};

class AngleObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant angle READ getValue WRITE setValue)  // 改为 QVariant
public:
    explicit AngleObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &angle) override;

private:
    float _angle = 0.0f;
};

class OpacityObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant opacity READ getValue WRITE setValue)  // 改为 QVariant
public:
    explicit OpacityObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &opacity) override;

private:
    float _opacity = 0.0f;
};

class QFLUENT_EXPORT FluentAnimation : public QPropertyAnimation {
    Q_OBJECT
public:
    explicit FluentAnimation(QObject *parent = nullptr);

    static QEasingCurve createBezierCurve(float x1, float y1, float x2, float y2);
    static QEasingCurve curve();

    void setSpeed(FluentAnimationSpeed speed);
    virtual int speedToDuration(FluentAnimationSpeed speed);

    void startAnimation(const QVariant &endValue, const QVariant &startValue = QVariant());
    QVariant value() const;
    void setValue(const QVariant &value);

    static void registerAnimation(FluentAnimationType name, std::function<FluentAnimation*(QObject*)> creator);
    static FluentAnimation *create(FluentAnimationType aniType, FluentAnimationProperty propertyType,
                                   FluentAnimationSpeed speed = FluentAnimationSpeed::FAST, const QVariant &value = QVariant(), QObject *parent = nullptr);

private:
    static QMap<FluentAnimationType, std::function<FluentAnimation*(QObject*)>> animations;
};

class QFLUENT_EXPORT FastInvokeAnimation : public FluentAnimation {
    Q_OBJECT
public:
    explicit FastInvokeAnimation(QObject *parent = nullptr);

    static QEasingCurve curve();
    int speedToDuration(FluentAnimationSpeed speed) override;
};

class QFLUENT_EXPORT StrongInvokeAnimation : public FluentAnimation {
    Q_OBJECT
public:
    explicit StrongInvokeAnimation(QObject *parent = nullptr);

    static QEasingCurve curve();
    int speedToDuration(FluentAnimationSpeed speed) override;
};

class QFLUENT_EXPORT FastDismissAnimation : public FastInvokeAnimation {
    Q_OBJECT
public:
    explicit FastDismissAnimation(QObject *parent = nullptr);
};

class QFLUENT_EXPORT SoftDismissAnimation : public FluentAnimation {
    Q_OBJECT
public:
    explicit SoftDismissAnimation(QObject *parent = nullptr);

    static QEasingCurve curve();
    int speedToDuration(FluentAnimationSpeed speed) override;
};

class QFLUENT_EXPORT PointToPointAnimation : public FastDismissAnimation {
    Q_OBJECT
public:
    explicit PointToPointAnimation(QObject *parent = nullptr);

    static QEasingCurve curve();
};

class QFLUENT_EXPORT FadeInOutAnimation : public FluentAnimation {
    Q_OBJECT
public:
    explicit FadeInOutAnimation(QObject *parent = nullptr);

    int speedToDuration(FluentAnimationSpeed speed) override;
};

#endif // ANIMATION_H
