#pragma once

#include <QPropertyAnimation>
#include <QColor>
#include <QPoint>
#include <QMap>
#include <QRectF>
#include <functional>

// 前置声明
class QGraphicsDropShadowEffect;
class BackgroundColorObject;
class FluentAnimationProperObject;
class FluentAnimation;

namespace Qt {
enum Orientation;
}

/**
 * @brief AnimationBase的私有实现
 */
class AnimationBasePrivate {
public:
    AnimationBasePrivate() = default;
    virtual ~AnimationBasePrivate() = default;

    // 禁用拷贝和移动
    AnimationBasePrivate(const AnimationBasePrivate &) = delete;
    AnimationBasePrivate &operator=(const AnimationBasePrivate &) = delete;
    AnimationBasePrivate(AnimationBasePrivate &&) = delete;
    AnimationBasePrivate &operator=(AnimationBasePrivate &&) = delete;
};

/**
 * @brief TranslateYAnimation的私有实现
 */
class TranslateYAnimationPrivate : public AnimationBasePrivate {
public:
    TranslateYAnimationPrivate()
        : m_yOffset(0.0f)
        , m_maxOffset(2)
        , m_animation(nullptr)
    {}

    ~TranslateYAnimationPrivate() override = default;

    float m_yOffset;
    int m_maxOffset;
    QPropertyAnimation *m_animation;  // 由父对象管理生命周期
};

/**
 * @brief BackgroundColorObject的私有实现
 */
class BackgroundColorObjectPrivate {
public:
    BackgroundColorObjectPrivate() = default;
    ~BackgroundColorObjectPrivate() = default;

    QColor m_backgroundColor;
};

/**
 * @brief BackgroundAnimationWidget的私有实现
 */
class BackgroundAnimationWidgetPrivate {
public:
    BackgroundAnimationWidgetPrivate()
        : m_isHover(false)
        , m_isPressed(false)
        , m_backgroundColorObject(nullptr)
        , m_backgroundColorAnimation(nullptr)
    {}

    ~BackgroundAnimationWidgetPrivate() = default;

    bool m_isHover;
    bool m_isPressed;
    BackgroundColorObject *m_backgroundColorObject;  // 由父对象管理生命周期
    QPropertyAnimation *m_backgroundColorAnimation;  // 由父对象管理生命周期
};

/**
 * @brief DropShadowAnimation的私有实现
 */
class DropShadowAnimationPrivate {
public:
    DropShadowAnimationPrivate()
        : m_normalColor(0, 0, 0, 0)
        , m_hoverColor(0, 0, 0, 75)
        , m_offset(0, 0)
        , m_blurRadius(38)
        , m_isHover(false)
        , m_shadowEffect(nullptr)
    {}

    ~DropShadowAnimationPrivate() = default;

    QColor m_normalColor;
    QColor m_hoverColor;
    QPoint m_offset;
    int m_blurRadius;
    bool m_isHover;
    QGraphicsDropShadowEffect *m_shadowEffect;  // 由父对象管理生命周期
};

/**
 * @brief FluentAnimation的私有实现
 */
class FluentAnimationPrivate {
public:
    FluentAnimationPrivate() = default;
    ~FluentAnimationPrivate() = default;

    // 注册表 - 用于创建不同类型的动画对象和属性对象
    static QMap<int, std::function<FluentAnimationProperObject*(QObject*)>> propertyObjects;
    static QMap<int, std::function<FluentAnimation*(QObject*)>> animations;
};

/**
 * @brief ScaleSlideAnimation的私有实现
 */
class ScaleSlideAnimationPrivate {
public:
    ScaleSlideAnimationPrivate()
        : m_orientation(Qt::Horizontal)
        , m_geometry(0, 0, 16, 3)
    {}

    ~ScaleSlideAnimationPrivate() = default;

    Qt::Orientation m_orientation;
    QRectF m_geometry;
};
