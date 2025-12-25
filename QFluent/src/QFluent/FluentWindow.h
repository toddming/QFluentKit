#pragma once

#include <QMainWindow>

#include "FluentGlobal.h"

class FluentTitleBar;
class FluentWindowPrivate;
class QFLUENT_EXPORT FluentWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, FluentWindow)

public:
    FluentWindow(QWidget *parent = nullptr);
    ~FluentWindow();

    void setWindowButtonHint(Fluent::WindowButtonHint hint, bool isEnable = true);
    void setWindowButtonHints(Fluent::WindowButtonHints hints);
    Fluent::WindowButtonHints windowButtonHints() const;

    void setWindowEffect(Fluent::WindowEffect effect);
    Fluent::WindowEffect windowEffect() const;

    FluentTitleBar *titleBar() const;

    void setHitTestVisible(QWidget *w, bool visible);

Q_SIGNALS:
    void backRequested();

protected:
    bool event(QEvent *event) override;
    QScopedPointer<FluentWindowPrivate> d_ptr;
};
