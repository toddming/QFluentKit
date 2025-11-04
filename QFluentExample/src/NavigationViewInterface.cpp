#include "NavigationViewInterface.h"

#include "FluentIcon.h"
#include "QFluent/navigation/Pivot.h"

NavigationViewInterface::NavigationViewInterface(QWidget *parent)
    : GalleryInterface("导航", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("NavigationViewInterface");

    auto pivot = new Pivot(this);
    pivot->addItem("1", "主页", FluentIcon(FluentIconType::HOME).qicon());
    pivot->addItem("2", "订阅", FluentIcon(FluentIconType::BOOK_SHELF).qicon());
    pivot->addItem("3", "历史", FluentIcon(FluentIconType::HISTORY).qicon());
    pivot->addItem("4", "设置", FluentIcon(FluentIconType::SETTING).qicon());

    addExampleCard("顶部导航栏", pivot);
}


