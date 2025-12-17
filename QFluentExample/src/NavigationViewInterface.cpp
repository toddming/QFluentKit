#include "NavigationViewInterface.h"

#include "FluentIcon.h"
#include "QFluent/Label.h"
#include "QFluent/SpinBox.h"
#include "QFluent/CheckBox.h"
#include "QFluent/ComboBox.h"

NavigationViewInterface::NavigationViewInterface(QWidget *parent)
    : GalleryInterface("导航", "", parent)
{
    setObjectName("NavigationViewInterface");


    addExampleCard("顶部导航栏", createPivotWidget(), "", 1);

    addExampleCard("标签导航", createTabWidget(), "", 1);
}

QWidget* NavigationViewInterface::createPivotWidget()
{
    auto pivotView = new QWidget(this);
    pivotView->setFixedHeight(200);

    pivotStacked = new StackedWidget(pivotView);
    pivotStacked->setStyleSheet(R"(
StackedWidget {
    border-right: none;
    border-bottom: none;
    border-top-left-radius: 10px;
    background-color: transparent;
    border: none;
}
)");

    pivot = new Pivot(pivotView);
    addPage("homePage", "主页", FluentIcon(Fluent::IconType::HOME));
    addPage("subscribePage", "订阅", FluentIcon(Fluent::IconType::BOOK_SHELF));
    addPage("historyPage", "历史", FluentIcon(Fluent::IconType::HISTORY));
    addPage("settingsPage", "设置", FluentIcon(Fluent::IconType::SETTING));

    connect(pivot, &Pivot::currentRouteKeyChanged, this, [=](const QString &routeKey){
        pivotStacked->setCurrentWidget(pivotStacked->findChild<QWidget *>(routeKey));
    });

    pivot->setCurrentItem("homePage");

    auto vBoxLayout = new QVBoxLayout(pivotView);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);

    vBoxLayout->addWidget(pivotView);
    vBoxLayout->addWidget(pivotStacked);

    return pivotView;
}

void NavigationViewInterface::addPage(const QString &routeKey, const QString &text, const FluentIconBase &icon)
{
    pivot->addItem(routeKey, text, icon);

    auto label = new TitleLabel(text, pivotStacked);
    label->setObjectName(routeKey);
    label->setAlignment(Qt::AlignCenter);
    pivotStacked->addWidget(label);
}

QWidget* NavigationViewInterface::createTabWidget()
{
    auto tabView = new QWidget(this);
    tabView->setMinimumHeight(200);

    tabStacked = new StackedWidget(tabView);
    tabStacked->setStyleSheet(R"(
StackedWidget {
    border-right: none;
    border-bottom: none;
    border-top-left-radius: 10px;
    background-color: transparent;
    border: none;
}
)");

    tabBar = new TabBar(tabView);
    tabBar->setMovable(true);         // 禁用拖拽标签
    tabBar->setTabsClosable(true);     // 显示关闭按钮
    tabBar->setAddButtonVisible(true); // 显示添加按钮
    tabBar->setScrollable(false);      // 禁用标签滚动
    tabBar->setTabShadowEnabled(true); // 启用标签阴影

    addTab();
    addTab();

    auto vBoxLayout = new QVBoxLayout(tabView);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);

    vBoxLayout->addWidget(tabBar);
    vBoxLayout->addWidget(tabStacked);

    connect(tabBar, &TabBar::tabAddRequested, this, [=](){ addTab(); });
    connect(tabBar, &TabBar::tabCloseRequested, this, [=](int index){ removeTab(index); });
    connect(tabBar, &TabBar::tabBarClicked, this, [=](int index){
        int key = tabBar->tabItem(index)->routeKey().toInt();
        tabStacked->setCurrentWidget(tabMap.value(key));
    });

    return tabView;
}

void NavigationViewInterface::addTab()
{
    int count = ++tabCount;
    tabBar->addTab(QString::number(count), QString("新建标签x%1").arg(count), QIcon(":/res/example.png"));

    auto label = new TitleLabel(QString("标签页x%1").arg(count), tabStacked);
    label->setAlignment(Qt::AlignCenter);
    tabStacked->addWidget(label);
    tabMap.insert(count, label);
}

void NavigationViewInterface::removeTab(int index)
{
    int key = tabBar->tabItem(index)->routeKey().toInt();
    tabStacked->removeWidget(tabMap.value(key));
    tabBar->removeTab(index);
}
