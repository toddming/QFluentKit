#ifndef MENUANIMATIONMANAGER_H
#define MENUANIMATIONMANAGER_H

#include <QObject>
#include <QPropertyAnimation>
#include <QRegion>
#include <QPoint>
#include <QMap>
#include <functional>
#include "../Property.h"

class RoundMenu;

enum class MenuAnimationType {
    NONE,
    DROP_DOWN,
    PULL_UP,
    FADE_IN_DROP_DOWN,
    FADE_IN_PULL_UP
};

class QFLUENT_EXPORT MenuAnimationManager : public QObject
{
    Q_OBJECT

public:
    explicit MenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    virtual ~MenuAnimationManager() = default;

    virtual void exec(const QPoint& pos) = 0;

    void _updateMenuViewport();

    static void registerManager(MenuAnimationType type, std::function<MenuAnimationManager*(RoundMenu*)> creator);
    static MenuAnimationManager* make(RoundMenu* menu, MenuAnimationType aniType);

protected:
    virtual QPoint _endPosition(const QPoint& pos) const;
    virtual std::pair<int, int> _menuSize() const;
    virtual void _onValueChanged();

    RoundMenu* menu() const { return m_menu; }
    QPropertyAnimation* animation() const { return m_ani; }

private:
    RoundMenu* m_menu;
    QPropertyAnimation* m_ani;

    static QMap<MenuAnimationType, std::function<MenuAnimationManager*(RoundMenu*)>> managers;
};

// Dummy Animation
class QFLUENT_EXPORT DummyMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit DummyMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;
};

// Drop Down Animation
class QFLUENT_EXPORT DropDownMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit DropDownMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;

protected:
    void _onValueChanged() override;
};

// Pull Up Animation
class QFLUENT_EXPORT PullUpMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit PullUpMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;

protected:
    QPoint _endPosition(const QPoint& pos) const override;
};

#endif // MENUANIMATIONMANAGER_H
