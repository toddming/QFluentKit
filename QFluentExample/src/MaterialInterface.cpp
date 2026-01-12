#include "MaterialInterface.h"
#include <QWidget>
#include <QActionGroup>

#include "QFluent/ProfileCard.h"
#include "QFluent/Material/AcrylicMenu.h"
#include "QFluent/Material/AcrylicCheckableMenu.h"

MaterialInterface::MaterialInterface(QWidget *parent)
    : GalleryInterface("材料", "", parent)
{
    setObjectName("MaterialInterface");

    createTimeAction = new Action(FluentIcon(Fluent::IconType::CALENDAR).qicon(), "创建日期", this);
    shootTimeAction = new Action(FluentIcon(Fluent::IconType::CAMERA).qicon(), "拍摄日期", this);
    modifiedTimeAction = new Action(FluentIcon(Fluent::IconType::EDIT).qicon(), "修改日期", this);
    nameAction = new Action(FluentIcon(Fluent::IconType::FONT).qicon(), "名称", this);
    createTimeAction->setCheckable(true);
    shootTimeAction->setCheckable(true);
    modifiedTimeAction->setCheckable(true);
    nameAction->setCheckable(true);

    auto actionGroup1 = new QActionGroup(this);
    actionGroup1->addAction(createTimeAction);
    actionGroup1->addAction(shootTimeAction);
    actionGroup1->addAction(modifiedTimeAction);
    actionGroup1->addAction(nameAction);

    ascendAction = new Action(FluentIcon(Fluent::IconType::UP).qicon(), "升序", this);
    descendAction = new Action(FluentIcon(Fluent::IconType::DOWN).qicon(), "降序", this);
    ascendAction->setCheckable(true);
    descendAction->setCheckable(true);

    auto actionGroup2 = new QActionGroup(this);
    actionGroup2->addAction(ascendAction);
    actionGroup2->addAction(descendAction);

    auto image1 = new MenuLabel(this);
    image1->setImage(":/res/Image10.jpg");
    image1->scaledToHeight(280);
    image1->setBorderRadius(6, 6, 6, 6);
    addExampleCard("亚克力效果的圆角菜单", image1);
    connect(image1, &MenuLabel::mouseRightClicked, this, [=](const QPoint &pos){
        createMenu(pos);
    });

    auto image2 = new MenuLabel(this);
    image2->setImage(":/res/Image10.jpg");
    image2->scaledToHeight(280);
    image2->setBorderRadius(6, 6, 6, 6);
    addExampleCard("亚克力效果的自定义组件菜单", image2);
    connect(image2, &MenuLabel::mouseRightClicked, this, [=](const QPoint &pos){
        createCustomWidgetMenu(pos);
    });


    auto image3 = new MenuLabel(this);
    image3->setImage(":/res/Image10.jpg");
    image3->scaledToHeight(280);
    image3->setBorderRadius(6, 6, 6, 6);
    addExampleCard("亚克力效果的可选中菜单", image3);
    connect(image3, &MenuLabel::mouseRightClicked, this, [=](const QPoint &pos){
        createCheckableMenu(pos);
    });

}

void MaterialInterface::createMenu(QPoint pos)
{
    auto menu = new AcrylicMenu("menu", this);
    menu->addAction(new Action(FluentIcon(Fluent::IconType::COPY).qicon(), "复制", this));
    menu->addAction(new Action(FluentIcon(Fluent::IconType::CUT).qicon(), "剪贴", this));

    auto submenu = new AcrylicMenu("添加到", this);
    submenu->setIcon(FluentIcon(Fluent::IconType::ADD).qicon());
    submenu->addAction(new Action(FluentIcon(Fluent::IconType::VIDEO).qicon(), "视频"));
    submenu->addAction(new Action(FluentIcon(Fluent::IconType::MUSIC).qicon(), "音乐"));
    menu->addMenu(submenu);

    menu->addAction(new Action(FluentIcon(Fluent::IconType::PASTE).qicon(), "粘贴"));
    menu->addAction(new Action(FluentIcon(Fluent::IconType::CANCEL).qicon(), "撤回"));
    menu->addSeparator();
    menu->addAction(new Action(FluentIcon(Fluent::IconType::SETTING).qicon(), "设置"));
    menu->addAction(new Action(FluentIcon(Fluent::IconType::HELP).qicon(), "帮助"));
    menu->addAction(new Action(FluentIcon(Fluent::IconType::FEEDBACK).qicon(), "反馈"));
    menu->addAction(new Action(FluentIcon(Fluent::IconType::NONE).qicon(), "全选"));

    menu->exec(pos);
}

void MaterialInterface::createCheckableMenu(QPoint pos)
{
    auto menu = new AcrylicCheckableMenu("menu", this, Fluent::MenuIndicator::RADIO);
    menu->addAction(createTimeAction);
    menu->addAction(shootTimeAction);
    menu->addAction(modifiedTimeAction);
    menu->addAction(nameAction);
    menu->addSeparator();

    menu->addAction(ascendAction);
    menu->addAction(descendAction);
    menu->exec(pos);
}

void MaterialInterface::createCustomWidgetMenu(QPoint pos)
{
    auto menu = new AcrylicMenu("menu", this);
    auto card = new ProfileCard(":/res/Shizuka.png", "源静香", "shizuka@gmail.com", menu);
    menu->setItemHeight(36);
    menu->view()->setMaxVisibleItems(0);

    menu->addWidget(card);
    menu->addSeparator();
    menu->addAction(new Action(FluentIcon(Fluent::IconType::PEOPLE).qicon(), "管理账户和设置"));
    menu->addAction(new Action(FluentIcon(Fluent::IconType::SHOPPING_CART).qicon(), "支付方式"));
    menu->addAction(new Action(FluentIcon(Fluent::IconType::CODE).qicon(), "兑换代码和礼品卡"));
    menu->addSeparator();
    menu->addAction(new Action(FluentIcon(Fluent::IconType::SETTING).qicon(), "设置"));

    menu->exec(pos, true, Fluent::MenuAnimation::DROP_DOWN);
}



MenuLabel::MenuLabel(QWidget *parent)
    : ImageLabel(parent)
{

}

void MenuLabel::contextMenuEvent(QContextMenuEvent *event)
{
    emit mouseRightClicked(event->globalPos());
}
