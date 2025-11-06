#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <memory>
#include <QPointer>
#include <QPushButton>

#include "Define.h"
#include "FluentIcon.h"

// PushButton
class RoundMenu;
class TranslateYAnimation;
class QFLUENT_EXPORT PushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit PushButton(QWidget *parent = nullptr);
    explicit PushButton(const QString &text, QWidget* parent = nullptr);
    explicit PushButton(const QString &text, const FluentIconBase &icon, QWidget* parent = nullptr);

    void setFluentIcon(const FluentIconBase &icon);
    FluentIconBase* fluentIcon() const;

    bool isPressed() { return m_isPressed; }
    bool isHovered() { return m_isHovered; }

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    virtual void drawIcon(QPainter* painter, const QRectF& rect);

private:
    void init();

    bool m_isPressed;
    bool m_isHovered;
    std::unique_ptr<FluentIconBase> m_fluentIcon;
};


// PrimaryPushButton
class QFLUENT_EXPORT PrimaryPushButton : public PushButton
{
    Q_OBJECT

public:
    using PushButton::PushButton;
};

// TransparentPushButton
class QFLUENT_EXPORT TransparentPushButton : public PushButton
{
    Q_OBJECT

public:
    using PushButton::PushButton;
};


// HyperlinkButton
class QFLUENT_EXPORT HyperlinkButton : public PushButton
{
    Q_OBJECT

public:
    using PushButton::PushButton;

protected:
    void drawIcon(QPainter* painter, const QRectF& rect) override;
};

// ToggleButton
class QFLUENT_EXPORT ToggleButton : public PushButton
{
    Q_OBJECT

public:
    explicit ToggleButton(QWidget *parent = nullptr);
    explicit ToggleButton(const QString &text, QWidget* parent = nullptr);
    explicit ToggleButton(const QString &text, const FluentIconBase &icon, QWidget* parent = nullptr);


protected:
    void drawIcon(QPainter* painter, const QRectF& rect) override;
};

// TransparentTogglePushButton
class QFLUENT_EXPORT TransparentTogglePushButton : public ToggleButton
{
    Q_OBJECT

public:
    using ToggleButton::ToggleButton;
};


// DropDownButtonBase
class QFLUENT_EXPORT DropDownButtonBase : public PushButton
{
    Q_OBJECT
public:
    explicit DropDownButtonBase(QWidget *parent = nullptr);
    explicit DropDownButtonBase(const QString &text, QWidget* parent = nullptr);
    explicit DropDownButtonBase(const QString &text, const FluentIconBase &icon, QWidget* parent = nullptr);

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


// DropDownPushButton
class QFLUENT_EXPORT DropDownPushButton : public DropDownButtonBase
{
    Q_OBJECT
public:
    using DropDownButtonBase::DropDownButtonBase;

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
};


// TransparentDropDownPushButton
class QFLUENT_EXPORT TransparentDropDownPushButton : public DropDownPushButton
{
    Q_OBJECT
public:
    using DropDownPushButton::DropDownPushButton;
};


// PillPushButton
class QFLUENT_EXPORT PillPushButton : public ToggleButton
{
    Q_OBJECT
public:
    using ToggleButton::ToggleButton;

protected:
    void paintEvent(QPaintEvent* event) override;
};


// PrimaryDropDownPushButton
class QFLUENT_EXPORT PrimaryDropDownPushButton : public DropDownButtonBase
{
  Q_OBJECT
public:
  using DropDownButtonBase::DropDownButtonBase;

protected:
    void paintEvent(QPaintEvent* event) override;
    void drawDropDownIcon(QPainter* painter, const QRectF& rect) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void drawIcon(QPainter* painter, const QRectF& rect) override;
};




#endif // PUSHBUTTON_H
