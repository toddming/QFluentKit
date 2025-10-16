#pragma once

#include <QMainWindow>

#include "Define.h"

class FluentWidgetPrivate;
class QFLUENT_EXPORT FluentWidget : public QMainWindow
{
    Q_OBJECT
    Q_Q_CREATE(FluentWidget)

public:
    explicit FluentWidget(QMainWindow *parent = nullptr);
    ~FluentWidget();

    void setWindowButtonFlag(AppBarType::ButtonType buttonFlag, bool isEnable = true);
    void setWindowButtonFlags(AppBarType::ButtonFlags buttonFlags);
    AppBarType::ButtonFlags getWindowButtonFlags() const;

    void setWindowDisplayMode(ApplicationType::WindowDisplayMode windowDisplayType);
    void setCustomWindowIcon(const QPixmap &pixmap, const QSize &size);

protected:
    bool event(QEvent *event) override;

};
