#include "MenuInterface.h"

#include <QWidget>
#include <QActionGroup>
#include "QFluent/PushButton.h"
#include "QFluent/ProfileCard.h"
#include "QFluent/Menu/RoundMenu.h"
#include "QFluent/Menu/CheckableMenu.h"
#include "QFluent/Menu/MenuActionListWidget.h"

MenuInterface::MenuInterface(QWidget *parent)
    : GalleryInterface("菜单", "", parent)
{
    setObjectName("MenuInterface");

    createTimeAction = new Action(Fluent::icon(Fluent::IconType::CALENDAR), "创建日期", this);
    shootTimeAction = new Action(Fluent::icon(Fluent::IconType::CAMERA), "拍摄日期", this);
    modifiedTimeAction = new Action(Fluent::icon(Fluent::IconType::EDIT), "修改日期", this);
    nameAction = new Action(Fluent::icon(Fluent::IconType::FONT), "名称", this);
    createTimeAction->setCheckable(true);
    shootTimeAction->setCheckable(true);
    modifiedTimeAction->setCheckable(true);
    nameAction->setCheckable(true);
    createTimeAction->setChecked(true);

    auto actionGroup1 = new QActionGroup(this);
    actionGroup1->addAction(createTimeAction);
    actionGroup1->addAction(shootTimeAction);
    actionGroup1->addAction(modifiedTimeAction);
    actionGroup1->addAction(nameAction);

    ascendAction = new Action(Fluent::icon(Fluent::IconType::UP), "升序", this);
    descendAction = new Action(Fluent::icon(Fluent::IconType::DOWN), "降序", this);
    ascendAction->setCheckable(true);
    descendAction->setCheckable(true);
    ascendAction->setChecked(true);

    auto actionGroup2 = new QActionGroup(this);
    actionGroup2->addAction(ascendAction);
    actionGroup2->addAction(descendAction);

    auto btn1 = new PushButton("显示菜单", this);
    connect(btn1, &PushButton::clicked, this, [=](){
        createMenu(btn1->mapToGlobal(QPoint(btn1->width() + 5, -100)));
    });

    addExampleCard("圆角菜单", btn1);

    auto btn2 = new PushButton("显示菜单", this);
    connect(btn2, &PushButton::clicked, this, [=](){
        createCustomWidgetMenu(btn2->mapToGlobal(QPoint(btn2->width() + 5, -100)));
    });

    addExampleCard("自定义组件菜单", btn2);

    auto btn3 = new PushButton("显示菜单", this);
    connect(btn3, &PushButton::clicked, this, [=](){
        createCheckableMenu(btn3->mapToGlobal(QPoint(btn3->width() + 5, -100)));
    });

    addExampleCard("可选中菜单", btn3);
}

void MenuInterface::createMenu(QPoint pos)
{
    RoundMenu *menu = new RoundMenu("menu", this);
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::COPY), "复制", this));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::CUT), "剪贴", this));

    RoundMenu *submenu = new RoundMenu("添加到", this);
    submenu->setIcon(Fluent::icon(Fluent::IconType::ADD));
    submenu->addAction(new Action(Fluent::icon(Fluent::IconType::VIDEO), "视频"));
    submenu->addAction(new Action(Fluent::icon(Fluent::IconType::MUSIC), "音乐"));
    menu->addMenu(submenu);

    menu->addAction(new Action(Fluent::icon(Fluent::IconType::PASTE), "粘贴"));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::CANCEL), "撤回"));
    menu->addSeparator();
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::SETTING), "设置"));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::HELP), "帮助"));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::FEEDBACK), "反馈"));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::NONE), "全选"));

    menu->exec(pos);
}

void MenuInterface::createCheckableMenu(QPoint pos)
{
    CheckableMenu *menu = new CheckableMenu("menu", this, Fluent::MenuIndicator::RADIO);
    menu->addAction(createTimeAction);
    menu->addAction(shootTimeAction);
    menu->addAction(modifiedTimeAction);
    menu->addAction(nameAction);
    menu->addSeparator();

    menu->addAction(ascendAction);
    menu->addAction(descendAction);
    menu->exec(pos);
}

void MenuInterface::createCustomWidgetMenu(QPoint pos)
{
    auto menu = new RoundMenu("menu", this);
    auto card = new ProfileCard(":/res/Shizuka.png", "源静香", "shizuka@gmail.com", menu);
    menu->setItemHeight(36);

    menu->addWidget(card);
    menu->addSeparator();
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::PEOPLE), "管理账户和设置"));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::SHOPPING_CART), "支付方式"));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::CODE), "兑换代码和礼品卡"));
    menu->addSeparator();
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::SETTING), "设置"));

    menu->exec(pos);
}
