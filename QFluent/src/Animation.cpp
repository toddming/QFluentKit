#include "Animation.h"
#include "Private/AnimationPrivate.h"

#include <QEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QSequentialAnimationGroup>

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
void AnimationBase::onHover(QEnterEvent *e) {
    Q_UNUSED(e);
}
#else
void AnimationBase::onHover(QEvent *e) {
    Q_UNUSED(e);
}
#endif

void AnimationBase::onLeave(QEvent *e) {
    Q_UNUSED(e);
}

void AnimationBase::onPress(QMouseEvent *e) {
    Q_UNUSED(e);
}

void AnimationBase::onRelease(QMouseEvent *e) {
    Q_UNUSED(e);
}

bool AnimationBase::eventFilter(QObject *obj, QEvent *e) {
    if (obj == parent()) {
        if (e->type() == QEvent::MouseButtonPress) {
            onPress(static_cast<QMouseEvent*>(e));
        } else if (e->type() == QEvent::MouseButtonRelease) {
            onRelease(static_cast<QMouseEvent*>(e));
        } else if (e->type() == QEvent::Enter) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            onHover(static_cast<QEnterEvent*>(e));
#else
            onHover(e);
#endif
        } else if (e->type() == QEvent::Leave) {
            onLeave(e);
        }
    }
    return QObject::eventFilter(obj, e);
}

// ==================== TranslateYAnimation ====================
TranslateYAnimation::TranslateYAnimation(QWidget *parent, int offset)
    : AnimationBase(*new TranslateYAnimationPrivate(), parent)
{
    Q_D(TranslateYAnimation);
    d->m_maxOffset = offset;
    d->m_ani = new QPropertyAnimation(this, "y", this);
}

TranslateYAnimation::~TranslateYAnimation() = default;

float TranslateYAnimation::y() const {
    Q_D(const TranslateYAnimation);
    return d->m_y;
}

void TranslateYAnimation::setY(float y) {
    Q_D(TranslateYAnimation);
    d->m_y = y;
    if (auto* w = qobject_cast<QWidget*>(parent())) {
        w->update();
    }
    emit valueChanged(y);
}

void TranslateYAnimation::onPress(QMouseEvent *e) {
    Q_UNUSED(e);
    Q_D(TranslateYAnimation);
    d->m_ani->setEndValue(d->m_maxOffset);
    d->m_ani->setEasingCurve(QEasingCurve::OutQuad);
    d->m_ani->setDuration(150);
    d->m_ani->start();
}

void TranslateYAnimation::onRelease(QMouseEvent *e) {
    Q_UNUSED(e);
    Q_D(TranslateYAnimation);
    d->m_ani->setEndValue(0);
    d->m_ani->setDuration(500);
    d->m_ani->setEasingCurve(QEasingCurve::OutElastic);
    d->m_ani->start();
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
    d->m_backgroundColor = color;
    emit backgroundColorChanged(color);
    if (auto widget = qobject_cast<QWidget*>(parent())) {
        widget->update();
    }
}

// ==================== BackgroundAnimationWidget ====================
BackgroundAnimationWidget::BackgroundAnimationWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BackgroundAnimationWidgetPrivate())
{
    init();
}

BackgroundAnimationWidget::BackgroundAnimationWidget(BackgroundAnimationWidgetPrivate &dd, QWidget *parent)
    : QWidget(parent)
    , d_ptr(&dd)
{
    init();
}

void BackgroundAnimationWidget::init()
{
    Q_D(BackgroundAnimationWidget);
    d->m_bgColorObject = new BackgroundColorObject(this);
    d->m_backgroundColorAni = new QPropertyAnimation(d->m_bgColorObject, "backgroundColor", this);
    d->m_backgroundColorAni->setDuration(120);
    installEventFilter(this);
}

BackgroundAnimationWidget::~BackgroundAnimationWidget() = default;

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
    QColor color;
    if (!isEnabled()) {
        color = disabledBackgroundColor();
    } else if (qobject_cast<QLineEdit*>(this) && hasFocus()) {
        color = focusInBackgroundColor();
    } else if (d->m_isPressed) {
        color = pressedBackgroundColor();
    } else if (d->m_isHover) {
        color = hoverBackgroundColor();
    } else {
        color = normalBackgroundColor();
    }

    d->m_backgroundColorAni->stop();
    d->m_backgroundColorAni->setEndValue(color);
    d->m_backgroundColorAni->start();
}

bool BackgroundAnimationWidget::eventFilter(QObject *obj, QEvent *e) {
    if (obj == this) {
        if (e->type() == QEvent::EnabledChange) {
            if (isEnabled()) {
                setBackgroundColor(normalBackgroundColor());
            } else {
                setBackgroundColor(disabledBackgroundColor());
            }
        }
    }
    return QWidget::eventFilter(obj, e);
}

void BackgroundAnimationWidget::mousePressEvent(QMouseEvent *e) {
    Q_D(BackgroundAnimationWidget);
    d->m_isPressed = true;
    updateBackgroundColor();
    QWidget::mousePressEvent(e);
}

void BackgroundAnimationWidget::mouseReleaseEvent(QMouseEvent *e) {
    Q_D(BackgroundAnimationWidget);
    d->m_isPressed = false;
    updateBackgroundColor();
    QWidget::mouseReleaseEvent(e);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void BackgroundAnimationWidget::enterEvent(QEnterEvent *e) {
    Q_D(BackgroundAnimationWidget);
    d->m_isHover = true;
    updateBackgroundColor();
    QWidget::enterEvent(e);
}
#else
void BackgroundAnimationWidget::enterEvent(QEvent *e) {
    Q_D(BackgroundAnimationWidget);
    d->m_isHover = true;
    updateBackgroundColor();
    QWidget::enterEvent(e);
}
#endif

void BackgroundAnimationWidget::leaveEvent(QEvent *e) {
    Q_D(BackgroundAnimationWidget);
    d->m_isHover = false;
    updateBackgroundColor();
    QWidget::leaveEvent(e);
}

void BackgroundAnimationWidget::focusInEvent(QFocusEvent *e) {
    QWidget::focusInEvent(e);
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
    d->m_isHover = hover;
}

void BackgroundAnimationWidget::setPressed(bool pressed) {
    Q_D(BackgroundAnimationWidget);
    d->m_isPressed = pressed;
}

QColor BackgroundAnimationWidget::backgroundColor() const {
    Q_D(const BackgroundAnimationWidget);
    return d->m_bgColorObject->backgroundColor();
}

void BackgroundAnimationWidget::setBackgroundColor(const QColor &color) {
    Q_D(BackgroundAnimationWidget);
    d->m_bgColorObject->setBackgroundColor(color);
}

// ==================== DropShadowAnimation ====================
DropShadowAnimation::DropShadowAnimation(QWidget *parent, const QColor &normalColor, const QColor &hoverColor)
    : QPropertyAnimation(parent)
    , d_ptr(new DropShadowAnimationPrivate())
{
    Q_D(DropShadowAnimation);
    d->m_normalColor = normalColor;
    d->m_hoverColor = hoverColor;
    d->m_shadowEffect = new QGraphicsDropShadowEffect(this);
    d->m_shadowEffect->setColor(normalColor);
    parent->installEventFilter(this);

    // 构造函数中连接一次，避免事件处理中重复连接
    connect(this, &QPropertyAnimation::finished, this, &DropShadowAnimation::onAniFinished);
}

DropShadowAnimation::~DropShadowAnimation() = default;

void DropShadowAnimation::setBlurRadius(int radius) {
    Q_D(DropShadowAnimation);
    d->m_blurRadius = radius;
}

void DropShadowAnimation::setOffset(int dx, int dy) {
    Q_D(DropShadowAnimation);
    d->m_offset = QPoint(dx, dy);
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
    Q_UNUSED(color);
}

QGraphicsDropShadowEffect *DropShadowAnimation::createShadowEffect() {
    Q_D(DropShadowAnimation);

    // Reuse existing effect if still valid (avoid delete/new on every hover)
    if (d->m_shadowEffect) {
        d->m_shadowEffect->setOffset(d->m_offset);
        d->m_shadowEffect->setBlurRadius(d->m_blurRadius);
        d->m_shadowEffect->setColor(d->m_normalColor);

        setTargetObject(d->m_shadowEffect);
        setStartValue(d->m_shadowEffect->color());
        setPropertyName("color");
        setDuration(150);

        return d->m_shadowEffect;
    }

    d->m_shadowEffect = new QGraphicsDropShadowEffect(this);
    d->m_shadowEffect->setOffset(d->m_offset);
    d->m_shadowEffect->setBlurRadius(d->m_blurRadius);
    d->m_shadowEffect->setColor(d->m_normalColor);

    setTargetObject(d->m_shadowEffect);
    setStartValue(d->m_shadowEffect->color());
    setPropertyName("color");
    setDuration(150);

    return d->m_shadowEffect;
}

bool DropShadowAnimation::eventFilter(QObject *obj, QEvent *e) {
    Q_D(DropShadowAnimation);
    QWidget *p = qobject_cast<QWidget*>(parent());
    if (!p) {
        return QPropertyAnimation::eventFilter(obj, e);
    }
    if (obj == p && p->isEnabled()) {
        if (e->type() == QEvent::Enter) {
            d->m_isHover = true;
            if (state() != QPropertyAnimation::Running) {
                p->setGraphicsEffect(createShadowEffect());
            }
            setEndValue(d->m_hoverColor);
            start();
        } else if (e->type() == QEvent::Leave || e->type() == QEvent::MouseButtonPress) {
            d->m_isHover = false;
            if (p->graphicsEffect()) {
                setEndValue(d->m_normalColor);
                start();
            }
        }
    }
    return QPropertyAnimation::eventFilter(obj, e);
}

void DropShadowAnimation::onAniFinished() {
    Q_D(DropShadowAnimation);
    // Reset effect color to normal for next reuse
    if (d->m_shadowEffect) {
        d->m_shadowEffect->setColor(d->m_normalColor);
    }
    // Remove effect from widget when hover animation finishes
    if (auto* w = qobject_cast<QWidget*>(parent())) {
        w->setGraphicsEffect(nullptr);
    }
    // Note: keep m_shadowEffect alive for reuse on next hover
}

// ==================== FluentAnimationProperObject ====================
// 使用函数包装静态 QMap，确保在首次使用时初始化（解决静态初始化顺序问题）
static QMap<FluentAnimationProperty, std::function<FluentAnimationProperObject*(QObject*)>>&
getFluentAnimationProperObjectObjects() {
    static QMap<FluentAnimationProperty, std::function<FluentAnimationProperObject*(QObject*)>> objects;
    return objects;
}

FluentAnimationProperObject::FluentAnimationProperObject(QObject *parent) : QObject(parent) {}

void FluentAnimationProperObject::registerObject(FluentAnimationProperty name,
                                                 std::function<FluentAnimationProperObject*(QObject*)> creator) {
    auto& objects = getFluentAnimationProperObjectObjects();
    if (!objects.contains(name)) {
        objects[name] = creator;
    }
}

FluentAnimationProperObject *FluentAnimationProperObject::create(FluentAnimationProperty propertyType,
                                                                 QObject *parent) {
    auto& objects = getFluentAnimationProperObjectObjects();
    if (!objects.contains(propertyType)) {
        return nullptr;
    }
    return objects[propertyType](parent);
}

// ==================== Property Objects ====================
PositionObject::PositionObject(QObject *parent)
    : FluentAnimationProperObject(parent), m_position(QPoint()) {}

QVariant PositionObject::value() const {
    return m_position;
}

void PositionObject::setValue(const QVariant &pos) {
    m_position = pos.toPoint();
    emit valueChanged(pos);
    if (auto* w = qobject_cast<QWidget*>(parent())) {
        w->update();
    }
}

ScaleObject::ScaleObject(QObject *parent) : FluentAnimationProperObject(parent) {}

QVariant ScaleObject::value() const {
    return m_scale;
}

void ScaleObject::setValue(const QVariant &scale) {
    m_scale = scale.toFloat();
    emit valueChanged(scale);
    if (auto* w = qobject_cast<QWidget*>(parent())) {
        w->update();
    }
}

AngleObject::AngleObject(QObject *parent) : FluentAnimationProperObject(parent) {}

QVariant AngleObject::value() const {
    return m_angle;
}

void AngleObject::setValue(const QVariant &angle) {
    m_angle = angle.toFloat();
    emit valueChanged(angle);
    if (auto* w = qobject_cast<QWidget*>(parent())) {
        w->update();
    }
}

OpacityObject::OpacityObject(QObject *parent) : FluentAnimationProperObject(parent) {}

QVariant OpacityObject::value() const {
    return m_opacity;
}

void OpacityObject::setValue(const QVariant &opacity) {
    m_opacity = opacity.toFloat();
    emit valueChanged(opacity);
    if (auto* w = qobject_cast<QWidget*>(parent())) {
        w->update();
    }
}

// ==================== FluentAnimation ====================
// 使用函数包装静态 QMap，确保在首次使用时初始化（解决静态初始化顺序问题）
static QMap<FluentAnimationType, std::function<FluentAnimation*(QObject*)>>&
getFluentAnimationAnimations() {
    static QMap<FluentAnimationType, std::function<FluentAnimation*(QObject*)>> animations;
    return animations;
}

FluentAnimation::FluentAnimation(QObject *parent)
    : QPropertyAnimation(parent)
    , d_ptr(new FluentAnimationPrivate())
{
    setSpeed(FluentAnimationSpeed::FAST);
    setEasingCurve(curve());
}

FluentAnimation::FluentAnimation(FluentAnimationPrivate &dd, QObject *parent)
    : QPropertyAnimation(parent)
    , d_ptr(&dd)
{
    setSpeed(FluentAnimationSpeed::FAST);
    setEasingCurve(curve());
}

FluentAnimation::~FluentAnimation() = default;

QEasingCurve FluentAnimation::createBezierCurve(float x1, float y1, float x2, float y2) {
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(x1, y1), QPointF(x2, y2), QPointF(1, 1));
    return curve;
}

QEasingCurve FluentAnimation::curve() const {
    return createBezierCurve(0, 0, 1, 1);
}

void FluentAnimation::setSpeed(FluentAnimationSpeed speed) {
    setDuration(speedToDuration(speed));
}

int FluentAnimation::speedToDuration(FluentAnimationSpeed speed) const {
    Q_UNUSED(speed);
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
    auto* obj = qobject_cast<FluentAnimationProperObject*>(targetObject());
    return obj ? obj->value() : QVariant();
}

void FluentAnimation::setValue(const QVariant &value) {
    auto* obj = qobject_cast<FluentAnimationProperObject*>(targetObject());
    if (obj) {
        obj->setValue(value);
    }
}

void FluentAnimation::registerAnimation(FluentAnimationType name,
                                       std::function<FluentAnimation*(QObject*)> creator) {
    auto& animations = getFluentAnimationAnimations();
    if (!animations.contains(name)) {
        animations[name] = creator;
    }
}

FluentAnimation *FluentAnimation::create(FluentAnimationType aniType,
                                        FluentAnimationProperty propertyType,
                                        FluentAnimationSpeed speed,
                                        const QVariant &value,
                                        QObject *parent) {
    auto& animations = getFluentAnimationAnimations();
    if (!animations.contains(aniType)) {
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

// ==================== Specific Animations ====================
FastInvokeAnimation::FastInvokeAnimation(QObject *parent) : FluentAnimation(parent) {}

QEasingCurve FastInvokeAnimation::curve() const {
    return createBezierCurve(0, 0, 0, 1);
}

int FastInvokeAnimation::speedToDuration(FluentAnimationSpeed speed) const {
    if (speed == FluentAnimationSpeed::FAST) return 187;
    if (speed == FluentAnimationSpeed::MEDIUM) return 333;
    return 500;
}

StrongInvokeAnimation::StrongInvokeAnimation(QObject *parent) : FluentAnimation(parent) {}

QEasingCurve StrongInvokeAnimation::curve() const {
    return createBezierCurve(0.13f, 1.62f, 0, 0.92f);
}

int StrongInvokeAnimation::speedToDuration(FluentAnimationSpeed speed) const {
    Q_UNUSED(speed);
    return 667;
}

FastDismissAnimation::FastDismissAnimation(QObject *parent) : FastInvokeAnimation(parent) {}

SoftDismissAnimation::SoftDismissAnimation(QObject *parent) : FluentAnimation(parent) {}

QEasingCurve SoftDismissAnimation::curve() const {
    return createBezierCurve(1, 0, 1, 1);
}

int SoftDismissAnimation::speedToDuration(FluentAnimationSpeed speed) const {
    Q_UNUSED(speed);
    return 167;
}

PointToPointAnimation::PointToPointAnimation(QObject *parent) : FastDismissAnimation(parent) {}

QEasingCurve PointToPointAnimation::curve() const {
    return createBezierCurve(0.55f, 0.55f, 0, 1);
}

FadeInOutAnimation::FadeInOutAnimation(QObject *parent) : FluentAnimation(parent) {}

int FadeInOutAnimation::speedToDuration(FluentAnimationSpeed speed) const {
    Q_UNUSED(speed);
    return 83;
}


// ==================== ScaleSlideAnimation ====================
ScaleSlideAnimation::ScaleSlideAnimation(QWidget *parent, Qt::Orientation orient)
    : QParallelAnimationGroup(parent)
    , d_ptr(new ScaleSlideAnimationPrivate())
{
    Q_D(ScaleSlideAnimation);
    d->m_orient = orient;

    if (isHorizontal()) {
        d->m_geometry = QRectF(0, 0, 16, 3);
    } else {
        d->m_geometry = QRectF(0, 0, 3, 16);
    }
}

ScaleSlideAnimation::~ScaleSlideAnimation() = default;

void ScaleSlideAnimation::startAnimation(const QRectF &endRect, bool useCrossFade) {
    stopAnimation();

    QRectF startRect = geometry();

    // 判断是否在同一层级
    bool sameLevel;
    qreal dim, start, end;

    if (isHorizontal()) {
        sameLevel = qAbs(startRect.y() - endRect.y()) < 1;
        dim = startRect.width();
        start = startRect.x();
        end = endRect.x();
    } else {
        sameLevel = qAbs(startRect.x() - endRect.x()) < 1;
        dim = startRect.height();
        start = startRect.y();
        end = endRect.y();
    }

    if (sameLevel && !useCrossFade) {
        startSlideAnimation(startRect, endRect, start, end, dim);
    } else {
        startCrossFadeAnimation(startRect, endRect);
    }
}

void ScaleSlideAnimation::stopAnimation() {
    stop();
    clear();
}

void ScaleSlideAnimation::startSlideAnimation(const QRectF &startRect, const QRectF &endRect,
                                              qreal from, qreal to, qreal dimension) {
    /* 使用 WinUI 3 的挤压和拉伸逻辑来动画化指示器
     *
     * 核心算法:
     * 1. middleScale = abs(to - from) / dimension + (from < to ? endScale : beginScale)
     * 2. 在 33% 进度时,指示器会拉伸以覆盖两个项目之间的距离
     */

    // 创建位置动画序列
    QPropertyAnimation *posAni1 = new QPropertyAnimation(this, "pos", this);
    QPropertyAnimation *posAni2 = new QPropertyAnimation(this, "pos", this);
    posAni1->setDuration(200);
    posAni2->setDuration(400);
    posAni1->setEasingCurve(FluentAnimation::createBezierCurve(0.9f, 0.1f, 1.0f, 0.2f));
    posAni2->setEasingCurve(FluentAnimation::createBezierCurve(0.1f, 0.9f, 0.2f, 1.0f));

    // 创建长度动画序列
    QPropertyAnimation *lengthAni1 = new QPropertyAnimation(this, "length", this);
    QPropertyAnimation *lengthAni2 = new QPropertyAnimation(this, "length", this);
    lengthAni1->setDuration(200);
    lengthAni2->setDuration(400);
    lengthAni1->setEasingCurve(FluentAnimation::createBezierCurve(0.9f, 0.1f, 1.0f, 0.2f));
    lengthAni2->setEasingCurve(FluentAnimation::createBezierCurve(0.1f, 0.9f, 0.2f, 1.0f));

    // 创建序列动画组
    QSequentialAnimationGroup *posAniGroup = new QSequentialAnimationGroup(this);
    QSequentialAnimationGroup *lengthAniGroup = new QSequentialAnimationGroup(this);
    posAniGroup->addAnimation(posAni1);
    posAniGroup->addAnimation(posAni2);
    lengthAniGroup->addAnimation(lengthAni1);
    lengthAniGroup->addAnimation(lengthAni2);

    addAnimation(posAniGroup);
    addAnimation(lengthAniGroup);

    qreal dist = qAbs(to - from);
    qreal midLength = dist + dimension;
    bool isForward = to > from;

    QPointF startPos = startRect.topLeft();
    QPointF endPos = endRect.topLeft();

    if (isForward) {
        // A--B   ----M--->    A'--B'
        // 0->0.33: B 移动到 M (长度增加)
        posAni1->setStartValue(startPos);
        posAni1->setEndValue(startPos);
        lengthAni1->setStartValue(dimension);
        lengthAni1->setEndValue(midLength);

        // 0.33->1.0: A 移动到 A', B (在 M) 移动到 B'
        posAni2->setStartValue(startPos);
        posAni2->setEndValue(endPos);
        lengthAni2->setStartValue(midLength);
        lengthAni2->setEndValue(dimension);
    } else {
        // A'--B'   <----M----    A--B
        // 0->0.33: A 移动到 M (长度增加)
        posAni1->setStartValue(startPos);
        posAni1->setEndValue(endPos);
        lengthAni1->setStartValue(dimension);
        lengthAni1->setEndValue(midLength);

        // 0.33->1.0: A (在 M) 移动到 A', B 移动到 B'
        posAni2->setStartValue(endPos);
        posAni2->setEndValue(endPos);
        lengthAni2->setStartValue(midLength);
        lengthAni2->setEndValue(dimension);
    }

    start();
}

void ScaleSlideAnimation::startCrossFadeAnimation(const QRectF &startRect, const QRectF &endRect) {
    setGeometry(endRect);

    // 根据相对位置确定增长方向
    // WinUI 3 逻辑: 根据方向从顶部/底部边缘增长
    bool isNextBelow;
    if (isHorizontal()) {
        isNextBelow = endRect.x() > startRect.x();
    } else {
        isNextBelow = endRect.y() > startRect.y();
    }

    QRectF startGeo;
    qreal dim;

    if (isHorizontal()) {
        dim = endRect.width();
        startGeo = QRectF(
            endRect.x() + (isNextBelow ? 0 : dim),
            endRect.y(),
            0,
            endRect.height()
        );
    } else {
        dim = endRect.height();
        startGeo = QRectF(
            endRect.x(),
            endRect.y() + (isNextBelow ? 0 : dim),
            endRect.width(),
            0
        );
    }

    setGeometry(startGeo);

    // 创建长度动画
    QPropertyAnimation *lenAni = new QPropertyAnimation(this, "length", this);
    lenAni->setDuration(600);
    lenAni->setStartValue(0);
    lenAni->setEndValue(dim);
    lenAni->setEasingCurve(QEasingCurve::OutQuint);

    // 创建位置动画
    QPropertyAnimation *posAni = new QPropertyAnimation(this, "pos", this);
    posAni->setDuration(600);
    posAni->setStartValue(startGeo.topLeft());
    posAni->setEndValue(endRect.topLeft());
    posAni->setEasingCurve(QEasingCurve::OutQuint);

    addAnimation(lenAni);
    addAnimation(posAni);
    start();
}

bool ScaleSlideAnimation::isHorizontal() const {
    Q_D(const ScaleSlideAnimation);
    return d->m_orient == Qt::Horizontal;
}

QPointF ScaleSlideAnimation::pos() const {
    return geometry().topLeft();
}

void ScaleSlideAnimation::setPos(const QPointF &pos) {
    Q_D(ScaleSlideAnimation);
    d->m_geometry.moveTopLeft(pos);
    emit valueChanged(geometry());
}

qreal ScaleSlideAnimation::length() const {
    QRectF geo = geometry();
    return isHorizontal() ? geo.width() : geo.height();
}

void ScaleSlideAnimation::setLength(qreal length) {
    Q_D(ScaleSlideAnimation);

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
    d->m_geometry = rect;
}

void ScaleSlideAnimation::moveLeft(qreal x) {
    Q_D(ScaleSlideAnimation);
    d->m_geometry.moveLeft(x);
    emit valueChanged(geometry());
}

void ScaleSlideAnimation::setValue(const QRectF &rect) {
    setGeometry(rect);
}
