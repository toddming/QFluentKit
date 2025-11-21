#include "NavigationViewInterface.h"

#include "FluentIcon.h"
#include "QFluent/Label.h"
#include "QFluent/SpinBox.h"
#include "QFluent/CheckBox.h"
#include "QFluent/ComboBox.h"
#include "QFluent/navigation/Pivot.h"

NavigationViewInterface::NavigationViewInterface(QWidget *parent)
    : GalleryInterface("导航", "", parent)
{
    setObjectName("NavigationViewInterface");

    auto pivot = new Pivot(this);
    pivot->addItem("1", "主页", FluentIcon(Fluent::IconType::HOME).qicon());
    pivot->addItem("2", "订阅", FluentIcon(Fluent::IconType::BOOK_SHELF).qicon());
    pivot->addItem("3", "历史", FluentIcon(Fluent::IconType::HISTORY).qicon());
    pivot->addItem("4", "设置", FluentIcon(Fluent::IconType::SETTING).qicon());

    addExampleCard("顶部导航栏", pivot);

    addExampleCard("标签导航", createTabWidget(), "", 1);
}

QWidget* NavigationViewInterface::createTabWidget()
{
    auto tabView = new QWidget(this);
    tabView->setMinimumHeight(200);

    tabBar = new TabBar(tabView);
    tabBar->setMovable(false);         // 禁用拖拽标签
    tabBar->setTabsClosable(true);     // 显示关闭按钮
    tabBar->setAddButtonVisible(true); // 显示添加按钮
    tabBar->setScrollable(false);      // 禁用标签滚动
    tabBar->setTabShadowEnabled(true); // 启用标签阴影

    addTab();
    addTab();

    auto vBoxLayout = new QVBoxLayout(tabView);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);

    vBoxLayout->addWidget(tabBar);
    vBoxLayout->addStretch();

    // auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/navigation_view_interface.qss");
    // StyleSheetManager::instance()->registerWidget(styleSource, widget);

    connect(tabBar, &TabBar::tabAddRequested, this, [=](){ addTab(); });
    connect(tabBar, &TabBar::tabCloseRequested, this, [=](int index){ removeTab(index); });

    return tabView;
}

void NavigationViewInterface::addTab()
{
    int count = ++tabCount;
    tabBar->addTab(QString::number(count), QString("新建标签x%1").arg(count), QIcon(":/res/example.png"));
}

void NavigationViewInterface::removeTab(int index)
{
    tabBar->removeTab(index);
}
