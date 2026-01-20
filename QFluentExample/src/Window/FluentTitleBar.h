#pragma once
#include <QFrame>
#include <QPointer>
#include <QAbstractButton>

// ========== AppBar 按钮类型 ==========
enum WindowButtonHint {
    None               = 0x0000,
    RouteBack          = 0x0001,
    WindowIcon         = 0x0002,
    StayOnTop          = 0x0004,
    ThemeToggle        = 0x0008,
    Minimize           = 0x0010,
    Maximize           = 0x0020,
    Close              = 0x0040,
    Title              = 0x0080
};
Q_DECLARE_FLAGS(WindowButtonHints, WindowButtonHint)
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowButtonHints)

class QEvent;
class QLabel;
class QWidget;
class QPushButton;
class FluentTitleBarPrivate;

class FluentTitleBar : public QFrame {
    Q_OBJECT
public:
    explicit FluentTitleBar(QWidget *parent = nullptr);
    ~FluentTitleBar();

    bool titleFollowWindow() const;
    void setTitleFollowWindow(bool value);

    bool iconFollowWindow() const;
    void setIconFollowWindow(bool value);

public:
    QLabel *titleLabel() const;
    QAbstractButton *iconButton() const;
    QAbstractButton *themeButton() const;
    QAbstractButton *minButton() const;
    QAbstractButton *maxButton() const;
    QAbstractButton *closeButton() const;
    QAbstractButton *backButton() const;

    QWidget *centerWidget() const;
    QWidget *hostWidget() const;
    void setHostWidget(QWidget *w);

    void setWindowButtonHint(WindowButtonHint hint, bool isEnable = true);
    void setWindowButtonHints(WindowButtonHints hints);
    WindowButtonHints windowButtonHints() const;

Q_SIGNALS:
    void backRequested();
    void themeRequested(bool dark = true);
    void minimizeRequested();
    void maximizeRequested(bool max = false);
    void closeRequested();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void titleChanged(const QString &text);
    virtual void iconChanged(const QIcon &icon);

private:
    QPushButton *_minButton;
    QPushButton *_maxButton;
    QPushButton *_closeButton;
    QPushButton *_themeButton;
    QPushButton *_backButton;
    QPushButton *_iconButton;
    QLabel *_titleLabel;
    QPointer<QWidget> _hostWidget;
    QWidget *_centerWidget;
    WindowButtonHints _buttonFlags;
    bool _autoTitle = true;
    bool _autoIcon = true;
};
