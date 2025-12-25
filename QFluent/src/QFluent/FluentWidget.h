#pragma once

#include <QWidget>

#include "FluentGlobal.h"

class FluentTitleBar;
class FluentWidgetPrivate;
class QFLUENT_EXPORT FluentWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, FluentWidget)

public:
    explicit FluentWidget(QWidget *parent = nullptr);
    ~FluentWidget();

    void setWindowButtonHint(Fluent::WindowButtonHint hint, bool isEnable = true);
    void setWindowButtonHints(Fluent::WindowButtonHints hints);
    Fluent::WindowButtonHints windowButtonHints() const;

    void setWindowEffect(Fluent::WindowEffect effect);
    Fluent::WindowEffect windowEffect() const;

    FluentTitleBar *titleBar() const;

    void setHitTestVisible(QWidget *w, bool visible);

protected:
    void resizeEvent(QResizeEvent *e) override;
    QScopedPointer<FluentWidgetPrivate> d_ptr;
};
