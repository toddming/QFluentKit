#pragma once

#include <memory>
#include <QPointer>
#include <QPushButton>

#include "FluentGlobal.h"
#include "FluentIcon.h"

// PushButton
class RoundMenu;
class QEnterEvent;
class QHBoxLayout;
class ToolButton;
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
    bool isHover() { return m_isHover; }

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *e) override;
#else
    void enterEvent(QEvent *e) override;
#endif
    void leaveEvent(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    virtual void drawIcon(QPainter* painter, const QRectF& rect);

private:
    void init();

    bool m_isPressed = false;
    bool m_isHover = false;
    std::unique_ptr<FluentIconBase> m_fluentIcon;
};


// PrimaryPushButton
class QFLUENT_EXPORT PrimaryPushButton : public PushButton
{
    Q_OBJECT

public:
    using PushButton::PushButton;

protected:
    void drawIcon(QPainter* painter, const QRectF& rect) override;

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

    bool m_isHover = false;
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


// SplitButtonBase
class QFLUENT_EXPORT SplitButtonBase : public QWidget {
    Q_OBJECT
public:
    explicit SplitButtonBase(QWidget* parent = nullptr);
    virtual ~SplitButtonBase();

    // 设置左侧的主控件
    void setWidget(QWidget* widget);

    // 替换下拉按钮
    void setDropButton(ToolButton* button);

    void setDropIconSize(const QSize& size);

    // 设置弹出菜单/窗口
    void setFlyout(QWidget* flyout);

signals:
    void dropDownClicked();

public slots:
    void showFlyout();

protected:
    QHBoxLayout* m_hBoxLayout;
    ToolButton* m_dropButton;
    QPointer<QWidget> m_flyout;
};


// SplitPushButton
class QFLUENT_EXPORT SplitPushButton : public SplitButtonBase {
    Q_OBJECT

public:
    explicit SplitPushButton(QWidget *parent = nullptr);
    explicit SplitPushButton(const QString &text, QWidget* parent = nullptr);
    explicit SplitPushButton(const QString &text, const FluentIconBase &icon, QWidget* parent = nullptr);

    QString text() const;

    void setText(const QString &text);

    void setIconSize(const QSize& size);

signals:
    void clicked();

private:
    PushButton* m_button;

    void init();
};

// PrimarySplitPushButton
class QFLUENT_EXPORT PrimarySplitPushButton : public SplitButtonBase {
    Q_OBJECT

public:
    explicit PrimarySplitPushButton(QWidget *parent = nullptr);
    explicit PrimarySplitPushButton(const QString &text, QWidget* parent = nullptr);
    explicit PrimarySplitPushButton(const QString &text, const FluentIconBase &icon, QWidget* parent = nullptr);

    QString text() const;

    void setText(const QString &text);

    void setIconSize(const QSize& size);

signals:
    void clicked();

private:
    PrimaryPushButton* m_button;

    void init();

};

