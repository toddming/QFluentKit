#pragma once

#include <QMainWindow>

#include "FluentGlobal.h"

class FluentWidgetPrivate;
class QFLUENT_EXPORT FluentWidget : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, FluentWidget)

public:
    explicit FluentWidget(QMainWindow *parent = nullptr);
    ~FluentWidget();

    void setWindowButtonHint(Fluent::WindowButtonHint hint, bool isEnable = true);
    void setWindowButtonHints(Fluent::WindowButtonHints hints);
    Fluent::WindowButtonHints windowButtonHints() const;

    void setWindowEffect(Fluent::WindowEffect effect);
    Fluent::WindowEffect windowEffect() const;

    void setCustomWindowIcon(const QPixmap &pixmap, const QSize &size);

protected:
    bool event(QEvent *event) override;

private:
    QScopedPointer<FluentWidgetPrivate> d_ptr;

};
