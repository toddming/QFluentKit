#pragma once

#include <memory>
#include <QToolButton>
#include <QPointer>

#include "FluentGlobal.h"
#include "FluentIcon.h"

class QHBoxLayout;
class QPainter;
class QRectF;
class QSize;
class QIcon;
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
    explicit ToolButton(const FluentIconBase &icon, QWidget *parent = nullptr);

    void setFluentIcon(const FluentIconBase &icon);
    FluentIconBase* fluentIcon() const;

    bool isPressed() const;
    bool isHover() const;

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

private:
    void init();

    std::unique_ptr<FluentIconBase> m_fluentIcon;
};

// PrimaryToolButton
class QFLUENT_EXPORT PrimaryToolButton : public ToolButton
{
    Q_OBJECT

public:
    using ToolButton::ToolButton;

protected:
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
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
    explicit ToggleToolButton(const FluentIconBase &icon, QWidget *parent = nullptr);

protected:
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
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
    explicit DropDownToolButtonBase(const FluentIconBase &icon, QWidget *parent = nullptr);

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
    using DropDownToolButton::DropDownToolButton;

protected:
    void drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
    void drawDropDownIcon(QPainter *painter, const QRectF &rect) override;
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
    virtual ~SplitToolBase();

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
    explicit SplitToolButton(const FluentIconBase &icon, QWidget *parent = nullptr);

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
    explicit PrimarySplitToolButton(const FluentIconBase &icon, QWidget *parent = nullptr);

    void setIconSize(const QSize &size);

signals:
    void clicked();

private:
    PrimaryToolButton *m_button;

    void init();
};
