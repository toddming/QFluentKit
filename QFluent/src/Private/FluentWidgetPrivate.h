#pragma once

#include "FluentGlobal.h"
#include "QFluent/FluentWidget.h"

class QObject;
class FluentTitleBar;
class FluentWidgetPrivate
{
    Q_DECLARE_PUBLIC(FluentWidget)

public:
    FluentWidget *q_ptr{nullptr};

    void setDarkTheme(bool dark);

    QObject *_windowAgent{nullptr};

    FluentTitleBar *_windowBar;

    Fluent::WindowEffect _windowDisplayMode;
};
