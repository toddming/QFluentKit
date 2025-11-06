#pragma once

#include <memory>
#include <QPointer>
#include <QToolButton>

#include "Define.h"
#include "FluentIcon.h"

// ToolButton
class RoundMenu;
class TranslateYAnimation;
class QFLUENT_EXPORT ToolButton : public QToolButton
{
    Q_OBJECT

public:
    explicit ToolButton(QWidget* parent = nullptr);
    explicit ToolButton(const QIcon &icon, QWidget* parent = nullptr);
    explicit ToolButton(const FluentIconBase &icon, QWidget* parent = nullptr);

    void setFluentIcon(const FluentIconBase &icon);
    FluentIconBase* fluentIcon() const;

    bool isPressed() const { return m_isPressed; }
    bool isHovered() const { return m_isHovered; }


protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    virtual void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);

private:
    void init();

    bool m_isPressed;
    bool m_isHovered;
    std::unique_ptr<FluentIconBase> m_fluentIcon;
};

// PrimaryToolButton
class QFLUENT_EXPORT PrimaryToolButton : public ToolButton
{
    Q_OBJECT
public:
    using ToolButton::ToolButton;

protected:
    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
};

// TransparentToolButton
class QFLUENT_EXPORT TransparentToolButton : public ToolButton
{
    Q_OBJECT
public:
    using ToolButton::ToolButton;
};

// ToggleToolButton
class QFLUENT_EXPORT ToggleToolButton : public ToolButton
{
    Q_OBJECT
public:
    explicit ToggleToolButton(QWidget* parent = nullptr);
    explicit ToggleToolButton(const QIcon &icon, QWidget* parent = nullptr);
    explicit ToggleToolButton(const FluentIconBase &icon, QWidget* parent = nullptr);

protected:
    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;

};

// TransparentToggleToolButton
class QFLUENT_EXPORT TransparentToggleToolButton : public ToggleToolButton
{
    Q_OBJECT
public:
    using ToggleToolButton::ToggleToolButton;

};

// PillToolButton
class QFLUENT_EXPORT PillToolButton : public ToggleToolButton
{
    Q_OBJECT
public:
    using ToggleToolButton::ToggleToolButton;

protected:
    void paintEvent(QPaintEvent* event) override;
};

// DropDownToolButtonBase
class QFLUENT_EXPORT DropDownToolButtonBase : public ToolButton
{
    Q_OBJECT
public:
    explicit DropDownToolButtonBase(QWidget* parent = nullptr);
    explicit DropDownToolButtonBase(const FluentIconBase &icon, QWidget* parent = nullptr);

    void setMenu(RoundMenu* menu);

    RoundMenu* menu() const;

    void showMenu();

    void hideMenu();

protected:
    virtual void drawDropDownIcon(QPainter* painter, const QRectF& rect);

    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPointer<RoundMenu> m_menu;
    TranslateYAnimation* m_arrowAni;

    bool m_isHovered = false;
    bool m_isPressed = false;
};


// DropDownToolButton
class QFLUENT_EXPORT DropDownToolButton : public DropDownToolButtonBase
{
    Q_OBJECT
public:
    using DropDownToolButtonBase::DropDownToolButtonBase;

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
};

// PrimaryDropDownToolButton
class QFLUENT_EXPORT PrimaryDropDownToolButton : public DropDownToolButton
{
    Q_OBJECT
public:
    using DropDownToolButton::DropDownToolButton;

    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
    void drawDropDownIcon(QPainter* painter, const QRectF& rect) override;
};


// TransparentDropDownToolButton
class QFLUENT_EXPORT TransparentDropDownToolButton : public DropDownToolButton
{
    Q_OBJECT
public:
    using DropDownToolButton::DropDownToolButton;

};
