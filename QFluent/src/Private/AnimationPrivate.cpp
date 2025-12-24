#include "AnimationPrivate.h"
#include "Animation.h"

// 静态成员初始化
QMap<int, std::function<FluentAnimationProperObject*(QObject*)>> FluentAnimationPrivate::objects;
QMap<int, std::function<FluentAnimation*(QObject*)>> FluentAnimationPrivate::animations;

// 匿名命名空间，用于注册
namespace {

struct RegisterProperObjects {
    RegisterProperObjects() {
        FluentAnimationProperObject::registerObject(FluentAnimationProperty::POSITION,
            [](QObject* parent) { return new PositionObject(parent); });
        FluentAnimationProperObject::registerObject(FluentAnimationProperty::SCALE,
            [](QObject* parent) { return new ScaleObject(parent); });
        FluentAnimationProperObject::registerObject(FluentAnimationProperty::ANGLE,
            [](QObject* parent) { return new AngleObject(parent); });
        FluentAnimationProperObject::registerObject(FluentAnimationProperty::OPACITY,
            [](QObject* parent) { return new OpacityObject(parent); });
    }
};
static RegisterProperObjects registerProperObjects;

struct RegisterAnimations {
    RegisterAnimations() {
        FluentAnimation::registerAnimation(FluentAnimationType::FAST_INVOKE,
            [](QObject* parent) { return new FastInvokeAnimation(parent); });
        FluentAnimation::registerAnimation(FluentAnimationType::STRONG_INVOKE,
            [](QObject* parent) { return new StrongInvokeAnimation(parent); });
        FluentAnimation::registerAnimation(FluentAnimationType::FAST_DISMISS,
            [](QObject* parent) { return new FastDismissAnimation(parent); });
        FluentAnimation::registerAnimation(FluentAnimationType::SOFT_DISMISS,
            [](QObject* parent) { return new SoftDismissAnimation(parent); });
        FluentAnimation::registerAnimation(FluentAnimationType::POINT_TO_POINT,
            [](QObject* parent) { return new PointToPointAnimation(parent); });
        FluentAnimation::registerAnimation(FluentAnimationType::FADE_IN_OUT,
            [](QObject* parent) { return new FadeInOutAnimation(parent); });
    }
};
static RegisterAnimations registerAnimations;

} // anonymous namespace
