#pragma once

#include "FluentGlobal.h"
#include "QFluent/FluentSplitWindow.h"

class QObject;
class QWidget;
class StackedWidget;
class FluentTitleBar;
class NavigationPanel;
class FluentSplitWindowPrivate
{
    Q_DECLARE_PUBLIC(FluentSplitWindow)

public:
    FluentSplitWindow *q_ptr{nullptr};

private:
    void setDarkTheme(bool dark);

    QObject *_windowAgent{nullptr};

    FluentTitleBar *_windowBar;

    StackedWidget *_stacked;

    NavigationPanel *_navPanel;

    QWidget *_userWidget;

    ApplicationType::WindowDisplayMode _windowDisplayMode;
};
