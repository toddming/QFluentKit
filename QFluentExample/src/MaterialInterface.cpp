#include "MaterialInterface.h"
#include <QWidget>
#include <QActionGroup>
#include "QFluent/Slider.h"
#include "QFluent/ProfileCard.h"
#include "QFluent/Material/AcrylicMenu.h"
#include "QFluent/Material/AcrylicCheckableMenu.h"

MaterialInterface::MaterialInterface(QWidget *parent)
    : GalleryInterface("材料", "", parent)
{
    setObjectName("MaterialInterface");

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

    menuAcrylicLabel = new MenuAcrylicLabel(this);
    menuAcrylicLabel->setImage(":/res/Mountain.png");
    menuAcrylicLabel->setFixedSize(535, 300);

    addExampleCard("亚克力标签(右键调整模糊半径)", menuAcrylicLabel);
    connect(menuAcrylicLabel, &MenuAcrylicLabel::mouseRightClicked, this, [=](const QPoint &pos){
        createSliderMenu(pos);
    });


    auto image1 = new MenuLabel(this);
    image1->setImage(":/res/Mountain.png");
    image1->scaledToHeight(300);
    image1->setBorderRadius(6, 6, 6, 6);
    addExampleCard("亚克力效果的圆角菜单(右键弹出菜单)", image1);
    connect(image1, &MenuLabel::mouseRightClicked, this, [=](const QPoint &pos){
        createMenu(pos);
    });

    auto image2 = new MenuLabel(this);
    image2->setImage(":/res/Mountain.png");
    image2->scaledToHeight(300);
    image2->setBorderRadius(6, 6, 6, 6);
    addExampleCard("亚克力效果的自定义组件菜单(右键弹出菜单)", image2);
    connect(image2, &MenuLabel::mouseRightClicked, this, [=](const QPoint &pos){
        createCustomWidgetMenu(pos);
    });


    auto image3 = new MenuLabel(this);
    image3->setImage(":/res/Mountain.png");
    image3->scaledToHeight(300);
    image3->setBorderRadius(6, 6, 6, 6);
    addExampleCard("亚克力效果的可选中菜单(右键弹出菜单)", image3);
    connect(image3, &MenuLabel::mouseRightClicked, this, [=](const QPoint &pos){
        createCheckableMenu(pos);
    });

}

void MaterialInterface::createMenu(QPoint pos)
{
    auto menu = new AcrylicMenu("menu", this);
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::COPY), "复制", this));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::CUT), "剪贴", this));

    auto submenu = new AcrylicMenu("添加到", this);
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

    menu->addWidget(card);
    menu->addSeparator();
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::PEOPLE), "管理账户和设置"));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::SHOPPING_CART), "支付方式"));
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::CODE), "兑换代码和礼品卡"));
    menu->addSeparator();
    menu->addAction(new Action(Fluent::icon(Fluent::IconType::SETTING), "设置"));

    menu->exec(pos);
}

void MaterialInterface::createSliderMenu(QPoint pos)
{
    auto menu = new RoundMenu("menu", this);

    auto widget = new QWidget(menu);
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 28, 0);
    widget->setFixedWidth(200);

    auto slider = new Slider(Qt::Horizontal, widget);
    slider->setRange(0, 40);
    slider->setValue(menuAcrylicLabel->blurRadius());
    layout->addWidget(slider);
    connect(slider, &Slider::valueChanged, this, [=](int value){
        menuAcrylicLabel->setBlurRadius(value);
        menuAcrylicLabel->setImage(":/res/Mountain.png");
    });

    menu->addWidget(widget);

    menu->exec(pos);
}


MenuLabel::MenuLabel(QWidget *parent)
    : ImageLabel(parent)
{

}

void MenuLabel::contextMenuEvent(QContextMenuEvent *event)
{
    emit mouseRightClicked(event->globalPos());
}

MenuAcrylicLabel::MenuAcrylicLabel(QWidget *parent)
    : AcrylicLabel(20,
                   QColor(105, 114, 168, 102),
                   QColor(255, 255, 255, 0),
                   QSize(535, 300),
                   parent)
{

}

void MenuAcrylicLabel::contextMenuEvent(QContextMenuEvent *event)
{
    emit mouseRightClicked(event->globalPos());
}
