#include "MenuAnimation.h"

#include <QHoverEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QApplication>
#include <QDebug>
#include <QLayout>
#include <QWidget>
#include <QMap>
#include <QPoint>

#include "Screen.h"
#include "RoundMenu.h"
#include "MenuActionListWidget.h"

// 使用局部静态变量解决初始化顺序问题
static QMap<Fluent::MenuAnimation, std::function<MenuAnimationManager*(RoundMenu*)>>& getManagers() {
    static QMap<Fluent::MenuAnimation, std::function<MenuAnimationManager*(RoundMenu*)>> managers;
    return managers;
}

namespace {
    struct RegisterMenuAnimationManagers {
        RegisterMenuAnimationManagers() {
            MenuAnimationManager::registerManager(Fluent::MenuAnimation::NONE,
                [](RoundMenu* menu) { return new DummyMenuAnimationManager(menu, menu); });

            MenuAnimationManager::registerManager(Fluent::MenuAnimation::DROP_DOWN,
                [](RoundMenu* menu) { return new DropDownMenuAnimationManager(menu, menu); });

            MenuAnimationManager::registerManager(Fluent::MenuAnimation::PULL_UP,
                [](RoundMenu* menu) { return new PullUpMenuAnimationManager(menu, menu); });

            MenuAnimationManager::registerManager(Fluent::MenuAnimation::FADE_IN_DROP_DOWN,
                [](RoundMenu* menu) { return new FadeInDropDownMenuAnimationManager(menu, menu); });

            MenuAnimationManager::registerManager(Fluent::MenuAnimation::FADE_IN_PULL_UP,
                [](RoundMenu* menu) { return new FadeInPullUpMenuAnimationManager(menu, menu); });
        }
    };

    static RegisterMenuAnimationManagers registerMenuAnimationManagersInstance;
}

// ================================================================================
// MenuAnimationManager 实现
// ================================================================================

MenuAnimationManager::MenuAnimationManager(RoundMenu* menu, QObject* parent)
    : QObject(parent), m_menu(menu)
{
    if (!menu) {
        qWarning() << "MenuAnimationManager created with null menu!";
    }

    m_ani = new QPropertyAnimation(menu, "pos", this);
    m_ani->setDuration(250);
    m_ani->setEasingCurve(QEasingCurve::OutQuad);

    connect(m_ani, &QPropertyAnimation::valueChanged, this, &MenuAnimationManager::onValueChanged);
    connect(m_ani, &QPropertyAnimation::valueChanged, this, &MenuAnimationManager::updateMenuViewport);
}

void MenuAnimationManager::registerManager(Fluent::MenuAnimation type,
                                           std::function<MenuAnimationManager*(RoundMenu*)> creator)
{
    getManagers()[type] = creator;
}

MenuAnimationManager* MenuAnimationManager::make(RoundMenu* menu, Fluent::MenuAnimation aniType)
{
    auto& map = getManagers();
    if (!map.contains(aniType)) {
        qWarning() << "Unknown animation type:" << static_cast<int>(aniType);
        return nullptr;
    }
    return map[aniType](menu);
}

void MenuAnimationManager::updateMenuViewport()
{
    if (isMenuValid() && menu()->view() && menu()->view()->viewport()) {
        menu()->view()->viewport()->update();
        menu()->view()->setAttribute(Qt::WA_UnderMouse, true);

        QHoverEvent hoverEnter(QEvent::HoverEnter, QPointF(), QPointF(1.0f, 1.0f));
        QApplication::sendEvent(menu()->view(), &hoverEnter);
    }
}

QPoint MenuAnimationManager::endPosition(const QPoint& pos) const
{
    if (!isMenuValid()) {
        return pos;
    }

    RoundMenu *m = menu();

    QMargins margins;
    if (m->layout()) {
        margins = m->layout()->contentsMargins();
    }

    QRect rect = Screen::getCurrentScreenGeometry();
    int w = m->width() + 5;
    int h = m->height();

    int x = qMin(pos.x() - margins.left(), rect.right() - w);
    int y = qMin(pos.y() - 4, rect.bottom() - h + 10);
    return QPoint(x, y);
}

std::pair<int, int> MenuAnimationManager::menuSize() const
{
    if (!isMenuValid()) {
        return {0, 0};
    }

    RoundMenu* m = menu();

    if (!m->view() || !m->layout()) {
        return {0, 0};
    }

    auto margins = m->layout()->contentsMargins();
    int w = m->view()->width() + margins.left() + margins.right() + 120;
    int h = m->view()->height() + margins.top() + margins.bottom() + 20;
    return {w, h};
}

void MenuAnimationManager::onValueChanged()
{
    // 默认实现:不做任何操作
}

RoundMenu* MenuAnimationManager::menu() const
{
    return m_menu.data();
}

QPropertyAnimation* MenuAnimationManager::animation() const
{
    return m_ani;
}

bool MenuAnimationManager::isMenuValid() const
{
    return !m_menu.isNull();
}

// ================================================================================
// DummyMenuAnimationManager 实现
// ================================================================================

DummyMenuAnimationManager::DummyMenuAnimationManager(RoundMenu* menu, QObject* parent)
    : MenuAnimationManager(menu, parent)
{
}

void DummyMenuAnimationManager::exec(const QPoint& pos)
{
    if (isMenuValid()) {
        menu()->move(endPosition(pos));
    }
}

// ================================================================================
// DropDownMenuAnimationManager 实现
// ================================================================================

DropDownMenuAnimationManager::DropDownMenuAnimationManager(RoundMenu* menu, QObject* parent)
    : MenuAnimationManager(menu, parent)
{
}

void DropDownMenuAnimationManager::exec(const QPoint& pos)
{
    if (!isMenuValid()) {
        return;
    }

    QPoint endPos = endPosition(pos);
    int h = menu()->height() + 5;
    animation()->setStartValue(endPos - QPoint(0, h / 2));
    animation()->setEndValue(endPos);
    animation()->start();
}

void DropDownMenuAnimationManager::onValueChanged()
{
    if (!isMenuValid()) {
        return;
    }

    auto [w, h] = menuSize();

    if (w == 0 && h == 0) {
        return;
    }

    QVariant endVal = animation()->endValue();
    QVariant curVal = animation()->currentValue();

    if (endVal.isValid() && curVal.isValid()) {
        int y = endVal.toPoint().y() - curVal.toPoint().y();
        menu()->setMask(QRegion(0, y, w, h));
    }
}

// ================================================================================
// PullUpMenuAnimationManager 实现
// ================================================================================

PullUpMenuAnimationManager::PullUpMenuAnimationManager(RoundMenu* menu, QObject* parent)
    : MenuAnimationManager(menu, parent)
{
}

void PullUpMenuAnimationManager::exec(const QPoint& pos)
{
    if (!isMenuValid()) {
        return;
    }

    QPoint endPos = endPosition(pos);
    int h = menu()->height() + 5;
    animation()->setStartValue(endPos + QPoint(0, h / 2));
    animation()->setEndValue(endPos);
    animation()->start();
}

QPoint PullUpMenuAnimationManager::endPosition(const QPoint& pos) const
{
    if (!isMenuValid()) {
        return pos;
    }

    RoundMenu *m = menu();

    QMargins margins;
    if (m->layout()) {
        margins = m->layout()->contentsMargins();
    }

    QRect rect = Screen::getCurrentScreenGeometry();
    int w = m->width() + 5;
    int h = m->height();
    int x = qMin(pos.x() - margins.left(), rect.right() - w);
    int y = qMax(pos.y() - h + 15, rect.top() + 4);
    return QPoint(x, y);
}

// ================================================================================
// FadeInDropDownMenuAnimationManager 实现
// ================================================================================

FadeInDropDownMenuAnimationManager::FadeInDropDownMenuAnimationManager(RoundMenu* menu, QObject* parent)
    : MenuAnimationManager(menu, parent)
{
    m_opacityAni = new QPropertyAnimation(menu, "windowOpacity", this);
    m_aniGroup = new QParallelAnimationGroup(this);

    m_aniGroup->addAnimation(animation());
    m_aniGroup->addAnimation(m_opacityAni);
}

void FadeInDropDownMenuAnimationManager::exec(const QPoint& pos)
{
    if (!isMenuValid()) {
        return;
    }

    QPoint endPos = endPosition(pos);

    // 设置透明度动画
    m_opacityAni->setStartValue(0.0);
    m_opacityAni->setEndValue(1.0);
    m_opacityAni->setDuration(150);
    m_opacityAni->setEasingCurve(QEasingCurve::OutQuad);

    // 设置位置动画
    animation()->setStartValue(endPos - QPoint(0, 8));
    animation()->setEndValue(endPos);
    animation()->setDuration(150);
    animation()->setEasingCurve(QEasingCurve::OutQuad);

    m_aniGroup->start();
}

// ================================================================================
// FadeInPullUpMenuAnimationManager 实现
// ================================================================================

FadeInPullUpMenuAnimationManager::FadeInPullUpMenuAnimationManager(RoundMenu* menu, QObject* parent)
    : MenuAnimationManager(menu, parent)
{
    m_opacityAni = new QPropertyAnimation(menu, "windowOpacity", this);
    m_aniGroup = new QParallelAnimationGroup(this);

    m_aniGroup->addAnimation(animation());
    m_aniGroup->addAnimation(m_opacityAni);
}

void FadeInPullUpMenuAnimationManager::exec(const QPoint& pos)
{
    if (!isMenuValid()) {
        return;
    }

    QPoint endPos = endPosition(pos);

    // 设置透明度动画
    m_opacityAni->setStartValue(0.0);
    m_opacityAni->setEndValue(1.0);
    m_opacityAni->setDuration(150);
    m_opacityAni->setEasingCurve(QEasingCurve::OutQuad);

    // 设置位置动画
    animation()->setStartValue(endPos + QPoint(0, 8));
    animation()->setEndValue(endPos);
    animation()->setDuration(200);
    animation()->setEasingCurve(QEasingCurve::OutQuad);

    m_aniGroup->start();
}

QPoint FadeInPullUpMenuAnimationManager::endPosition(const QPoint& pos) const
{
    if (!isMenuValid()) {
        return pos;
    }

    RoundMenu *m = menu();

    QMargins margins;
    if (m->layout()) {
        margins = m->layout()->contentsMargins();
    }

    QRect rect = Screen::getCurrentScreenGeometry();
    int w = m->width() + 5;
    int h = m->height();
    int x = qMin(pos.x() - margins.left(), rect.right() - w);
    int y = qMax(pos.y() - h + 15, rect.top() + 4);
    return QPoint(x, y);
}
