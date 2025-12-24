#pragma once

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QVariant>
#include <QParallelAnimationGroup>

#include "FluentGlobal.h"

class AnimationBasePrivate;
class TranslateYAnimationPrivate;
class BackgroundAnimationWidgetPrivate;
class BackgroundColorObjectPrivate;
class DropShadowAnimationPrivate;
class FluentAnimationProperObjectPrivate;
class FluentAnimationPrivate;
class ScaleSlideAnimationPrivate;

// 枚举定义
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

// AnimationBase
class QFLUENT_EXPORT AnimationBase : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(AnimationBase)
public:
    explicit AnimationBase(QWidget *parent = nullptr);
    virtual ~AnimationBase();

protected:
    AnimationBase(AnimationBasePrivate &dd, QWidget *parent);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual void _onHover(QEnterEvent *e);
#else
    virtual void _onHover(QEvent *e);
#endif
    virtual void _onLeave(QEvent *e);
    virtual void _onPress(QMouseEvent *e);
    virtual void _onRelease(QMouseEvent *e);

    bool eventFilter(QObject *obj, QEvent *e) override;

    QScopedPointer<AnimationBasePrivate> d_ptr;
};

// TranslateYAnimation
class QFLUENT_EXPORT TranslateYAnimation : public AnimationBase {
    Q_OBJECT
    Q_DECLARE_PRIVATE(TranslateYAnimation)
    Q_PROPERTY(float y READ y WRITE setY NOTIFY valueChanged)
public:
    explicit TranslateYAnimation(QWidget *parent, int offset = 2);
    ~TranslateYAnimation() override;

    float y() const;
    void setY(float y);

signals:
    void valueChanged(float);

protected:
    void _onPress(QMouseEvent *e) override;
    void _onRelease(QMouseEvent *e) override;
};

// BackgroundColorObject
class QFLUENT_EXPORT BackgroundColorObject : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(BackgroundColorObject)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
public:
    explicit BackgroundColorObject(QWidget *parent);
    ~BackgroundColorObject();

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

private:
    QScopedPointer<BackgroundColorObjectPrivate> d_ptr;
};

// BackgroundAnimationWidget
class QFLUENT_EXPORT BackgroundAnimationWidget : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(BackgroundAnimationWidget)
public:
    explicit BackgroundAnimationWidget(QWidget *parent = nullptr);
    ~BackgroundAnimationWidget() override;

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    virtual QColor normalBackgroundColor() const;
    virtual QColor hoverBackgroundColor() const;
    virtual QColor pressedBackgroundColor() const;
    virtual QColor focusInBackgroundColor() const;
    virtual QColor disabledBackgroundColor() const;

protected:
    BackgroundAnimationWidget(BackgroundAnimationWidgetPrivate &dd, QWidget *parent);

    void updateBackgroundColor();

    bool eventFilter(QObject *obj, QEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *e) override;
#else
    void enterEvent(QEvent *e) override;
#endif
    void leaveEvent(QEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

    bool isHover() const;
    bool isPressed() const;
    void setHover(bool hover);
    void setPressed(bool pressed);

    QScopedPointer<BackgroundAnimationWidgetPrivate> d_ptr;
};

// DropShadowAnimation
class QFLUENT_EXPORT DropShadowAnimation : public QPropertyAnimation {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DropShadowAnimation)
public:
    explicit DropShadowAnimation(QWidget *parent,
                                const QColor &normalColor = QColor(0, 0, 0, 0),
                                const QColor &hoverColor = QColor(0, 0, 0, 75));
    ~DropShadowAnimation();

    void setBlurRadius(int radius);
    void setOffset(int dx, int dy);
    void setNormalColor(const QColor &color);
    void setHoverColor(const QColor &color);
    void setColor(const QColor &color);

protected:
    class QGraphicsDropShadowEffect *_createShadowEffect();
    bool eventFilter(QObject *obj, QEvent *e) override;

private slots:
    void _onAniFinished();

private:
    QScopedPointer<DropShadowAnimationPrivate> d_ptr;
};

// FluentAnimationProperObject
class QFLUENT_EXPORT FluentAnimationProperObject : public QObject {
    Q_OBJECT
public:
    explicit FluentAnimationProperObject(QObject *parent = nullptr);
    virtual ~FluentAnimationProperObject();

    virtual QVariant getValue() const = 0;
    virtual void setValue(const QVariant &value) = 0;

    static void registerObject(FluentAnimationProperty name,
                              std::function<FluentAnimationProperObject*(QObject*)> creator);
    static FluentAnimationProperObject *create(FluentAnimationProperty propertyType,
                                              QObject *parent = nullptr);
};

// PositionObject
class PositionObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant position READ getValue WRITE setValue)
public:
    explicit PositionObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &pos) override;

private:
    QPoint _position;
};

// ScaleObject
class ScaleObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant scale READ getValue WRITE setValue)
public:
    explicit ScaleObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &scale) override;

private:
    float _scale = 1.0f;
};

// AngleObject
class AngleObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant angle READ getValue WRITE setValue)
public:
    explicit AngleObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &angle) override;

private:
    float _angle = 0.0f;
};

// OpacityObject
class OpacityObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant opacity READ getValue WRITE setValue)
public:
    explicit OpacityObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &opacity) override;

private:
    float _opacity = 0.0f;
};

// FluentAnimation
class QFLUENT_EXPORT FluentAnimation : public QPropertyAnimation {
    Q_OBJECT
    Q_DECLARE_PRIVATE(FluentAnimation)
public:
    explicit FluentAnimation(QObject *parent = nullptr);
    ~FluentAnimation() override;

    static QEasingCurve createBezierCurve(float x1, float y1, float x2, float y2);
    virtual QEasingCurve curve();

    void setSpeed(FluentAnimationSpeed speed);
    virtual int speedToDuration(FluentAnimationSpeed speed);

    void startAnimation(const QVariant &endValue, const QVariant &startValue = QVariant());
    QVariant value() const;
    void setValue(const QVariant &value);

    static void registerAnimation(FluentAnimationType name,
                                 std::function<FluentAnimation*(QObject*)> creator);
    static FluentAnimation *create(FluentAnimationType aniType,
                                  FluentAnimationProperty propertyType,
                                  FluentAnimationSpeed speed = FluentAnimationSpeed::FAST,
                                  const QVariant &value = QVariant(),
                                  QObject *parent = nullptr);

protected:
    FluentAnimation(FluentAnimationPrivate &dd, QObject *parent);
    QScopedPointer<FluentAnimationPrivate> d_ptr;
};

// FastInvokeAnimation
class QFLUENT_EXPORT FastInvokeAnimation : public FluentAnimation {
    Q_OBJECT
public:
    explicit FastInvokeAnimation(QObject *parent = nullptr);

    QEasingCurve curve() override;
    int speedToDuration(FluentAnimationSpeed speed) override;
};

// StrongInvokeAnimation
class QFLUENT_EXPORT StrongInvokeAnimation : public FluentAnimation {
    Q_OBJECT
public:
    explicit StrongInvokeAnimation(QObject *parent = nullptr);

    QEasingCurve curve() override;
    int speedToDuration(FluentAnimationSpeed speed) override;
};

// FastDismissAnimation
class QFLUENT_EXPORT FastDismissAnimation : public FastInvokeAnimation {
    Q_OBJECT
public:
    explicit FastDismissAnimation(QObject *parent = nullptr);
};

// SoftDismissAnimation
class QFLUENT_EXPORT SoftDismissAnimation : public FluentAnimation {
    Q_OBJECT
public:
    explicit SoftDismissAnimation(QObject *parent = nullptr);

    QEasingCurve curve() override;
    int speedToDuration(FluentAnimationSpeed speed) override;
};

// PointToPointAnimation
class QFLUENT_EXPORT PointToPointAnimation : public FastDismissAnimation {
    Q_OBJECT
public:
    explicit PointToPointAnimation(QObject *parent = nullptr);

    QEasingCurve curve() override;
};

// FadeInOutAnimation
class QFLUENT_EXPORT FadeInOutAnimation : public FluentAnimation {
    Q_OBJECT
public:
    explicit FadeInOutAnimation(QObject *parent = nullptr);

    int speedToDuration(FluentAnimationSpeed speed) override;
};

// ScaleSlideAnimation
class QFLUENT_EXPORT ScaleSlideAnimation : public QParallelAnimationGroup {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ScaleSlideAnimation)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal length READ length WRITE setLength)
    Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry)

public:
    explicit ScaleSlideAnimation(QWidget *parent = nullptr, Qt::Orientation orient = Qt::Horizontal);
    ~ScaleSlideAnimation() override;

    void startAnimation(const QRectF &endRect, bool useCrossFade = false);
    void stopAnimation();

    QPointF pos() const;
    void setPos(const QPointF &pos);

    qreal length() const;
    void setLength(qreal length);

    QRectF geometry() const;
    void setGeometry(const QRectF &rect);

    void moveLeft(qreal x);
    void setValue(const QRectF &rect);

    bool isHorizontal() const;

signals:
    void valueChanged(const QRectF &rect);

private:
    void _startSlideAnimation(const QRectF &startRect, const QRectF &endRect,
                             qreal from, qreal to, qreal dimension);
    void _startCrossFadeAnimation(const QRectF &startRect, const QRectF &endRect);

    QScopedPointer<ScaleSlideAnimationPrivate> d_ptr;
};
