#include "BasicInputInterface.h"

#include <QButtonGroup>
#include <QVBoxLayout>

#include "FluentIcon.h"
#include "QFluent/CheckBox.h"
#include "QFluent/ComboBox.h"
#include "QFluent/ToolButton.h"
#include "QFluent/PushButton.h"
#include "QFluent/RadioButton.h"
#include "QFluent/SwitchButton.h"
#include "QFluent/EditableComboBox.h"

BasicInputInterface::BasicInputInterface(QWidget *parent)
    : GalleryInterface("基本输入", "", parent)
{
    setObjectName("basicInputInterface");

    addExampleCard("带有文本的简单按钮", new PushButton("标准按钮", this));

    auto button = new ToolButton(QIcon(":/res/Slices.png"), this);
    button->setIconSize(QSize(40, 40));
    button->resize(70, 70);
    addExampleCard("带有图标的按钮", button);

    addExampleCard("主题色按钮", new PrimaryPushButton("主题色按钮", this));
    addExampleCard("主题色工具按钮", new PrimaryToolButton(FluentIcon(Fluent::IconType::BASKETBALL), this));
    addExampleCard("椭圆按钮", new PillPushButton("标签", FluentIcon(Fluent::IconType::TAG), this));
    addExampleCard("椭圆工具按钮", new PillToolButton(FluentIcon(Fluent::IconType::BASKETBALL), this));
    addExampleCard("透明按钮", new TransparentPushButton("透明按钮", FluentIcon(Fluent::IconType::BOOK_SHELF), this));
    addExampleCard("透明按钮", new TransparentToolButton(FluentIcon(Fluent::IconType::BOOK_SHELF), this));
    addExampleCard("双态复选框", new CheckBox("双态复选框", this));

    auto checkBox = new CheckBox("三态复选框", this);
    checkBox->setTristate(true);
    addExampleCard("三态复选框", checkBox);

    auto comboBox = new ComboBox(this);
    comboBox->addItems({"请选择你的项目😍", "请选择你的项目😘", "请选择你的项目😜"});
    comboBox->setCurrentIndex(0);
    comboBox->setMinimumWidth(210);
    addExampleCard("下拉框", comboBox);

    auto editableComboBox = new EditableComboBox(this);
    editableComboBox->setPlaceholderText("可编辑的下拉框");
    editableComboBox->addItems({"可编辑的项目_1" , "可编辑的项目_2", "可编辑的项目_3", "可编辑的项目_4"});
    editableComboBox->setMinimumWidth(210);
    addExampleCard("可编辑的下拉框", editableComboBox);

    auto dropDownPushButtonMenu = new RoundMenu("menu", this);
    dropDownPushButtonMenu->addAction(new Action(FluentIcon(Fluent::IconType::SEND).qicon(), "发送"));
    dropDownPushButtonMenu->addAction(new Action(FluentIcon(Fluent::IconType::EDUCATION).qicon(), "保存"));

    auto dropDownPushButton = new DropDownPushButton("邮件", FluentIcon(Fluent::IconType::MAIL), this);
    dropDownPushButton->setMenu(dropDownPushButtonMenu);
    addExampleCard("带下拉菜单的按钮", dropDownPushButton);

    auto drowDownToolButton = new DropDownToolButton(FluentIcon(Fluent::IconType::MAIL), this);
    drowDownToolButton->setMenu(dropDownPushButtonMenu);
    addExampleCard("带下拉菜单的工具按钮", drowDownToolButton);

    auto primaryDropDownPushButton = new PrimaryDropDownPushButton("邮件", FluentIcon(Fluent::IconType::MAIL), this);
    primaryDropDownPushButton->setMenu(dropDownPushButtonMenu);
    addExampleCard("带下拉菜单的主题色按钮", primaryDropDownPushButton);

    auto primaryDropDownToolButton = new PrimaryDropDownToolButton(FluentIcon(Fluent::IconType::MAIL), this);
    primaryDropDownToolButton->setMenu(dropDownPushButtonMenu);
    addExampleCard("带下拉菜单的主题色工具按钮", primaryDropDownToolButton);

    auto transparentDropDownPushButton = new TransparentDropDownPushButton("邮件", FluentIcon(Fluent::IconType::MAIL), this);
    transparentDropDownPushButton->setMenu(dropDownPushButtonMenu);
    addExampleCard("带下拉菜单的透明按钮", transparentDropDownPushButton);

    auto transparentDropDownToolButton = new TransparentDropDownToolButton(FluentIcon(Fluent::IconType::MAIL), this);
    transparentDropDownToolButton->setMenu(dropDownPushButtonMenu);
    addExampleCard("带下拉菜单的透明工具按钮", transparentDropDownToolButton);

    auto hyperlinkButton = new HyperlinkButton("GitHub", FluentIcon(Fluent::IconType::LINK), this);
    addExampleCard("导航到一个超链接的按钮", hyperlinkButton);

    auto radioWidget = new QWidget();
    auto radioLayout = new QVBoxLayout(radioWidget);
    radioLayout->setContentsMargins(2, 0, 0, 0);
    radioLayout->setSpacing(15);
    auto radioBtn1 = new RadioButton("单选按钮_1", radioWidget);
    auto radioBtn2 = new RadioButton("单选按钮_2", radioWidget);
    auto radioBtn3 = new RadioButton("单选按钮_3", radioWidget);
    auto btnGroup = new QButtonGroup(radioWidget);
    btnGroup->addButton(radioBtn1);
    btnGroup->addButton(radioBtn2);
    btnGroup->addButton(radioBtn3);
    radioLayout->addWidget(radioBtn1);
    radioLayout->addWidget(radioBtn2);
    radioLayout->addWidget(radioBtn3);
    radioBtn1->click();
    addExampleCard("单选按钮", radioWidget);

    addExampleCard("开关按钮", new SwitchButton(this));
    addExampleCard("状态开关按钮", new TransparentTogglePushButton("开始练习", FluentIcon(Fluent::IconType::BASKETBALL), this));
    addExampleCard("状态开关工具按钮", new TransparentToggleToolButton(FluentIcon(Fluent::IconType::BASKETBALL), this));

    addExampleCard("测试按钮", new ToolButton(FluentIcon(Fluent::IconType::HELP), this));
}
