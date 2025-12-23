#include "AnimationPrivate.h"
#include "Animation.h"

// 静态成员初始化
QMap<int, std::function<FluentAnimationProperObject*(QObject*)>>
    FluentAnimationPrivate::propertyObjects;

QMap<int, std::function<FluentAnimation*(QObject*)>>
    FluentAnimationPrivate::animations;

// 匿名命名空间 - 用于自动注册
namespace {

/**
 * @brief 自动注册属性对象创建器
 */
struct PropertyObjectRegistrar {
    PropertyObjectRegistrar() {
        FluentAnimationProperObject::registerObject(
            FluentAnimationProperty::POSITION,
            [](QObject* parent) -> FluentAnimationProperObject* {
                return new PositionObject(parent);
            });

        FluentAnimationProperObject::registerObject(
            FluentAnimationProperty::SCALE,
            [](QObject* parent) -> FluentAnimationProperObject* {
                return new ScaleObject(parent);
            });

        FluentAnimationProperObject::registerObject(
            FluentAnimationProperty::ANGLE,
            [](QObject* parent) -> FluentAnimationProperObject* {
                return new AngleObject(parent);
            });

        FluentAnimationProperObject::registerObject(
            FluentAnimationProperty::OPACITY,
            [](QObject* parent) -> FluentAnimationProperObject* {
                return new OpacityObject(parent);
            });
    }
};

// 全局静态对象 - 在程序启动时自动执行注册
static PropertyObjectRegistrar g_propertyObjectRegistrar;

/**
 * @brief 自动注册动画创建器
 */
struct AnimationRegistrar {
    AnimationRegistrar() {
        FluentAnimation::registerAnimation(
            FluentAnimationType::FAST_INVOKE,
            [](QObject* parent) -> FluentAnimation* {
                return new FastInvokeAnimation(parent);
            });

        FluentAnimation::registerAnimation(
            FluentAnimationType::STRONG_INVOKE,
            [](QObject* parent) -> FluentAnimation* {
                return new StrongInvokeAnimation(parent);
            });

        FluentAnimation::registerAnimation(
            FluentAnimationType::FAST_DISMISS,
            [](QObject* parent) -> FluentAnimation* {
                return new FastDismissAnimation(parent);
            });

        FluentAnimation::registerAnimation(
            FluentAnimationType::SOFT_DISMISS,
            [](QObject* parent) -> FluentAnimation* {
                return new SoftDismissAnimation(parent);
            });

        FluentAnimation::registerAnimation(
            FluentAnimationType::POINT_TO_POINT,
            [](QObject* parent) -> FluentAnimation* {
                return new PointToPointAnimation(parent);
            });

        FluentAnimation::registerAnimation(
            FluentAnimationType::FADE_IN_OUT,
            [](QObject* parent) -> FluentAnimation* {
                return new FadeInOutAnimation(parent);
            });
    }
};

// 全局静态对象 - 在程序启动时自动执行注册
static AnimationRegistrar g_animationRegistrar;

} // anonymous namespace
