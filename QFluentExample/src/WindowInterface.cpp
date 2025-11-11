#include "WindowInterface.h"

#include "QFluent/PushButton.h"
#include "Window/LoginWindow.h"
#include "Window/NavbarWindow.h"

#include "ConfigManager.h"

WindowInterface::WindowInterface(QWidget *parent)
    : GalleryInterface("滚动", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("WindowInterface");

    const QString effect = ConfigManager::instance().getValue("Window/effect", "none").toString();
    QStringList modes; modes << "none" << "dwm-blur" << "acrylic-material" << "mica" << "miac-alt";
    static int windowMode = modes.indexOf(effect);


    auto button1 = new PushButton("弹出窗口", this);
    connect(button1, &PushButton::clicked, this, [=](){
        auto loginWindow = new LoginWidget(false);
        loginWindow->setWindowModality(Qt::ApplicationModal);
        loginWindow->setWindowDisplayMode(static_cast<ApplicationType::WindowDisplayMode>(windowMode));
        loginWindow->show();
    });
    addExampleCard("登录窗口", button1);

    auto button2 = new PushButton("弹出窗口", this);
    connect(button2, &PushButton::clicked, this, [=](){
        auto navWidget = new NavbarWidget();
        navWidget->setWindowDisplayMode(static_cast<ApplicationType::WindowDisplayMode>(windowMode));
        navWidget->show();
    });
    addExampleCard("带导航栏的窗口", button2);
}


