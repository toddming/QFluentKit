#pragma once

#include <QPointer>
#include <QToolButton>
#include "Define.h"

class RoundMenu;
class TranslateYAnimation;
class QFLUENT_EXPORT ToolButton : public QToolButton
{
    Q_OBJECT

public:
    explicit ToolButton(QWidget* parent = nullptr);
    explicit ToolButton(FluentIconType::IconType icon, QWidget* parent = nullptr);
    explicit ToolButton(const QString& templatePath, QWidget* parent = nullptr);
    explicit ToolButton(QIcon icon, QWidget* parent = nullptr);

    void setIcon(FluentIconType::IconType icon);
    void setIcon(const QString& templatePath);
    bool isPressed() const { return m_isPressed; }
    bool isHover() const { return m_isHover; }
    FluentIconType::IconType iconType() const { return m_iconType; }
    QString templatePath() const { return m_templatePath; }

signals:
    // No additional signals defined in the Python base ToolButton.

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    virtual void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);

private:
    virtual void postInit();

    bool m_isPressed;
    bool m_isHover;
    FluentIconType::IconType m_iconType;

    QString m_templatePath;
};


class QFLUENT_EXPORT PrimaryToolButton : public ToolButton
{
    Q_OBJECT
public:
    using ToolButton::ToolButton;

protected:
    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
};


class QFLUENT_EXPORT TransparentToolButton : public ToolButton
{
    Q_OBJECT
public:
    using ToolButton::ToolButton;
};





class QFLUENT_EXPORT ToggleToolButton : public ToolButton
{
    Q_OBJECT
public:
    using ToolButton::ToolButton;
protected:
    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;

private:
    void postInit() override;
};


class QFLUENT_EXPORT TransparentToggleToolButton : public ToggleToolButton
{
    Q_OBJECT
public:
    using ToggleToolButton::ToggleToolButton;

};


class QFLUENT_EXPORT PillToolButton : public ToggleToolButton
{
  Q_OBJECT
public:
    using ToggleToolButton::ToggleToolButton;

protected:
    void paintEvent(QPaintEvent* event) override;
};


class QFLUENT_EXPORT DropDownToolButtonBase : public ToolButton
{
  Q_OBJECT
public:
    explicit DropDownToolButtonBase(QWidget* parent = nullptr);
    explicit DropDownToolButtonBase(FluentIconType::IconType icon, QWidget* parent = nullptr);
    explicit DropDownToolButtonBase(const QString& templatePath, QWidget* parent = nullptr);
    explicit DropDownToolButtonBase(QIcon icon, QWidget* parent = nullptr);

    ~DropDownToolButtonBase();

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

    bool m_isHover = false;
    bool m_isPressed = false;
};



class QFLUENT_EXPORT DropDownToolButton : public DropDownToolButtonBase
{
  Q_OBJECT
public:
    using DropDownToolButton::DropDownToolButton;
protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);
};


class QFLUENT_EXPORT PrimaryDropDownToolButton : public DropDownToolButton
{
  Q_OBJECT
public:
    using DropDownToolButton::DropDownToolButton;

    void drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);
    void drawDropDownIcon(QPainter* painter, const QRectF& rect) override;
};



class QFLUENT_EXPORT TransparentDropDownToolButton : public DropDownToolButton
{
    Q_OBJECT
  public:
      using DropDownToolButton::DropDownToolButton;

};
