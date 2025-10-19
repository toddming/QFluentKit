#include "Animation.h"

#include <QEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

QMap<FluentAnimationProperty, std::function<FluentAnimationProperObject*(QObject*)>> FluentAnimationProperObject::objects;
QMap<FluentAnimationType, std::function<FluentAnimation*(QObject*)>> FluentAnimation::animations;

namespace {
    struct RegisterProperObjects {
        RegisterProperObjects() {
            FluentAnimationProperObject::registerObject(FluentAnimationProperty::POSITION, [](QObject* parent) { return new PositionObject(parent); });
            FluentAnimationProperObject::registerObject(FluentAnimationProperty::SCALE, [](QObject* parent) { return new ScaleObject(parent); });
            FluentAnimationProperObject::registerObject(FluentAnimationProperty::ANGLE, [](QObject* parent) { return new AngleObject(parent); });
            FluentAnimationProperObject::registerObject(FluentAnimationProperty::OPACITY, [](QObject* parent) { return new OpacityObject(parent); });
        }
    };
    static RegisterProperObjects registerProperObjects;

    struct RegisterAnimations {
        RegisterAnimations() {
            FluentAnimation::registerAnimation(FluentAnimationType::FAST_INVOKE, [](QObject* parent) { return new FastInvokeAnimation(parent); });
            FluentAnimation::registerAnimation(FluentAnimationType::STRONG_INVOKE, [](QObject* parent) { return new StrongInvokeAnimation(parent); });
            FluentAnimation::registerAnimation(FluentAnimationType::FAST_DISMISS, [](QObject* parent) { return new FastDismissAnimation(parent); });
            FluentAnimation::registerAnimation(FluentAnimationType::SOFT_DISMISS, [](QObject* parent) { return new SoftDismissAnimation(parent); });
            FluentAnimation::registerAnimation(FluentAnimationType::POINT_TO_POINT, [](QObject* parent) { return new PointToPointAnimation(parent); });
            FluentAnimation::registerAnimation(FluentAnimationType::FADE_IN_OUT, [](QObject* parent) { return new FadeInOutAnimation(parent); });
        }
    };
    static RegisterAnimations registerAnimations;
}

AnimationBase::AnimationBase(QWidget *parent) : QObject(parent) {
    if (parent) {
        parent->installEventFilter(this);
    }
}

void AnimationBase::_onHover(QEnterEvent *e) {
    Q_UNUSED(e);
}

void AnimationBase::_onLeave(QEvent *e) {
    Q_UNUSED(e);
}

void AnimationBase::_onPress(QMouseEvent *e) {
    Q_UNUSED(e);
}

void AnimationBase::_onRelease(QMouseEvent *e) {
    Q_UNUSED(e);
}

bool AnimationBase::eventFilter(QObject *obj, QEvent *e) {
    if (obj == parent()) {
        if (e->type() == QEvent::MouseButtonPress) {
            _onPress(static_cast<QMouseEvent*>(e));
        } else if (e->type() == QEvent::MouseButtonRelease) {
            _onRelease(static_cast<QMouseEvent*>(e));
        } else if (e->type() == QEvent::Enter) {
            _onHover(static_cast<QEnterEvent*>(e));
        } else if (e->type() == QEvent::Leave) {
            _onLeave(e);
        }
    }
    return QObject::eventFilter(obj, e);
}

TranslateYAnimation::TranslateYAnimation(QWidget *parent, int offset) : AnimationBase(parent), maxOffset(offset) {
    ani = new QPropertyAnimation(this, "y", this);
}

float TranslateYAnimation::y() const {
    return _y;
}

void TranslateYAnimation::setY(float y) {
    _y = y;
    static_cast<QWidget*>(parent())->update();
    emit valueChanged(y);
}

void TranslateYAnimation::_onPress(QMouseEvent *e) {
    Q_UNUSED(e);
    ani->setEndValue(maxOffset);
    ani->setEasingCurve(QEasingCurve::OutQuad);
    ani->setDuration(150);
    ani->start();
}

void TranslateYAnimation::_onRelease(QMouseEvent *e) {
    Q_UNUSED(e);
    ani->setEndValue(0);
    ani->setDuration(500);
    ani->setEasingCurve(QEasingCurve::OutElastic);
    ani->start();
}

BackgroundAnimationWidget::BackgroundAnimationWidget(QWidget *parent) : QWidget(parent) {
    bgColorObject = new BackgroundColorObject(this);
    backgroundColorAni = new QPropertyAnimation(bgColorObject, "backgroundColor", this);
    backgroundColorAni->setDuration(120);
    installEventFilter(this);

    // QObject::connect(&qconfig, &QConfig::themeChanged, this, &BackgroundAnimationWidget::_updateBackgroundColor);
}

QColor BackgroundAnimationWidget::_normalBackgroundColor() const {
    return QColor(0, 0, 0, 0);
}

QColor BackgroundAnimationWidget::_hoverBackgroundColor() const {
    return _normalBackgroundColor();
}

QColor BackgroundAnimationWidget::_pressedBackgroundColor() const {
    return _normalBackgroundColor();
}

QColor BackgroundAnimationWidget::_focusInBackgroundColor() const {
    return _normalBackgroundColor();
}

QColor BackgroundAnimationWidget::_disabledBackgroundColor() const {
    return _normalBackgroundColor();
}

void BackgroundAnimationWidget::_updateBackgroundColor() {
    QColor color;
    if (!isEnabled()) {
        color = _disabledBackgroundColor();
    } else if (qobject_cast<QLineEdit*>(this) && hasFocus()) {
        color = _focusInBackgroundColor();
    } else if (isPressed) {
        color = _pressedBackgroundColor();
    } else if (isHover) {
        color = _hoverBackgroundColor();
    } else {
        color = _normalBackgroundColor();
    }

    backgroundColorAni->stop();
    backgroundColorAni->setEndValue(color);
    backgroundColorAni->start();
}

bool BackgroundAnimationWidget::eventFilter(QObject *obj, QEvent *e) {
    if (obj == this) {
        if (e->type() == QEvent::EnabledChange) {
            if (isEnabled()) {
                setBackgroundColor(_normalBackgroundColor());
            } else {
                setBackgroundColor(_disabledBackgroundColor());
            }
        }
    }
    return QWidget::eventFilter(obj, e);
}

void BackgroundAnimationWidget::mousePressEvent(QMouseEvent *e) {
    isPressed = true;
    _updateBackgroundColor();
    QWidget::mousePressEvent(e);
}

void BackgroundAnimationWidget::mouseReleaseEvent(QMouseEvent *e) {
    isPressed = false;
    _updateBackgroundColor();
    QWidget::mouseReleaseEvent(e);
}

void BackgroundAnimationWidget::enterEvent(QEnterEvent *e) {
    isHover = true;
    _updateBackgroundColor();
    QWidget::enterEvent(e);
}

void BackgroundAnimationWidget::leaveEvent(QEvent *e) {
    isHover = false;
    _updateBackgroundColor();
    QWidget::leaveEvent(e);
}

void BackgroundAnimationWidget::focusInEvent(QFocusEvent *e) {
    QWidget::focusInEvent(e);
    _updateBackgroundColor();
}

QColor BackgroundAnimationWidget::getBackgroundColor() const {
    return bgColorObject->backgroundColor();
}

void BackgroundAnimationWidget::setBackgroundColor(const QColor &color) {
    bgColorObject->setBackgroundColor(color);
}

BackgroundColorObject::BackgroundColorObject(BackgroundAnimationWidget *parent) : QObject(parent) {
    _backgroundColor = parent->_normalBackgroundColor();
}

QColor BackgroundColorObject::backgroundColor() const {
    return _backgroundColor;
}

void BackgroundColorObject::setBackgroundColor(const QColor &color) {
    _backgroundColor = color;
    static_cast<BackgroundAnimationWidget*>(parent())->update();
}

DropShadowAnimation::DropShadowAnimation(QWidget *parent, const QColor &normalColor, const QColor &hoverColor)
    : QPropertyAnimation(parent), normalColor(normalColor), hoverColor(hoverColor) {
    offset = QPoint(0, 0);
    shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(this->normalColor);
    parent->installEventFilter(this);
}

void DropShadowAnimation::setBlurRadius(int radius) {
    blurRadius = radius;
}

void DropShadowAnimation::setOffset(int dx, int dy) {
    offset = QPoint(dx, dy);
}

void DropShadowAnimation::setNormalColor(const QColor &color) {
    normalColor = color;
}

void DropShadowAnimation::setHoverColor(const QColor &color) {
    hoverColor = color;
}

void DropShadowAnimation::setColor(const QColor &color) {
    Q_UNUSED(color);
}

QGraphicsDropShadowEffect *DropShadowAnimation::_createShadowEffect() {
    shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(offset);
    shadowEffect->setBlurRadius(blurRadius);
    shadowEffect->setColor(normalColor);

    setTargetObject(shadowEffect);
    setStartValue(shadowEffect->color());
    setPropertyName("color");
    setDuration(150);

    return shadowEffect;
}

bool DropShadowAnimation::eventFilter(QObject *obj, QEvent *e) {
    QWidget *p = static_cast<QWidget*>(parent());
    if (obj == p && p->isEnabled()) {
        if (e->type() == QEvent::Enter) {
            isHover = true;
            if (state() != QPropertyAnimation::Running) {
                p->setGraphicsEffect(_createShadowEffect());
            }
            setEndValue(hoverColor);
            start();
        } else if (e->type() == QEvent::Leave || e->type() == QEvent::MouseButtonPress) {
            isHover = false;
            if (p->graphicsEffect()) {
                connect(this, &QPropertyAnimation::finished, this, &DropShadowAnimation::_onAniFinished);
                setEndValue(normalColor);
                start();
            }
        }
    }
    return QPropertyAnimation::eventFilter(obj, e);
}

void DropShadowAnimation::_onAniFinished() {
    disconnect(this, &QPropertyAnimation::finished, this, &DropShadowAnimation::_onAniFinished);
    shadowEffect = nullptr;
    static_cast<QWidget*>(parent())->setGraphicsEffect(nullptr);
}

FluentAnimationProperObject::FluentAnimationProperObject(QObject *parent) : QObject(parent) {}

void FluentAnimationProperObject::registerObject(FluentAnimationProperty name, std::function<FluentAnimationProperObject*(QObject*)> creator) {
    if (!objects.contains(name)) {
        objects[name] = creator;
    }
}

FluentAnimationProperObject *FluentAnimationProperObject::create(FluentAnimationProperty propertyType, QObject *parent) {
    if (!objects.contains(propertyType)) {
        qWarning() << "FluentAnimationProperObject: " << static_cast<int>(propertyType) << " has not been registered";
        return nullptr;
    }
    return objects[propertyType](parent);
}

PositionObject::PositionObject(QObject *parent) : FluentAnimationProperObject(parent), _position(QPoint()) {}

QVariant PositionObject::getValue() const {
    return _position;
}

void PositionObject::setValue(const QVariant &pos) {
    _position = pos.toPoint();
    if (parent()) {
        static_cast<QWidget*>(parent())->update();
    }
}

ScaleObject::ScaleObject(QObject *parent) : FluentAnimationProperObject(parent) {}

QVariant ScaleObject::getValue() const {
    return _scale;
}

void ScaleObject::setValue(const QVariant &scale) {
    _scale = scale.toFloat();
    if (parent()) {
        static_cast<QWidget*>(parent())->update();
    }
}

AngleObject::AngleObject(QObject *parent) : FluentAnimationProperObject(parent) {}

QVariant AngleObject::getValue() const {
    return _angle;
}

void AngleObject::setValue(const QVariant &angle) {
    _angle = angle.toFloat();
    if (parent()) {
        static_cast<QWidget*>(parent())->update();
    }
}

OpacityObject::OpacityObject(QObject *parent) : FluentAnimationProperObject(parent) {}

QVariant OpacityObject::getValue() const {
    return _opacity;
}

void OpacityObject::setValue(const QVariant &opacity) {
    _opacity = opacity.toFloat();
    if (parent()) {
        static_cast<QWidget*>(parent())->update();
    }
}

FluentAnimation::FluentAnimation(QObject *parent) : QPropertyAnimation(parent) {
    setSpeed(FluentAnimationSpeed::FAST);
    setEasingCurve(curve());
}

QEasingCurve FluentAnimation::createBezierCurve(float x1, float y1, float x2, float y2) {
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(x1, y1), QPointF(x2, y2), QPointF(1, 1));
    return curve;
}

QEasingCurve FluentAnimation::curve() {
    return createBezierCurve(0, 0, 1, 1);
}

void FluentAnimation::setSpeed(FluentAnimationSpeed speed) {
    setDuration(speedToDuration(speed));
}

int FluentAnimation::speedToDuration(FluentAnimationSpeed speed) {
    return 100;
}

void FluentAnimation::startAnimation(const QVariant &endValue, const QVariant &startValue) {
    stop();
    if (!startValue.isValid()) {
        setStartValue(value());
    } else {
        setStartValue(startValue);
    }
    setEndValue(endValue);
    start();
}

QVariant FluentAnimation::value() const {
    return static_cast<FluentAnimationProperObject*>(targetObject())->getValue();
}

void FluentAnimation::setValue(const QVariant &value) {
    static_cast<FluentAnimationProperObject*>(targetObject())->setValue(value);
}

void FluentAnimation::registerAnimation(FluentAnimationType name, std::function<FluentAnimation*(QObject*)> creator) {
    if (!animations.contains(name)) {
        animations[name] = creator;
    }
}

FluentAnimation *FluentAnimation::create(FluentAnimationType aniType, FluentAnimationProperty propertyType,
                                         FluentAnimationSpeed speed, const QVariant &value, QObject *parent) {
    if (!animations.contains(aniType)) {
        qWarning() << "FluentAnimation: " << static_cast<int>(aniType) << " has not been registered.";
        return nullptr;
    }
    FluentAnimationProperObject *obj = FluentAnimationProperObject::create(propertyType, parent);
    if (!obj) {
        return nullptr;
    }
    FluentAnimation *ani = animations[aniType](parent);
    ani->setSpeed(speed);
    ani->setTargetObject(obj);
    ani->setPropertyName(QByteArray(propertyType == FluentAnimationProperty::POSITION ? "position" :
                                    propertyType == FluentAnimationProperty::SCALE ? "scale" :
                                    propertyType == FluentAnimationProperty::ANGLE ? "angle" : "opacity"));
    if (value.isValid()) {
        ani->setValue(value);
    }
    return ani;
}

FastInvokeAnimation::FastInvokeAnimation(QObject *parent) : FluentAnimation(parent) {}

QEasingCurve FastInvokeAnimation::curve() {
    return createBezierCurve(0, 0, 0, 1);
}

int FastInvokeAnimation::speedToDuration(FluentAnimationSpeed speed) {
    if (speed == FluentAnimationSpeed::FAST) return 187;
    if (speed == FluentAnimationSpeed::MEDIUM) return 333;
    return 500;
}

StrongInvokeAnimation::StrongInvokeAnimation(QObject *parent) : FluentAnimation(parent) {}

QEasingCurve StrongInvokeAnimation::curve() {
    return createBezierCurve(0.13f, 1.62f, 0, 0.92f);
}

int StrongInvokeAnimation::speedToDuration(FluentAnimationSpeed speed) {
    return 667;
}

FastDismissAnimation::FastDismissAnimation(QObject *parent) : FastInvokeAnimation(parent) {}

SoftDismissAnimation::SoftDismissAnimation(QObject *parent) : FluentAnimation(parent) {}

QEasingCurve SoftDismissAnimation::curve() {
    return createBezierCurve(1, 0, 1, 1);
}

int SoftDismissAnimation::speedToDuration(FluentAnimationSpeed speed) {
    return 167;
}

PointToPointAnimation::PointToPointAnimation(QObject *parent) : FastDismissAnimation(parent) {}

QEasingCurve PointToPointAnimation::curve() {
    return createBezierCurve(0.55f, 0.55f, 0, 1);
}

FadeInOutAnimation::FadeInOutAnimation(QObject *parent) : FluentAnimation(parent) {}

int FadeInOutAnimation::speedToDuration(FluentAnimationSpeed speed) {
    return 83;
}
