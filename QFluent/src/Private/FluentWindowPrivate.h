#pragma once

#include "Define.h"

class QObject;
class StackedWidget;
class FluentWindow;
class FluentTitleBar;
class NavigationPanel;
class FluentWindowPrivate
{
    Q_D_CREATE(FluentWindow)

private:
    void setDarkTheme(bool dark);

    QObject *windowAgent{nullptr};

    FluentTitleBar *_windowBar;

    StackedWidget *_stacked;

    NavigationPanel *_navPanel;

    ApplicationType::WindowDisplayMode _windowDisplayMode;
};
