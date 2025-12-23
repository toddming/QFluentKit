#pragma once

#include "FluentGlobal.h"

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

// 前置声明 - 减少头文件依赖
class QWidget;
class QEasingCurve;
class QVariant;
class QMouseEvent;
class QEvent;
class QEnterEvent;
class QGraphicsDropShadowEffect;

// 私有实现类前置声明
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

/**
 * @brief 动画基类 - 提供基本的事件过滤和动画管理功能
 *
 * 这个类作为所有动画类的基础,提供了鼠标和悬停事件的处理机制。
 * 使用 PIMPL 模式来隐藏实现细节。
 */
class QFLUENT_EXPORT AnimationBase : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(AnimationBase)

public:
    explicit AnimationBase(QWidget *parent = nullptr);
    ~AnimationBase() override;

    // 禁用拷贝和移动
    AnimationBase(const AnimationBase &) = delete;
    AnimationBase &operator=(const AnimationBase &) = delete;
    AnimationBase(AnimationBase &&) = delete;
    AnimationBase &operator=(AnimationBase &&) = delete;

protected:
    AnimationBase(AnimationBasePrivate &dd, QWidget *parent);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual void onHoverEvent(QEnterEvent *event);
#else
    virtual void onHoverEvent(QEvent *event);
#endif
    virtual void onLeaveEvent(QEvent *event);
    virtual void onPressEvent(QMouseEvent *event);
    virtual void onReleaseEvent(QMouseEvent *event);

    bool eventFilter(QObject *watched, QEvent *event) override;

    QScopedPointer<AnimationBasePrivate> d_ptr;
};

/**
 * @brief Y轴平移动画类
 *
 * 提供按下和释放时的垂直位移动画效果。
 * 按下时向下偏移,释放时弹性回弹。
 */
class QFLUENT_EXPORT TranslateYAnimation : public AnimationBase {
    Q_OBJECT
    Q_DECLARE_PRIVATE(TranslateYAnimation)
    Q_PROPERTY(float yOffset READ y WRITE setYOffset NOTIFY yOffsetChanged)

public:
    explicit TranslateYAnimation(QWidget *parent, int maxOffset = 2);
    ~TranslateYAnimation() override;

    float y() const;
    void setYOffset(float offset);

signals:
    void yOffsetChanged(float offset);

protected:
    void onPressEvent(QMouseEvent *event) override;
    void onReleaseEvent(QMouseEvent *event) override;
};

/**
 * @brief 背景颜色对象类
 *
 * 用于管理widget的背景颜色,配合动画使用。
 */
class QFLUENT_EXPORT BackgroundColorObject : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(BackgroundColorObject)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit BackgroundColorObject(QWidget *parent);
    ~BackgroundColorObject() override;

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

private:
    QScopedPointer<BackgroundColorObjectPrivate> d_ptr;
};

/**
 * @brief 背景动画Widget基类
 *
 * 提供背景色动画功能的Widget基类。
 * 支持hover、pressed、disabled等状态的颜色过渡动画。
 */
class QFLUENT_EXPORT BackgroundAnimationWidget : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(BackgroundAnimationWidget)

public:
    explicit BackgroundAnimationWidget(QWidget *parent = nullptr);
    ~BackgroundAnimationWidget() override;

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

    // 子类可重写以定义不同状态的背景色
    virtual QColor normalBackgroundColor() const;
    virtual QColor hoverBackgroundColor() const;
    virtual QColor pressedBackgroundColor() const;
    virtual QColor focusInBackgroundColor() const;
    virtual QColor disabledBackgroundColor() const;

protected:
    BackgroundAnimationWidget(BackgroundAnimationWidgetPrivate &dd, QWidget *parent);

    void updateBackgroundColor();

    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

    bool isHover() const;
    bool isPressed() const;
    void setHover(bool hover);
    void setPressed(bool pressed);

    QScopedPointer<BackgroundAnimationWidgetPrivate> d_ptr;
};

/**
 * @brief 阴影动画类
 *
 * 提供悬停时的阴影渐变效果。
 */
class QFLUENT_EXPORT DropShadowAnimation : public QPropertyAnimation {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DropShadowAnimation)

public:
    explicit DropShadowAnimation(QWidget *parent,
                                const QColor &normalColor = QColor(0, 0, 0, 0),
                                const QColor &hoverColor = QColor(0, 0, 0, 75));
    ~DropShadowAnimation() override;

    void setBlurRadius(int radius);
    void setOffset(int dx, int dy);
    void setNormalColor(const QColor &color);
    void setHoverColor(const QColor &color);
    void setColor(const QColor &color);

protected:
    QGraphicsDropShadowEffect *createShadowEffect();
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onAnimationFinished();

private:
    QScopedPointer<DropShadowAnimationPrivate> d_ptr;
};

/**
 * @brief 动画属性对象基类
 *
 * 用于封装不同类型的动画属性(位置、缩放、角度、透明度等)。
 */
class QFLUENT_EXPORT FluentAnimationProperObject : public QObject {
    Q_OBJECT

public:
    explicit FluentAnimationProperObject(QObject *parent = nullptr);
    ~FluentAnimationProperObject() override;

    virtual QVariant getValue() const = 0;
    virtual void setValue(const QVariant &value) = 0;

    static void registerObject(FluentAnimationProperty name,
                              std::function<FluentAnimationProperObject*(QObject*)> creator);
    static FluentAnimationProperObject *create(FluentAnimationProperty propertyType,
                                              QObject *parent = nullptr);
};

/**
 * @brief 位置属性对象
 */
class PositionObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant position READ getValue WRITE setValue)

public:
    explicit PositionObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &position) override;

private:
    QPoint m_position;
};

/**
 * @brief 缩放属性对象
 */
class ScaleObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant scale READ getValue WRITE setValue)

public:
    explicit ScaleObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &scale) override;

private:
    float m_scale = 1.0f;
};

/**
 * @brief 角度属性对象
 */
class AngleObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant angle READ getValue WRITE setValue)

public:
    explicit AngleObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &angle) override;

private:
    float m_angle = 0.0f;
};

/**
 * @brief 透明度属性对象
 */
class OpacityObject : public FluentAnimationProperObject {
    Q_OBJECT
    Q_PROPERTY(QVariant opacity READ getValue WRITE setValue)

public:
    explicit OpacityObject(QObject *parent = nullptr);

    QVariant getValue() const override;
    void setValue(const QVariant &opacity) override;

private:
    float m_opacity = 0.0f;
};

/**
 * @brief Fluent动画基类
 *
 * 提供Fluent Design风格的动画曲线和时长配置。
 */
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
    static FluentAnimation *create(FluentAnimationType animationType,
                                  FluentAnimationProperty propertyType,
                                  FluentAnimationSpeed speed = FluentAnimationSpeed::FAST,
                                  const QVariant &value = QVariant(),
                                  QObject *parent = nullptr);

protected:
    FluentAnimation(FluentAnimationPrivate &dd, QObject *parent);
    QScopedPointer<FluentAnimationPrivate> d_ptr;
};

/**
 * @brief 快速调用动画
 */
class QFLUENT_EXPORT FastInvokeAnimation : public FluentAnimation {
    Q_OBJECT

public:
    explicit FastInvokeAnimation(QObject *parent = nullptr);

    QEasingCurve curve() override;
    int speedToDuration(FluentAnimationSpeed speed) override;
};

/**
 * @brief 强烈调用动画
 */
class QFLUENT_EXPORT StrongInvokeAnimation : public FluentAnimation {
    Q_OBJECT

public:
    explicit StrongInvokeAnimation(QObject *parent = nullptr);

    QEasingCurve curve() override;
    int speedToDuration(FluentAnimationSpeed speed) override;
};

/**
 * @brief 快速消失动画
 */
class QFLUENT_EXPORT FastDismissAnimation : public FastInvokeAnimation {
    Q_OBJECT

public:
    explicit FastDismissAnimation(QObject *parent = nullptr);
};

/**
 * @brief 柔和消失动画
 */
class QFLUENT_EXPORT SoftDismissAnimation : public FluentAnimation {
    Q_OBJECT

public:
    explicit SoftDismissAnimation(QObject *parent = nullptr);

    QEasingCurve curve() override;
    int speedToDuration(FluentAnimationSpeed speed) override;
};

/**
 * @brief 点对点动画
 */
class QFLUENT_EXPORT PointToPointAnimation : public FastDismissAnimation {
    Q_OBJECT

public:
    explicit PointToPointAnimation(QObject *parent = nullptr);

    QEasingCurve curve() override;
};

/**
 * @brief 淡入淡出动画
 */
class QFLUENT_EXPORT FadeInOutAnimation : public FluentAnimation {
    Q_OBJECT

public:
    explicit FadeInOutAnimation(QObject *parent = nullptr);

    int speedToDuration(FluentAnimationSpeed speed) override;
};

/**
 * @brief 缩放滑动动画
 *
 * 提供类似WinUI 3的指示器滑动动画效果。
 * 支持水平和垂直方向,带有挤压拉伸效果。
 */
class QFLUENT_EXPORT ScaleSlideAnimation : public QParallelAnimationGroup {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ScaleSlideAnimation)
    Q_PROPERTY(QPointF position READ position WRITE setPosition)
    Q_PROPERTY(qreal length READ length WRITE setLength)
    Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry)

public:
    explicit ScaleSlideAnimation(QWidget *parent = nullptr,
                                Qt::Orientation orientation = Qt::Horizontal);
    ~ScaleSlideAnimation() override;

    void startAnimation(const QRectF &endRect, bool useCrossFade = false);
    void stopAnimation();

    QPointF position() const;
    void setPosition(const QPointF &position);

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
    void startSlideAnimation(const QRectF &startRect, const QRectF &endRect,
                            qreal from, qreal to, qreal dimension);
    void startCrossFadeAnimation(const QRectF &startRect, const QRectF &endRect);

    QScopedPointer<ScaleSlideAnimationPrivate> d_ptr;
};
