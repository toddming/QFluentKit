#pragma once

#include <QPropertyAnimation>
#include <QColor>
#include <QPoint>
#include <QMap>
#include <functional>
#include <QRectF>
#include <Qt>

class QGraphicsDropShadowEffect;
class BackgroundColorObject;
class FluentAnimationProperObject;

// AnimationBasePrivate
class AnimationBasePrivate {
public:
    AnimationBasePrivate() {}
    virtual ~AnimationBasePrivate() = default;
};

// TranslateYAnimationPrivate
class TranslateYAnimationPrivate : public AnimationBasePrivate {
public:
    TranslateYAnimationPrivate() : m_y(0.0f), m_maxOffset(2), m_ani(nullptr) {}

    float m_y;
    int m_maxOffset;
    QPropertyAnimation *m_ani;
};

// BackgroundColorObjectPrivate
class BackgroundColorObjectPrivate {
public:
    BackgroundColorObjectPrivate() {}

    QColor m_backgroundColor;
};

// BackgroundAnimationWidgetPrivate
class BackgroundAnimationWidgetPrivate {
public:
    BackgroundAnimationWidgetPrivate()
        : m_isHover(false)
        , m_isPressed(false)
        , m_bgColorObject(nullptr)
        , m_backgroundColorAni(nullptr)
    {}

    bool m_isHover;
    bool m_isPressed;
    BackgroundColorObject *m_bgColorObject;
    QPropertyAnimation *m_backgroundColorAni;
};

// DropShadowAnimationPrivate
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

    QColor m_normalColor;
    QColor m_hoverColor;
    QPoint m_offset;
    int m_blurRadius;
    bool m_isHover;
    QGraphicsDropShadowEffect *m_shadowEffect;
};

// FluentAnimationPrivate
class FluentAnimationPrivate {
public:
    FluentAnimationPrivate() {}
};

// ScaleSlideAnimationPrivate
class ScaleSlideAnimationPrivate {
public:
    ScaleSlideAnimationPrivate()
        : m_orient(Qt::Horizontal)
        , m_geometry(0, 0, 16, 3)
    {}

    Qt::Orientation m_orient;
    QRectF m_geometry;
};

