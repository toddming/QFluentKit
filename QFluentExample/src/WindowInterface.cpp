#include "WindowInterface.h"

#include "QFluent/PushButton.h"
#include "Window/LoginWindow.h"
#include "Window/NavbarWindow.h"
#include "Window/SplitWindow.h"

#include "MainWindow.h"
#include "ConfigManager.h"

WindowInterface::WindowInterface(QWidget *parent)
    : GalleryInterface("滚动", "", parent)
{
    setObjectName("WindowInterface");

    auto mainWindow = qobject_cast<MainWindow*>(this->window());

    auto button1 = new PushButton("弹出窗口", this);
    connect(button1, &PushButton::clicked, this, [=](){
        auto loginWindow = new LoginWidget(false);
        loginWindow->setWindowModality(Qt::ApplicationModal);
        loginWindow->setWindowEffect(mainWindow->windowEffect());
        loginWindow->show();
    });
    addExampleCard("登录窗口", button1);

    auto button2 = new PushButton("弹出窗口", this);
    connect(button2, &PushButton::clicked, this, [=](){
        auto navWidget = new NavbarWidget();
        navWidget->setWindowEffect(mainWindow->windowEffect());
        navWidget->show();
    });
    addExampleCard("带导航栏的窗口", button2);

    auto button3 = new PushButton("弹出窗口", this);
    connect(button3, &PushButton::clicked, this, [=](){
        auto splitWindow = new SplitWidget();
        splitWindow->setWindowEffect(mainWindow->windowEffect());
        splitWindow->show();
    });
    addExampleCard("分割风格的窗口", button3);
}


