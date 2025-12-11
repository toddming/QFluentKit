#pragma once

#include <QObject>
#include <QPointer>
#include <functional>

#include "FluentGlobal.h"

class RoundMenu;
class QPropertyAnimation;
class QPoint;
template<class Key, class T> class QMap;

class MenuAnimationManager : public QObject
{
    Q_OBJECT

public:
    explicit MenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    virtual ~MenuAnimationManager() = default;

    virtual void exec(const QPoint& pos) = 0;

    void updateMenuViewport();

    static void registerManager(Fluent::MenuAnimation type,
                                std::function<MenuAnimationManager*(RoundMenu*)> creator);
    static MenuAnimationManager* make(RoundMenu* menu, Fluent::MenuAnimation aniType);

protected:
    virtual QPoint endPosition(const QPoint& pos) const;
    virtual std::pair<int, int> menuSize() const;
    virtual void onValueChanged();

    RoundMenu* menu() const;
    QPropertyAnimation* animation() const;
    bool isMenuValid() const;

private:
    QPointer<RoundMenu> m_menu;
    QPropertyAnimation* m_ani;
};

class DummyMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit DummyMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;
};

class DropDownMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit DropDownMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;

protected:
    void onValueChanged() override;
};

class PullUpMenuAnimationManager : public MenuAnimationManager
{
    Q_OBJECT
public:
    explicit PullUpMenuAnimationManager(RoundMenu* menu, QObject* parent = nullptr);
    void exec(const QPoint& pos) override;

protected:
    QPoint endPosition(const QPoint& pos) const override;
};
