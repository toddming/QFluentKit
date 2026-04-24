#pragma once

#include <QToolButton>
#include <QPointer>
#include "FluentIcon.h"

#include "FluentGlobal.h"

class QHBoxLayout;
class QPainter;
class QRectF;
class QSize;
class QMouseEvent;
class QPaintEvent;
class QEvent;

class RoundMenu;
class TranslateYAnimation;

// ToolButton
class QFLUENT_EXPORT ToolButton : public QToolButton
{
    Q_OBJECT

public:
    explicit ToolButton(QWidget *parent = nullptr);
    explicit ToolButton(const QIcon &icon, QWidget *parent = nullptr);
    explicit ToolButton(Fluent::IconType type, QWidget *parent = nullptr);

    bool isPressed() const;
    bool isHover() const;

    void setIcon(Fluent::IconType type);
    void setIcon(const QIcon &icon);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    virtual void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);

    bool m_isPressed;
    bool m_isHover;
    FluentQIcon m_fluentIcon;

private:
    void init();
};

// PrimaryToolButton
class QFLUENT_EXPORT PrimaryToolButton : public ToolButton
{
    Q_OBJECT

public:
    explicit PrimaryToolButton(QWidget *parent = nullptr);
    explicit PrimaryToolButton(Fluent::IconType type, QWidget *parent = nullptr);
    explicit PrimaryToolButton(const QIcon &icon, QWidget *parent = nullptr);
    void setIcon(Fluent::IconType type);
    void setIcon(const QIcon &icon);

protected:
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;

private:
    FluentQIcon m_fluentIcon;
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
    explicit ToggleToolButton(QWidget *parent = nullptr);
    explicit ToggleToolButton(const QIcon &icon, QWidget *parent = nullptr);
    explicit ToggleToolButton(Fluent::IconType type, QWidget *parent = nullptr);

    void setIcon(Fluent::IconType type);
    void setIcon(const QIcon &icon);

protected:
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;

private:
    FluentQIcon m_fluentIcon;
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
    void paintEvent(QPaintEvent *event) override;
};

// DropDownToolButtonBase
class QFLUENT_EXPORT DropDownToolButtonBase : public ToolButton
{
    Q_OBJECT

public:
    explicit DropDownToolButtonBase(QWidget *parent = nullptr);
    explicit DropDownToolButtonBase(const QIcon &icon, QWidget *parent = nullptr);
    explicit DropDownToolButtonBase(Fluent::IconType type, QWidget *parent = nullptr);

    void setMenu(RoundMenu *menu);
    RoundMenu* menu() const;

    void showMenu();
    void hideMenu();

protected:
    virtual void drawDropDownIcon(QPainter *painter, const QRectF &rect);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPointer<RoundMenu> m_menu;
    TranslateYAnimation *m_arrowAni;
};


// DropDownToolButton
class QFLUENT_EXPORT DropDownToolButton : public DropDownToolButtonBase
{
    Q_OBJECT

public:
    using DropDownToolButtonBase::DropDownToolButtonBase;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
};

// PrimaryDropDownToolButton
class QFLUENT_EXPORT PrimaryDropDownToolButton : public DropDownToolButton
{
    Q_OBJECT

public:
    explicit PrimaryDropDownToolButton(QWidget *parent = nullptr);
    explicit PrimaryDropDownToolButton(Fluent::IconType type, QWidget *parent = nullptr);
    explicit PrimaryDropDownToolButton(const QIcon &icon, QWidget *parent = nullptr);

    void setIcon(Fluent::IconType type);
    void setIcon(const QIcon &icon);

protected:
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
    void drawDropDownIcon(QPainter *painter, const QRectF &rect) override;

private:
    FluentQIcon m_fluentIcon;
};


// TransparentDropDownToolButton
class QFLUENT_EXPORT TransparentDropDownToolButton : public DropDownToolButton
{
    Q_OBJECT

public:
    using DropDownToolButton::DropDownToolButton;
};

// SplitDropButton
class QFLUENT_EXPORT SplitDropButton : public ToolButton
{
    Q_OBJECT

public:
    explicit SplitDropButton(QWidget *parent = nullptr);

protected:
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;

private:
    TranslateYAnimation *m_arrowAni;
};


// PrimarySplitDropButton
class QFLUENT_EXPORT PrimarySplitDropButton : public PrimaryToolButton
{
    Q_OBJECT

public:
    explicit PrimarySplitDropButton(QWidget *parent = nullptr);

protected:
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;

private:
    TranslateYAnimation *m_arrowAni;
};


// SplitToolBase
class QFLUENT_EXPORT SplitToolBase : public QWidget
{
    Q_OBJECT

public:
    explicit SplitToolBase(QWidget *parent = nullptr);
    ~SplitToolBase() override = default;

    void setWidget(QWidget *widget);
    void setDropButton(ToolButton *button);
    void setDropIconSize(const QSize &size);
    void setFlyout(QWidget *flyout);

signals:
    void dropDownClicked();

public slots:
    void showFlyout();

protected:
    QHBoxLayout *m_hBoxLayout;
    ToolButton *m_dropButton;
    QPointer<QWidget> m_flyout;
};


// SplitToolButton
class QFLUENT_EXPORT SplitToolButton : public SplitToolBase
{
    Q_OBJECT

public:
    explicit SplitToolButton(QWidget *parent = nullptr);
    explicit SplitToolButton(const QIcon &icon, QWidget *parent = nullptr);
    explicit SplitToolButton(Fluent::IconType type, QWidget *parent = nullptr);

    void setIconSize(const QSize &size);

signals:
    void clicked();

private:
    ToolButton *m_button;

    void init();
};

// PrimarySplitToolButton
class QFLUENT_EXPORT PrimarySplitToolButton : public SplitToolBase
{
    Q_OBJECT

public:
    explicit PrimarySplitToolButton(QWidget *parent = nullptr);
    explicit PrimarySplitToolButton(const QIcon &icon, QWidget *parent = nullptr);
    explicit PrimarySplitToolButton(Fluent::IconType type, QWidget *parent = nullptr);

    void setIconSize(const QSize &size);

signals:
    void clicked();

private:
    PrimaryToolButton *m_button;

    void init();
};
