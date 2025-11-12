#pragma once

#include "Define.h"

class QObject;
class QWidget;
class StackedWidget;
class FluentSplitWindow;
class FluentTitleBar;
class NavigationPanel;
class FluentSplitWindowPrivate
{
    Q_D_CREATE(FluentSplitWindow)

private:
    void setDarkTheme(bool dark);

    QObject *windowAgent{nullptr};

    FluentTitleBar *_windowBar;

    StackedWidget *_stacked;

    NavigationPanel *_navPanel;

    QWidget *_userWidget;

    ApplicationType::WindowDisplayMode _windowDisplayMode;
};
