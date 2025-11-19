#pragma once

#include "FluentGlobal.h"
#include "QFluent/FluentWindow.h"

class QObject;
class StackedWidget;
class FluentTitleBar;
class NavigationPanel;
class FluentWindowPrivate
{
    Q_DECLARE_PUBLIC(FluentWindow)

public:
    FluentWindow *q_ptr{nullptr};

    void setDarkTheme(bool dark);

    QObject *_windowAgent{nullptr};

    FluentTitleBar *_windowBar;

    StackedWidget *_stacked;

    NavigationPanel *_navPanel;

    ApplicationType::WindowDisplayMode _windowDisplayMode;
};
