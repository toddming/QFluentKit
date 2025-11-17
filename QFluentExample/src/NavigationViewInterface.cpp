#include "NavigationViewInterface.h"

#include "FluentIcon.h"
#include "QFluent/Label.h"
#include "QFluent/SpinBox.h"
#include "QFluent/TabView.h"
#include "QFluent/CheckBox.h"
#include "QFluent/ComboBox.h"
#include "QFluent/navigation/Pivot.h"

NavigationViewInterface::NavigationViewInterface(QWidget *parent)
    : GalleryInterface("导航", "", parent)
{
    setObjectName("NavigationViewInterface");

    auto pivot = new Pivot(this);
    pivot->addItem("1", "主页", FluentIcon(FluentIconType::HOME).qicon());
    pivot->addItem("2", "订阅", FluentIcon(FluentIconType::BOOK_SHELF).qicon());
    pivot->addItem("3", "历史", FluentIcon(FluentIconType::HISTORY).qicon());
    pivot->addItem("4", "设置", FluentIcon(FluentIconType::SETTING).qicon());

    addExampleCard("顶部导航栏", pivot);

    addExampleCard("标签导航", createTabWidget());
}

QWidget* NavigationViewInterface::createTabWidget()
{
    TabBar* tabBar = new TabBar(this);
    // tabBar->setMinimumHeight(80);
    tabBar->setMovable(true);          // 允许拖拽标签
    tabBar->setTabsClosable(true);     // 显示关闭按钮
    tabBar->setAddButtonVisible(true); // 显示添加按钮

    // 基本添加
    tabBar->addTab("1", "标签文字x1", QIcon(":/res/example.png"));
    tabBar->addTab("2", "标签文字x2", QIcon(":/res/example.png"));

    auto tabView = new QWidget(this);
    auto controlPanel = new QFrame(this);
    controlPanel->setFixedWidth(220);
    auto movableCheckBox = new CheckBox("启用标签拖拽", controlPanel);
    auto scrollableCheckBox = new CheckBox("启用标签滚动", controlPanel);
    auto shadowEnabledCheckBox = new CheckBox("启用标签阴影", controlPanel);
    auto tabMaxWidthLabel = new BodyLabel("标签最大宽度", controlPanel);
    auto tabMaxWidthSpinBox = new SpinBox(controlPanel);
    auto closeDisplayModeLabel = new BodyLabel("关闭按钮显示模式", controlPanel);
    auto closeDisplayModeComboBox = new ComboBox(this);

    auto hBoxLayout = new QHBoxLayout(tabView);
    auto vBoxLayout = new QVBoxLayout();
    auto panelLayout = new QVBoxLayout(controlPanel);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(8);
    panelLayout->setContentsMargins(14, 16, 14, 14);
    panelLayout->setAlignment(Qt::AlignTop);

    vBoxLayout->addWidget(tabBar);
    vBoxLayout->addStretch();
    hBoxLayout->addLayout(vBoxLayout, 1);
    hBoxLayout->addWidget(controlPanel, 0);
    panelLayout->addWidget(movableCheckBox);
    panelLayout->addWidget(scrollableCheckBox);
    panelLayout->addWidget(shadowEnabledCheckBox);
    panelLayout->addSpacing(4);
    panelLayout->addWidget(tabMaxWidthLabel);
    panelLayout->addWidget(tabMaxWidthSpinBox);
    panelLayout->addSpacing(4);
    panelLayout->addWidget(closeDisplayModeLabel);
    panelLayout->addWidget(closeDisplayModeComboBox);

    connect(movableCheckBox, &CheckBox::clicked, this, [=](bool checked){
        tabBar->setMovable(checked);
    });
    connect(scrollableCheckBox, &CheckBox::clicked, this, [=](bool checked){
        tabBar->setScrollable(checked);
    });
    connect(shadowEnabledCheckBox, &CheckBox::clicked, this, [=](bool checked){
        tabBar->setTabShadowEnabled(checked);
    });

    return tabView;
}
