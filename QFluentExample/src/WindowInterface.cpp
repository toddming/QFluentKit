#include "WindowInterface.h"

#include "QFluent/PushButton.h"
#include "QFluent/PagiNation.h"
#include "Window/LoginWindow.h"

WindowInterface::WindowInterface(QWidget *parent)
    : GalleryInterface("滚动", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("WindowInterface");

    auto button = new PushButton("弹出窗口", this);
    connect(button, &PushButton::clicked, this, [=](){
        auto loginWindow = new LoginWidget(false);
        loginWindow->setWindowModality(Qt::ApplicationModal);
        loginWindow->show();
    });
    addExampleCard("登录窗口", button);
}


