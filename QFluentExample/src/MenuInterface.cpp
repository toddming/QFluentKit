#include "MenuInterface.h"

#include <QWidget>
#include <QActionGroup>
#include "QFluent/PushButton.h"
#include "QFluent/ProfileCard.h"
#include "QFluent/menu/RoundMenu.h"
#include "QFluent/menu/CheckableMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"

MenuInterface::MenuInterface(QWidget *parent)
    : GalleryInterface("菜单", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("MenuInterface");

    createTimeAction = new Action(FluentIcon(FluentIconType::CALENDAR).qicon(), "创建日期", this);
    shootTimeAction = new Action(FluentIcon(FluentIconType::CAMERA).qicon(), "拍摄日期", this);
    modifiedTimeAction = new Action(FluentIcon(FluentIconType::EDIT).qicon(), "修改日期", this);
    nameAction = new Action(FluentIcon(FluentIconType::FONT).qicon(), "名称", this);
    createTimeAction->setCheckable(true);
    shootTimeAction->setCheckable(true);
    modifiedTimeAction->setCheckable(true);
    nameAction->setCheckable(true);

    auto actionGroup1 = new QActionGroup(this);
    actionGroup1->addAction(createTimeAction);
    actionGroup1->addAction(shootTimeAction);
    actionGroup1->addAction(modifiedTimeAction);
    actionGroup1->addAction(nameAction);

    ascendAction = new Action(FluentIcon(FluentIconType::UP).qicon(), "升序", this);
    descendAction = new Action(FluentIcon(FluentIconType::DOWN).qicon(), "降序", this);
    ascendAction->setCheckable(true);
    descendAction->setCheckable(true);

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
    menu->addAction(new Action(FluentIcon(FluentIconType::COPY).qicon(), "复制", this));
    menu->addAction(new Action(FluentIcon(FluentIconType::CUT).qicon(), "剪贴", this));

    RoundMenu *submenu = new RoundMenu("添加到", this);
    submenu->setIcon(FluentIcon(FluentIconType::ADD).qicon());
    submenu->addAction(new Action(FluentIcon(FluentIconType::VIDEO).qicon(), "视频"));
    submenu->addAction(new Action(FluentIcon(FluentIconType::MUSIC).qicon(), "音乐"));
    menu->addMenu(submenu);

    menu->addAction(new Action(FluentIcon(FluentIconType::PASTE).qicon(), "粘贴"));
    menu->addAction(new Action(FluentIcon(FluentIconType::CANCEL).qicon(), "撤回"));
    menu->addSeparator();
    menu->addAction(new Action(FluentIcon(FluentIconType::SETTING).qicon(), "设置"));
    menu->addAction(new Action(FluentIcon(FluentIconType::HELP).qicon(), "帮助"));
    menu->addAction(new Action(FluentIcon(FluentIconType::FEEDBACK).qicon(), "反馈"));
    menu->addAction(new Action(FluentIcon(FluentIconType::NONE).qicon(), "全选"));

    menu->exec(pos);
}

void MenuInterface::createCheckableMenu(QPoint pos)
{
    CheckableMenu *menu = new CheckableMenu("menu", this, MenuIndicatorType::MenuIndicator::RADIO);
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
    auto card = new ProfileCard(":/res/app/avatar.png", "硝子酱", "shokokawaii@outlook.com", menu);
    menu->setItemHeight(36);
    menu->view()->setMaxVisibleItems(0);
    menu->addWidget(card);
    menu->addSeparator();
    menu->addAction(new Action(FluentIcon(FluentIconType::PEOPLE).qicon(), "管理账户和设置"));
    menu->addAction(new Action(FluentIcon(FluentIconType::SHOPPING_CART).qicon(), "支付方式"));
    menu->addAction(new Action(FluentIcon(FluentIconType::CODE).qicon(), "兑换代码和礼品卡"));
    menu->addSeparator();
    menu->addAction(new Action(FluentIcon(FluentIconType::SETTING).qicon(), "设置"));

    menu->exec(pos);
}
