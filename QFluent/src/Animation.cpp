#include "Animation.h"
#include "Private/AnimationPrivate.h"

#include <QEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QSequentialAnimationGroup>
#include <QWidget>
#include <QEasingCurve>
#include <QVariant>
#include <QtMath>

// ==================== AnimationBase ====================
AnimationBase::AnimationBase(QWidget *parent)
    : QObject(parent)
    , d_ptr(new AnimationBasePrivate())
{
    if (parent) {
        parent->installEventFilter(this);
    }
}

AnimationBase::AnimationBase(AnimationBasePrivate &dd, QWidget *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    if (parent) {
        parent->installEventFilter(this);
    }
}

AnimationBase::~AnimationBase() = default;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void AnimationBase::onHoverEvent(QEnterEvent *event) {
    Q_UNUSED(event);
}
#else
void AnimationBase::onHoverEvent(QEvent *event) {
    Q_UNUSED(event);
}
#endif

void AnimationBase::onLeaveEvent(QEvent *event) {
    Q_UNUSED(event);
}

void AnimationBase::onPressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
}

void AnimationBase::onReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
}

bool AnimationBase::eventFilter(QObject *watched, QEvent *event) {
    if (!event) {
        return QObject::eventFilter(watched, event);
    }

    if (watched == parent()) {
        switch (event->type()) {
            case QEvent::MouseButtonPress:
                onPressEvent(static_cast<QMouseEvent*>(event));
                break;
            case QEvent::MouseButtonRelease:
                onReleaseEvent(static_cast<QMouseEvent*>(event));
                break;
            case QEvent::Enter:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                onHoverEvent(static_cast<QEnterEvent*>(event));
#else
                onHoverEvent(event);
#endif
                break;
            case QEvent::Leave:
                onLeaveEvent(event);
                break;
            default:
                break;
        }
    }
    return QObject::eventFilter(watched, event);
}

// ==================== TranslateYAnimation ====================
TranslateYAnimation::TranslateYAnimation(QWidget *parent, int maxOffset)
    : AnimationBase(*new TranslateYAnimationPrivate(), parent)
{
    Q_D(TranslateYAnimation);
    d->m_maxOffset = maxOffset;
    d->m_animation = new QPropertyAnimation(this, "yOffset", this);
}

TranslateYAnimation::~TranslateYAnimation() = default;

float TranslateYAnimation::y() const {
    Q_D(const TranslateYAnimation);
    return d->m_yOffset;
}

void TranslateYAnimation::setYOffset(float offset) {
    Q_D(TranslateYAnimation);
    if (qFuzzyCompare(d->m_yOffset, offset)) {
        return;
    }

    d->m_yOffset = offset;

    if (QWidget *widget = qobject_cast<QWidget*>(parent())) {
        widget->update();
    }

    emit yOffsetChanged(offset);
}

void TranslateYAnimation::onPressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    Q_D(TranslateYAnimation);

    if (!d->m_animation) {
        return;
    }

    d->m_animation->stop();
    d->m_animation->setEndValue(d->m_maxOffset);
    d->m_animation->setEasingCurve(QEasingCurve::OutQuad);
    d->m_animation->setDuration(150);
    d->m_animation->start();
}

void TranslateYAnimation::onReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    Q_D(TranslateYAnimation);

    if (!d->m_animation) {
        return;
    }

    d->m_animation->stop();
    d->m_animation->setEndValue(0);
    d->m_animation->setDuration(500);
    d->m_animation->setEasingCurve(QEasingCurve::OutElastic);
    d->m_animation->start();
}

// ==================== BackgroundColorObject ====================
BackgroundColorObject::BackgroundColorObject(QWidget *parent)
    : QObject(parent)
    , d_ptr(new BackgroundColorObjectPrivate())
{
}

BackgroundColorObject::~BackgroundColorObject() = default;

QColor BackgroundColorObject::backgroundColor() const {
    Q_D(const BackgroundColorObject);
    return d->m_backgroundColor;
}

void BackgroundColorObject::setBackgroundColor(const QColor &color) {
    Q_D(BackgroundColorObject);

    if (d->m_backgroundColor == color) {
        return;
    }

    d->m_backgroundColor = color;

    if (QWidget *widget = qobject_cast<QWidget*>(parent())) {
        widget->update();
    }
}

// ==================== BackgroundAnimationWidget ====================
BackgroundAnimationWidget::BackgroundAnimationWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BackgroundAnimationWidgetPrivate())
{
    Q_D(BackgroundAnimationWidget);
    d->m_backgroundColorObject = new BackgroundColorObject(this);
    d->m_backgroundColorAnimation = new QPropertyAnimation(
        d->m_backgroundColorObject, "backgroundColor", this);
    d->m_backgroundColorAnimation->setDuration(120);
    installEventFilter(this);
}

BackgroundAnimationWidget::BackgroundAnimationWidget(
    BackgroundAnimationWidgetPrivate &dd, QWidget *parent)
    : QWidget(parent)
    , d_ptr(&dd)
{
    Q_D(BackgroundAnimationWidget);
    d->m_backgroundColorObject = new BackgroundColorObject(this);
    d->m_backgroundColorAnimation = new QPropertyAnimation(
        d->m_backgroundColorObject, "backgroundColor", this);
    d->m_backgroundColorAnimation->setDuration(120);
    installEventFilter(this);
}

BackgroundAnimationWidget::~BackgroundAnimationWidget() = default;

QColor BackgroundAnimationWidget::backgroundColor() const {
    Q_D(const BackgroundAnimationWidget);
    if (d->m_backgroundColorObject) {
        return d->m_backgroundColorObject->backgroundColor();
    }
    return QColor();
}

void BackgroundAnimationWidget::setBackgroundColor(const QColor &color) {
    Q_D(BackgroundAnimationWidget);
    if (d->m_backgroundColorObject) {
        d->m_backgroundColorObject->setBackgroundColor(color);
    }
}

QColor BackgroundAnimationWidget::normalBackgroundColor() const {
    return QColor(0, 0, 0, 0);
}

QColor BackgroundAnimationWidget::hoverBackgroundColor() const {
    return normalBackgroundColor();
}

QColor BackgroundAnimationWidget::pressedBackgroundColor() const {
    return normalBackgroundColor();
}

QColor BackgroundAnimationWidget::focusInBackgroundColor() const {
    return normalBackgroundColor();
}

QColor BackgroundAnimationWidget::disabledBackgroundColor() const {
    return normalBackgroundColor();
}

void BackgroundAnimationWidget::updateBackgroundColor() {
    Q_D(BackgroundAnimationWidget);

    if (!d->m_backgroundColorAnimation) {
        return;
    }

    QColor targetColor;
    if (!isEnabled()) {
        targetColor = disabledBackgroundColor();
    } else if (qobject_cast<QLineEdit*>(this) && hasFocus()) {
        targetColor = focusInBackgroundColor();
    } else if (d->m_isPressed) {
        targetColor = pressedBackgroundColor();
    } else if (d->m_isHover) {
        targetColor = hoverBackgroundColor();
    } else {
        targetColor = normalBackgroundColor();
    }

    d->m_backgroundColorAnimation->stop();
    d->m_backgroundColorAnimation->setEndValue(targetColor);
    d->m_backgroundColorAnimation->start();
}

bool BackgroundAnimationWidget::eventFilter(QObject *watched, QEvent *event) {
    if (watched == this && event) {
        if (event->type() == QEvent::EnabledChange) {
            if (isEnabled()) {
                setBackgroundColor(normalBackgroundColor());
            } else {
                setBackgroundColor(disabledBackgroundColor());
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void BackgroundAnimationWidget::mousePressEvent(QMouseEvent *event) {
    Q_D(BackgroundAnimationWidget);
    d->m_isPressed = true;
    updateBackgroundColor();
    QWidget::mousePressEvent(event);
}

void BackgroundAnimationWidget::mouseReleaseEvent(QMouseEvent *event) {
    Q_D(BackgroundAnimationWidget);
    d->m_isPressed = false;
    updateBackgroundColor();
    QWidget::mouseReleaseEvent(event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void BackgroundAnimationWidget::enterEvent(QEnterEvent *event) {
    Q_D(BackgroundAnimationWidget);
    d->m_isHover = true;
    updateBackgroundColor();
    QWidget::enterEvent(event);
}
#else
void BackgroundAnimationWidget::enterEvent(QEvent *event) {
    Q_D(BackgroundAnimationWidget);
    d->m_isHover = true;
    updateBackgroundColor();
    QWidget::enterEvent(event);
}
#endif

void BackgroundAnimationWidget::leaveEvent(QEvent *event) {
    Q_D(BackgroundAnimationWidget);
    d->m_isHover = false;
    updateBackgroundColor();
    QWidget::leaveEvent(event);
}

void BackgroundAnimationWidget::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    updateBackgroundColor();
}

bool BackgroundAnimationWidget::isHover() const {
    Q_D(const BackgroundAnimationWidget);
    return d->m_isHover;
}

bool BackgroundAnimationWidget::isPressed() const {
    Q_D(const BackgroundAnimationWidget);
    return d->m_isPressed;
}

void BackgroundAnimationWidget::setHover(bool hover) {
    Q_D(BackgroundAnimationWidget);
    if (d->m_isHover != hover) {
        d->m_isHover = hover;
        updateBackgroundColor();
    }
}

void BackgroundAnimationWidget::setPressed(bool pressed) {
    Q_D(BackgroundAnimationWidget);
    if (d->m_isPressed != pressed) {
        d->m_isPressed = pressed;
        updateBackgroundColor();
    }
}

// ==================== DropShadowAnimation ====================
DropShadowAnimation::DropShadowAnimation(QWidget *parent,
                                        const QColor &normalColor,
                                        const QColor &hoverColor)
    : QPropertyAnimation(parent)
    , d_ptr(new DropShadowAnimationPrivate())
{
    Q_D(DropShadowAnimation);
    d->m_normalColor = normalColor;
    d->m_hoverColor = hoverColor;
    d->m_shadowEffect = createShadowEffect();

    setTargetObject(d->m_shadowEffect);
    setPropertyName("color");
    setDuration(150);

    if (parent) {
        parent->installEventFilter(this);
    }

    connect(this, &QPropertyAnimation::finished,
            this, &DropShadowAnimation::onAnimationFinished);
}

DropShadowAnimation::~DropShadowAnimation() = default;

QGraphicsDropShadowEffect *DropShadowAnimation::createShadowEffect() {
    Q_D(DropShadowAnimation);

    if (QWidget *widget = qobject_cast<QWidget*>(parent())) {
        auto *effect = new QGraphicsDropShadowEffect(widget);
        effect->setBlurRadius(d->m_blurRadius);
        effect->setOffset(d->m_offset);
        effect->setColor(d->m_normalColor);
        widget->setGraphicsEffect(effect);
        return effect;
    }

    return nullptr;
}

void DropShadowAnimation::setBlurRadius(int radius) {
    Q_D(DropShadowAnimation);
    d->m_blurRadius = radius;
    if (d->m_shadowEffect) {
        d->m_shadowEffect->setBlurRadius(radius);
    }
}

void DropShadowAnimation::setOffset(int dx, int dy) {
    Q_D(DropShadowAnimation);
    d->m_offset = QPoint(dx, dy);
    if (d->m_shadowEffect) {
        d->m_shadowEffect->setOffset(dx, dy);
    }
}

void DropShadowAnimation::setNormalColor(const QColor &color) {
    Q_D(DropShadowAnimation);
    d->m_normalColor = color;
}

void DropShadowAnimation::setHoverColor(const QColor &color) {
    Q_D(DropShadowAnimation);
    d->m_hoverColor = color;
}

void DropShadowAnimation::setColor(const QColor &color) {
    Q_D(DropShadowAnimation);
    if (d->m_shadowEffect) {
        d->m_shadowEffect->setColor(color);
    }
}

bool DropShadowAnimation::eventFilter(QObject *watched, QEvent *event) {
    if (!event || watched != parent()) {
        return QPropertyAnimation::eventFilter(watched, event);
    }

    Q_D(DropShadowAnimation);

    if (event->type() == QEvent::Enter) {
        d->m_isHover = true;
        setEndValue(d->m_hoverColor);
        start();
    } else if (event->type() == QEvent::Leave) {
        d->m_isHover = false;
        setEndValue(d->m_normalColor);
        start();
    }

    return QPropertyAnimation::eventFilter(watched, event);
}

void DropShadowAnimation::onAnimationFinished() {
    Q_D(DropShadowAnimation);
    if (!d->m_shadowEffect) {
        return;
    }

    // 确保最终颜色正确
    if (d->m_isHover) {
        d->m_shadowEffect->setColor(d->m_hoverColor);
    } else {
        d->m_shadowEffect->setColor(d->m_normalColor);
    }
}

// ==================== FluentAnimationProperObject ====================
FluentAnimationProperObject::FluentAnimationProperObject(QObject *parent)
    : QObject(parent)
{
}

FluentAnimationProperObject::~FluentAnimationProperObject() = default;

void FluentAnimationProperObject::registerObject(
    FluentAnimationProperty name,
    std::function<FluentAnimationProperObject*(QObject*)> creator)
{
    FluentAnimationPrivate::propertyObjects[static_cast<int>(name)] = creator;
}

FluentAnimationProperObject *FluentAnimationProperObject::create(
    FluentAnimationProperty propertyType,
    QObject *parent)
{
    int key = static_cast<int>(propertyType);
    if (FluentAnimationPrivate::propertyObjects.contains(key)) {
        return FluentAnimationPrivate::propertyObjects[key](parent);
    }
    return nullptr;
}

// ==================== PositionObject ====================
PositionObject::PositionObject(QObject *parent)
    : FluentAnimationProperObject(parent)
    , m_position(0, 0)
{
}

QVariant PositionObject::getValue() const {
    return m_position;
}

void PositionObject::setValue(const QVariant &position) {
    m_position = position.toPoint();
}

// ==================== ScaleObject ====================
ScaleObject::ScaleObject(QObject *parent)
    : FluentAnimationProperObject(parent)
{
}

QVariant ScaleObject::getValue() const {
    return m_scale;
}

void ScaleObject::setValue(const QVariant &scale) {
    bool ok = false;
    float value = scale.toFloat(&ok);
    if (ok) {
        m_scale = value;
    }
}

// ==================== AngleObject ====================
AngleObject::AngleObject(QObject *parent)
    : FluentAnimationProperObject(parent)
{
}

QVariant AngleObject::getValue() const {
    return m_angle;
}

void AngleObject::setValue(const QVariant &angle) {
    bool ok = false;
    float value = angle.toFloat(&ok);
    if (ok) {
        m_angle = value;
    }
}

// ==================== OpacityObject ====================
OpacityObject::OpacityObject(QObject *parent)
    : FluentAnimationProperObject(parent)
{
}

QVariant OpacityObject::getValue() const {
    return m_opacity;
}

void OpacityObject::setValue(const QVariant &opacity) {
    bool ok = false;
    float value = opacity.toFloat(&ok);
    if (ok) {
        m_opacity = qBound(0.0f, value, 1.0f);
    }
}

// ==================== FluentAnimation ====================
FluentAnimation::FluentAnimation(QObject *parent)
    : QPropertyAnimation(parent)
    , d_ptr(new FluentAnimationPrivate())
{
}

FluentAnimation::FluentAnimation(FluentAnimationPrivate &dd, QObject *parent)
    : QPropertyAnimation(parent)
    , d_ptr(&dd)
{
}

FluentAnimation::~FluentAnimation() = default;

QEasingCurve FluentAnimation::createBezierCurve(float x1, float y1, float x2, float y2) {
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(x1, y1), QPointF(x2, y2), QPointF(1.0, 1.0));
    return curve;
}

QEasingCurve FluentAnimation::curve() {
    return QEasingCurve::Linear;
}

void FluentAnimation::setSpeed(FluentAnimationSpeed speed) {
    setDuration(speedToDuration(speed));
}

int FluentAnimation::speedToDuration(FluentAnimationSpeed speed) {
    switch (speed) {
        case FluentAnimationSpeed::FAST:
            return 250;
        case FluentAnimationSpeed::MEDIUM:
            return 500;
        case FluentAnimationSpeed::SLOW:
            return 1000;
        default:
            return 250;
    }
}

void FluentAnimation::startAnimation(const QVariant &endValue, const QVariant &startValue) {
    if (startValue.isValid()) {
        setStartValue(startValue);
    }
    setEndValue(endValue);
    setEasingCurve(curve());
    start();
}

QVariant FluentAnimation::value() const {
    return currentValue();
}

void FluentAnimation::setValue(const QVariant &value) {
    if (FluentAnimationProperObject *obj =
        qobject_cast<FluentAnimationProperObject*>(targetObject())) {
        obj->setValue(value);
    }
}

void FluentAnimation::registerAnimation(
    FluentAnimationType name,
    std::function<FluentAnimation*(QObject*)> creator)
{
    FluentAnimationPrivate::animations[static_cast<int>(name)] = creator;
}

FluentAnimation *FluentAnimation::create(
    FluentAnimationType animationType,
    FluentAnimationProperty propertyType,
    FluentAnimationSpeed speed,
    const QVariant &value,
    QObject *parent)
{
    // 创建动画对象
    int aniKey = static_cast<int>(animationType);
    if (!FluentAnimationPrivate::animations.contains(aniKey)) {
        return nullptr;
    }

    FluentAnimation *animation = FluentAnimationPrivate::animations[aniKey](parent);
    if (!animation) {
        return nullptr;
    }

    // 创建属性对象
    FluentAnimationProperObject *propertyObject =
        FluentAnimationProperObject::create(propertyType, animation);

    if (!propertyObject) {
        delete animation;
        return nullptr;
    }

    // 设置初始值
    if (value.isValid()) {
        propertyObject->setValue(value);
    }

    // 配置动画
    animation->setTargetObject(propertyObject);

    QString propertyName;
    switch (propertyType) {
        case FluentAnimationProperty::POSITION:
            propertyName = "position";
            break;
        case FluentAnimationProperty::SCALE:
            propertyName = "scale";
            break;
        case FluentAnimationProperty::ANGLE:
            propertyName = "angle";
            break;
        case FluentAnimationProperty::OPACITY:
            propertyName = "opacity";
            break;
    }

    animation->setPropertyName(propertyName.toLatin1());
    animation->setSpeed(speed);

    return animation;
}

// ==================== FastInvokeAnimation ====================
FastInvokeAnimation::FastInvokeAnimation(QObject *parent)
    : FluentAnimation(parent)
{
}

QEasingCurve FastInvokeAnimation::curve() {
    return createBezierCurve(0.0f, 0.22f, 0.19f, 1.0f);
}

int FastInvokeAnimation::speedToDuration(FluentAnimationSpeed speed) {
    switch (speed) {
        case FluentAnimationSpeed::FAST:
            return 187;
        case FluentAnimationSpeed::MEDIUM:
            return 333;
        case FluentAnimationSpeed::SLOW:
            return 500;
        default:
            return 187;
    }
}

// ==================== StrongInvokeAnimation ====================
StrongInvokeAnimation::StrongInvokeAnimation(QObject *parent)
    : FluentAnimation(parent)
{
}

QEasingCurve StrongInvokeAnimation::curve() {
    return createBezierCurve(0.13f, 1.62f, 0.0f, 0.92f);
}

int StrongInvokeAnimation::speedToDuration(FluentAnimationSpeed speed) {
    Q_UNUSED(speed);
    return 667;
}

// ==================== FastDismissAnimation ====================
FastDismissAnimation::FastDismissAnimation(QObject *parent)
    : FastInvokeAnimation(parent)
{
}

// ==================== SoftDismissAnimation ====================
SoftDismissAnimation::SoftDismissAnimation(QObject *parent)
    : FluentAnimation(parent)
{
}

QEasingCurve SoftDismissAnimation::curve() {
    return createBezierCurve(1.0f, 0.0f, 1.0f, 1.0f);
}

int SoftDismissAnimation::speedToDuration(FluentAnimationSpeed speed) {
    Q_UNUSED(speed);
    return 167;
}

// ==================== PointToPointAnimation ====================
PointToPointAnimation::PointToPointAnimation(QObject *parent)
    : FastDismissAnimation(parent)
{
}

QEasingCurve PointToPointAnimation::curve() {
    return createBezierCurve(0.55f, 0.55f, 0.0f, 1.0f);
}

// ==================== FadeInOutAnimation ====================
FadeInOutAnimation::FadeInOutAnimation(QObject *parent)
    : FluentAnimation(parent)
{
}

int FadeInOutAnimation::speedToDuration(FluentAnimationSpeed speed) {
    Q_UNUSED(speed);
    return 83;
}

// ==================== ScaleSlideAnimation ====================
ScaleSlideAnimation::ScaleSlideAnimation(QWidget *parent, Qt::Orientation orientation)
    : QParallelAnimationGroup(parent)
    , d_ptr(new ScaleSlideAnimationPrivate())
{
    Q_D(ScaleSlideAnimation);
    d->m_orientation = orientation;

    if (isHorizontal()) {
        d->m_geometry = QRectF(0, 0, 16, 3);
    } else {
        d->m_geometry = QRectF(0, 0, 3, 16);
    }
}

ScaleSlideAnimation::~ScaleSlideAnimation() = default;

void ScaleSlideAnimation::startAnimation(const QRectF &endRect, bool useCrossFade) {
    stopAnimation();

    const QRectF startRect = geometry();

    // 判断是否在同一层级
    bool sameLevel;
    qreal dimension, start, end;

    if (isHorizontal()) {
        sameLevel = qAbs(startRect.y() - endRect.y()) < 1.0;
        dimension = startRect.width();
        start = startRect.x();
        end = endRect.x();
    } else {
        sameLevel = qAbs(startRect.x() - endRect.x()) < 1.0;
        dimension = startRect.height();
        start = startRect.y();
        end = endRect.y();
    }

    if (sameLevel && !useCrossFade) {
        startSlideAnimation(startRect, endRect, start, end, dimension);
    } else {
        startCrossFadeAnimation(startRect, endRect);
    }
}

void ScaleSlideAnimation::stopAnimation() {
    stop();
    clear();
}

void ScaleSlideAnimation::startSlideAnimation(
    const QRectF &startRect, const QRectF &endRect,
    qreal from, qreal to, qreal dimension)
{
    /* 使用 WinUI 3 的挤压和拉伸逻辑来动画化指示器
     *
     * 核心算法:
     * 1. middleScale = abs(to - from) / dimension + (from < to ? endScale : beginScale)
     * 2. 在 33% 进度时,指示器会拉伸以覆盖两个项目之间的距离
     */

    // 创建位置动画序列
    auto *posAnimation1 = new QPropertyAnimation(this, "position", this);
    auto *posAnimation2 = new QPropertyAnimation(this, "position", this);
    posAnimation1->setDuration(200);
    posAnimation2->setDuration(400);
    posAnimation1->setEasingCurve(FluentAnimation::createBezierCurve(0.9f, 0.1f, 1.0f, 0.2f));
    posAnimation2->setEasingCurve(FluentAnimation::createBezierCurve(0.1f, 0.9f, 0.2f, 1.0f));

    // 创建长度动画序列
    auto *lengthAnimation1 = new QPropertyAnimation(this, "length", this);
    auto *lengthAnimation2 = new QPropertyAnimation(this, "length", this);
    lengthAnimation1->setDuration(200);
    lengthAnimation2->setDuration(400);
    lengthAnimation1->setEasingCurve(FluentAnimation::createBezierCurve(0.9f, 0.1f, 1.0f, 0.2f));
    lengthAnimation2->setEasingCurve(FluentAnimation::createBezierCurve(0.1f, 0.9f, 0.2f, 1.0f));

    // 创建序列动画组
    auto *posAnimationGroup = new QSequentialAnimationGroup(this);
    auto *lengthAnimationGroup = new QSequentialAnimationGroup(this);
    posAnimationGroup->addAnimation(posAnimation1);
    posAnimationGroup->addAnimation(posAnimation2);
    lengthAnimationGroup->addAnimation(lengthAnimation1);
    lengthAnimationGroup->addAnimation(lengthAnimation2);

    addAnimation(posAnimationGroup);
    addAnimation(lengthAnimationGroup);

    const qreal distance = qAbs(to - from);
    const qreal midLength = distance + dimension;
    const bool isForward = to > from;

    const QPointF startPos = startRect.topLeft();
    const QPointF endPos = endRect.topLeft();

    if (isForward) {
        // A--B   ----M--->    A'--B'
        // 0->0.33: B 移动到 M (长度增加)
        posAnimation1->setStartValue(startPos);
        posAnimation1->setEndValue(startPos);
        lengthAnimation1->setStartValue(dimension);
        lengthAnimation1->setEndValue(midLength);

        // 0.33->1.0: A 移动到 A', B (在 M) 移动到 B'
        posAnimation2->setStartValue(startPos);
        posAnimation2->setEndValue(endPos);
        lengthAnimation2->setStartValue(midLength);
        lengthAnimation2->setEndValue(dimension);
    } else {
        // A'--B'   <----M----    A--B
        // 0->0.33: A 移动到 M (长度增加)
        posAnimation1->setStartValue(startPos);
        posAnimation1->setEndValue(endPos);
        lengthAnimation1->setStartValue(dimension);
        lengthAnimation1->setEndValue(midLength);

        // 0.33->1.0: A (在 M) 移动到 A', B 移动到 B'
        posAnimation2->setStartValue(endPos);
        posAnimation2->setEndValue(endPos);
        lengthAnimation2->setStartValue(midLength);
        lengthAnimation2->setEndValue(dimension);
    }

    start();
}

void ScaleSlideAnimation::startCrossFadeAnimation(
    const QRectF &startRect, const QRectF &endRect)
{
    setGeometry(endRect);

    // 根据相对位置确定增长方向
    // WinUI 3 逻辑: 根据方向从顶部/底部边缘增长
    bool isNextBelow;
    if (isHorizontal()) {
        isNextBelow = endRect.x() > startRect.x();
    } else {
        isNextBelow = endRect.y() > startRect.y();
    }

    QRectF startGeometry;
    qreal dim;

    if (isHorizontal()) {
        dim = endRect.width();
        startGeometry = QRectF(
            endRect.x() + (isNextBelow ? 0.0 : dim),
            endRect.y(),
            0.0,
            endRect.height()
        );
    } else {
        dim = endRect.height();
        startGeometry = QRectF(
            endRect.x(),
            endRect.y() + (isNextBelow ? 0.0 : dim),
            endRect.width(),
            0.0
        );
    }

    setGeometry(startGeometry);

    // 创建长度动画
    auto *lengthAnimation = new QPropertyAnimation(this, "length", this);
    lengthAnimation->setDuration(600);
    lengthAnimation->setStartValue(0.0);
    lengthAnimation->setEndValue(dim);
    lengthAnimation->setEasingCurve(QEasingCurve::OutQuint);

    // 创建位置动画
    auto *posAnimation = new QPropertyAnimation(this, "position", this);
    posAnimation->setDuration(600);
    posAnimation->setStartValue(startGeometry.topLeft());
    posAnimation->setEndValue(endRect.topLeft());
    posAnimation->setEasingCurve(QEasingCurve::OutQuint);

    addAnimation(lengthAnimation);
    addAnimation(posAnimation);
    start();
}

bool ScaleSlideAnimation::isHorizontal() const {
    Q_D(const ScaleSlideAnimation);
    return d->m_orientation == Qt::Horizontal;
}

QPointF ScaleSlideAnimation::position() const {
    return geometry().topLeft();
}

void ScaleSlideAnimation::setPosition(const QPointF &position) {
    Q_D(ScaleSlideAnimation);
    if (d->m_geometry.topLeft() != position) {
        d->m_geometry.moveTopLeft(position);
        emit valueChanged(geometry());
    }
}

qreal ScaleSlideAnimation::length() const {
    const QRectF geo = geometry();
    return isHorizontal() ? geo.width() : geo.height();
}

void ScaleSlideAnimation::setLength(qreal length) {
    Q_D(ScaleSlideAnimation);

    qreal currentLength = this->length();
    if (qFuzzyCompare(currentLength, length)) {
        return;
    }

    if (isHorizontal()) {
        d->m_geometry.setWidth(length);
    } else {
        d->m_geometry.setHeight(length);
    }

    emit valueChanged(geometry());
}

QRectF ScaleSlideAnimation::geometry() const {
    Q_D(const ScaleSlideAnimation);
    return d->m_geometry;
}

void ScaleSlideAnimation::setGeometry(const QRectF &rect) {
    Q_D(ScaleSlideAnimation);
    if (d->m_geometry != rect) {
        d->m_geometry = rect;
        emit valueChanged(rect);
    }
}

void ScaleSlideAnimation::moveLeft(qreal x) {
    Q_D(ScaleSlideAnimation);
    if (!qFuzzyCompare(d->m_geometry.left(), x)) {
        d->m_geometry.moveLeft(x);
        emit valueChanged(geometry());
    }
}

void ScaleSlideAnimation::setValue(const QRectF &rect) {
    setGeometry(rect);
}
