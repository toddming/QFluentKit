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
    virtual ~AnimationBasePrivate() {}
};

// TranslateYAnimationPrivate
class TranslateYAnimationPrivate : public AnimationBasePrivate {
public:
    TranslateYAnimationPrivate() : _y(0.0f), maxOffset(2), ani(nullptr) {}

    float _y;
    int maxOffset;
    QPropertyAnimation *ani;
};

// BackgroundColorObjectPrivate
class BackgroundColorObjectPrivate {
public:
    BackgroundColorObjectPrivate() {}

    QColor _backgroundColor;
};

// BackgroundAnimationWidgetPrivate
class BackgroundAnimationWidgetPrivate {
public:
    BackgroundAnimationWidgetPrivate()
        : m_isHover(false)
        , m_isPressed(false)
        , bgColorObject(nullptr)
        , backgroundColorAni(nullptr)
    {}

    bool m_isHover;
    bool m_isPressed;
    BackgroundColorObject *bgColorObject;
    QPropertyAnimation *backgroundColorAni;
};

// DropShadowAnimationPrivate
class DropShadowAnimationPrivate {
public:
    DropShadowAnimationPrivate()
        : normalColor(0, 0, 0, 0)
        , hoverColor(0, 0, 0, 75)
        , offset(0, 0)
        , blurRadius(38)
        , isHover(false)
        , shadowEffect(nullptr)
    {}

    QColor normalColor;
    QColor hoverColor;
    QPoint offset;
    int blurRadius;
    bool isHover;
    QGraphicsDropShadowEffect *shadowEffect;
};

// FluentAnimationPrivate
class FluentAnimationPrivate {
public:
    FluentAnimationPrivate() {}

    static QMap<int, std::function<FluentAnimationProperObject*(QObject*)>> objects;
    static QMap<int, std::function<class FluentAnimation*(QObject*)>> animations;
};

// ScaleSlideAnimationPrivate
class ScaleSlideAnimationPrivate {
public:
    ScaleSlideAnimationPrivate()
        : orient(Qt::Horizontal)
        , _geometry(0, 0, 16, 3)
    {}

    Qt::Orientation orient;
    QRectF _geometry;
};

