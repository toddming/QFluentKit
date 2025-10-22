#pragma once

#include "Define.h"

class QObject;
class FluentWidget;
class FluentTitleBar;
class FluentWidgetPrivate
{
    Q_D_CREATE(FluentWidget)

private:
    void setDarkTheme(bool dark);

    QObject *windowAgent{nullptr};

    FluentTitleBar *_windowBar;
};
