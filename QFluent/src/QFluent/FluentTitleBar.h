#ifndef FLUENTTITLEBAR_H
#define FLUENTTITLEBAR_H

#include <QFrame>
#include <QAbstractButton>
#include "FluentGlobal.h"

class QEvent;
class QLabel;
class QWidget;
class FluentTitleBarPrivate;
class QFLUENT_EXPORT FluentTitleBar : public QFrame {
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, FluentTitleBar)

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
    void setWindowButtonHint(Fluent::WindowButtonHint hint, bool isEnable = true);
    void setWindowButtonHints(Fluent::WindowButtonHints hints);
    Fluent::WindowButtonHints windowButtonHints() const;

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
    QScopedPointer<FluentTitleBarPrivate> d_ptr;

};



#endif // FLUENTTITLEBAR_H
