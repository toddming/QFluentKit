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
    tabBar = new TabBar(this);
    tabBar->setMovable(true);          // 允许拖拽标签
    tabBar->setTabsClosable(true);     // 显示关闭按钮
    tabBar->setAddButtonVisible(true); // 显示添加按钮

    addTab();
    addTab();

    auto widget = new QWidget(this);
    auto tabView = new QWidget(widget);
    auto controlPanel = new QFrame(widget);
    controlPanel->setObjectName("controlPanel");
    controlPanel->setFixedWidth(220);
    auto movableCheckBox = new CheckBox("启用标签拖拽", widget);
    auto scrollableCheckBox = new CheckBox("启用标签滚动", widget);
    auto shadowEnabledCheckBox = new CheckBox("启用标签阴影", widget);
    auto tabMaxWidthLabel = new BodyLabel("标签最大宽度", widget);
    auto tabMaxWidthSpinBox = new SpinBox(widget);
    auto closeDisplayModeLabel = new BodyLabel("关闭按钮显示模式", widget);
    auto closeDisplayModeComboBox = new ComboBox(widget);
    movableCheckBox->setChecked(true);
    scrollableCheckBox->setChecked(true);
    shadowEnabledCheckBox->setChecked(true);
    auto hBoxLayout = new QHBoxLayout(widget);
    auto vBoxLayout = new QVBoxLayout(tabView);
    auto panelLayout = new QVBoxLayout(controlPanel);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(8);
    panelLayout->setContentsMargins(14, 16, 14, 14);
    panelLayout->setAlignment(Qt::AlignTop);

    vBoxLayout->addWidget(tabBar);
    vBoxLayout->addStretch();
    panelLayout->addWidget(movableCheckBox);
    panelLayout->addWidget(scrollableCheckBox);
    panelLayout->addWidget(shadowEnabledCheckBox);
    panelLayout->addSpacing(4);
    panelLayout->addWidget(tabMaxWidthLabel);
    panelLayout->addWidget(tabMaxWidthSpinBox);
    panelLayout->addSpacing(4);
    panelLayout->addWidget(closeDisplayModeLabel);
    panelLayout->addWidget(closeDisplayModeComboBox);

    hBoxLayout->addWidget(tabView, 1);
    hBoxLayout->addWidget(controlPanel, 0);

    connect(movableCheckBox, &CheckBox::clicked, this, [=](bool checked){
        tabBar->setMovable(checked);
    });
    connect(scrollableCheckBox, &CheckBox::clicked, this, [=](bool checked){
        tabBar->setScrollable(checked);
    });
    connect(shadowEnabledCheckBox, &CheckBox::clicked, this, [=](bool checked){
        tabBar->setTabShadowEnabled(checked);
    });

    auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/navigation_view_interface.qss");
    StyleSheetManager::instance()->registerWidget(styleSource, widget);

    connect(tabBar, &TabBar::tabAddRequested, this, [=](){ addTab(); });
    connect(tabBar, &TabBar::tabCloseRequested, this, [=](int index){ removeTab(index); });

    return widget;
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
