#pragma once

#include <QObject>

#include "FluentGlobal.h"

class RoundMenu;
class QPropertyAnimation;
class MenuAnimationManager : public QObject
{
    Q_OBJECT

public:
    explicit MenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    virtual ~MenuAnimationManager() = default;

    virtual void exec(const QPoint& pos) = 0;

    void _updateMenuViewport();

    static void registerManager(Fluent::MenuAnimation type, std::function<MenuAnimationManager*(RoundMenu*)> creator);
    static MenuAnimationManager* make(RoundMenu* menu, Fluent::MenuAnimation aniType);

protected:
    virtual QPoint _endPosition(const QPoint& pos) const;
    virtual std::pair<int, int> _menuSize() const;
    virtual void _onValueChanged();

    RoundMenu* menu() const { return m_menu; }
    QPropertyAnimation* animation() const { return m_ani; }

private:
    RoundMenu* m_menu;
    QPropertyAnimation* m_ani;

    static QMap<Fluent::MenuAnimation, std::function<MenuAnimationManager*(RoundMenu*)>> managers;
};

// Dummy Animation
class DummyMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit DummyMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;
};

// Drop Down Animation
class DropDownMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit DropDownMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;

protected:
    void _onValueChanged() override;
};

// Pull Up Animation
class PullUpMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit PullUpMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;

protected:
    QPoint _endPosition(const QPoint& pos) const override;
};

