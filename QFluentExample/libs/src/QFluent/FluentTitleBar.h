#ifndef FLUENTTITLEBAR_H
#define FLUENTTITLEBAR_H

#include <QFrame>
#include <QAbstractButton>
#include "Define.h"

class QEvent;
class QLabel;
class QWidget;
class FluentTitleBarPrivate;
class QFLUENT_EXPORT FluentTitleBar : public QFrame {
    Q_OBJECT
    Q_Q_CREATE(FluentTitleBar)

public:
    explicit FluentTitleBar(QWidget *parent = nullptr);
    ~FluentTitleBar();

public:
    QLabel *titleLabel() const;
    QAbstractButton *iconButton() const;
    QAbstractButton *themeButton() const;
    QAbstractButton *minButton() const;
    QAbstractButton *maxButton() const;
    QAbstractButton *closeButton() const;
    QAbstractButton *backButton() const;
    QLabel *iconLabel() const;

    QWidget *hostWidget() const;
    void setHostWidget(QWidget *w);
    void setWindowButtonFlag(AppBarType::ButtonType buttonFlag, bool isEnable = true);
    void setWindowButtonFlags(AppBarType::ButtonFlags buttonFlags);
    AppBarType::ButtonFlags getWindowButtonFlags() const;

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
};



#endif // FLUENTTITLEBAR_H
