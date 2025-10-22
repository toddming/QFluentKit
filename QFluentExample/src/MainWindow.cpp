#include "MainWindow.h"
#include "GalleryInterface.h"

#include "QFluent/PushButton.h"

MainWindow::MainWindow()
{
    setMinimumSize(866, 600);

    auto w = new GalleryInterface("基本输入1", "qfluentwidgets.components.widgets", this);
    addSubInterface("1", IconType::FLuentIcon::HOME, "主页1", w, true, NavigationType::NavigationItemPosition::TOP);

    auto w2 = new GalleryInterface("基本输入2", "qfluentwidgets.components.widgets", this);
    addSubInterface("2", IconType::FLuentIcon::HOME, "主页2", w2, true, NavigationType::NavigationItemPosition::TOP);

    for (int i=0; i < 10; i++) {
        w2->addExampleCard("按钮", new PushButton("按钮", w2), "");
    }
}
