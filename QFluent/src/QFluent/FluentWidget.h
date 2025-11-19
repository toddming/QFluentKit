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

    void setWindowButtonFlag(AppBarType::ButtonType buttonFlag, bool isEnable = true);
    void setWindowButtonFlags(AppBarType::ButtonFlags buttonFlags);
    AppBarType::ButtonFlags getWindowButtonFlags() const;

    void setWindowDisplayMode(ApplicationType::WindowDisplayMode windowDisplayType);
    ApplicationType::WindowDisplayMode windowDisplayMode() const;

    void setCustomWindowIcon(const QPixmap &pixmap, const QSize &size);

protected:
    bool event(QEvent *event) override;

private:
    QScopedPointer<FluentWidgetPrivate> d_ptr;

};
