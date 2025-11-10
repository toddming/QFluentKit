#include "MenuAnimation.h"

#include <QHoverEvent>
#include <QPropertyAnimation>
#include <QApplication>
#include "Screen.h"
#include "RoundMenu.h"
#include "MenuActionListWidget.h"

QMap<MenuAnimationType::MenuAnimation, std::function<MenuAnimationManager*(RoundMenu*)>> MenuAnimationManager::managers;
namespace {
    struct RegisterMenuAnimationManagers {
        RegisterMenuAnimationManagers() {
            MenuAnimationManager::registerManager(MenuAnimationType::MenuAnimation::NONE,
                [](RoundMenu* menu) { return new DummyMenuAnimationManager(menu); });

            MenuAnimationManager::registerManager(MenuAnimationType::MenuAnimation::DROP_DOWN,
                [](RoundMenu* menu) { return new DropDownMenuAnimationManager(menu); });

            MenuAnimationManager::registerManager(MenuAnimationType::MenuAnimation::PULL_UP,
                [](RoundMenu* menu) { return new PullUpMenuAnimationManager(menu); });
        }
    };

    static RegisterMenuAnimationManagers registerMenuAnimationManagersInstance;
}


MenuAnimationManager::MenuAnimationManager(RoundMenu* menu, QObject* parent)
    : QObject(parent), m_menu(menu)
{
    m_ani = new QPropertyAnimation(menu, "pos", this);
    m_ani->setDuration(250);
    m_ani->setEasingCurve(QEasingCurve::OutQuad);

    connect(m_ani, &QPropertyAnimation::valueChanged, this, &MenuAnimationManager::_onValueChanged);
    connect(m_ani, &QPropertyAnimation::valueChanged, this, &MenuAnimationManager::_updateMenuViewport);
}

void MenuAnimationManager::registerManager(MenuAnimationType::MenuAnimation type,
                                           std::function<MenuAnimationManager*(RoundMenu*)> creator)
{
    managers[type] = creator;
}

MenuAnimationManager* MenuAnimationManager::make(RoundMenu* menu, MenuAnimationType::MenuAnimation aniType)
{
    if (!managers.contains(aniType)) {
        // qWarning() << "Invalid animation type:" << static_cast<int>(aniType);
        return nullptr;
    }
    return managers[aniType](menu);
}

void MenuAnimationManager::_updateMenuViewport()
{
    if (menu()->view()) {
        menu()->view()->viewport()->update();
        menu()->view()->setAttribute(Qt::WA_UnderMouse, true);

        // QHoverEvent hoverEnter(QEvent::HoverEnter, QPointF(), QPointF(1.0f, 1.0f), QPointF());
        // QApplication::sendEvent(menu()->view(), &hoverEnter);

        QHoverEvent hoverEnter(QEvent::HoverEnter, QPointF(), QPointF(1.0f, 1.0f));
        QApplication::sendEvent(menu()->view(), &hoverEnter);
    }
}

QPoint MenuAnimationManager::_endPosition(const QPoint& pos) const
{
    RoundMenu *m = menu();
    QRect rect = Screen::getCurrentScreenGeometry();
    int w = m->width() + 5;
    int h = m->height();
    int x= qMin(pos.x() - m->layout()->contentsMargins().left(), rect.right() - w);
    int y = qMin(pos.y() - 4, rect.bottom() - h + 10);
    return QPoint(x, y);
}

std::pair<int, int> MenuAnimationManager::_menuSize() const
{
    auto margins = menu()->layout()->contentsMargins();
    int w = menu()->view()->width() + margins.left() + margins.right() + 120;
    int h = menu()->view()->height() + margins.top() + margins.bottom() + 20;
    return {w, h};
}

void MenuAnimationManager::_onValueChanged()
{
    // Default: do nothing
}

// ————————————————————————————————————————
// DummyMenuAnimationManager
// ————————————————————————————————————————

DummyMenuAnimationManager::DummyMenuAnimationManager(RoundMenu* menu, QObject* parent)
    : MenuAnimationManager(menu, parent) {}

void DummyMenuAnimationManager::exec(const QPoint& pos)
{
    menu()->move(_endPosition(pos));
}

// ————————————————————————————————————————
// DropDownMenuAnimationManager
// ————————————————————————————————————————

DropDownMenuAnimationManager::DropDownMenuAnimationManager(RoundMenu* menu, QObject* parent)
    : MenuAnimationManager(menu, parent) {}

void DropDownMenuAnimationManager::exec(const QPoint& pos)
{
    QPoint endPos = _endPosition(pos);
    int h = menu()->height() + 5;
    animation()->setStartValue(endPos - QPoint(0, h / 2));
    animation()->setEndValue(endPos);
    animation()->start();
}

void DropDownMenuAnimationManager::_onValueChanged()
{
    auto [w, h] = _menuSize();
    int y = animation()->endValue().toPoint().y() - animation()->currentValue().toPoint().y();
    menu()->setMask(QRegion(0, y, w, h));
}

// ————————————————————————————————————————
// PullUpMenuAnimationManager
// ————————————————————————————————————————

PullUpMenuAnimationManager::PullUpMenuAnimationManager(RoundMenu* menu, QObject* parent)
    : MenuAnimationManager(menu, parent) {}

void PullUpMenuAnimationManager::exec(const QPoint& pos)
{
    QPoint endPos = _endPosition(pos);
    int h = menu()->height() + 5;
    animation()->setStartValue(endPos + QPoint(0, h / 2));
    animation()->setEndValue(endPos);
    animation()->start();
}

QPoint PullUpMenuAnimationManager::_endPosition(const QPoint& pos) const
{
    RoundMenu *m = menu();
    QRect rect = Screen::getCurrentScreenGeometry();
    int w = m->width() + 5;
    int h = m->height();
    int x= qMin(pos.x() - m->layout()->contentsMargins().left(), rect.right() - w);
    int y = qMax(pos.y() - h + 13, rect.top() + 4);
    return QPoint(x, y);
}

