#include "BasicInputInterface.h"

#include "QFluent/CheckBox.h"
#include "QFluent/ToolButton.h"
#include "QFluent/PushButton.h"

BasicInputInterface::BasicInputInterface(QWidget *parent)
    : GalleryInterface("基本输入", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("basicInputInterface");

    addExampleCard("带有文本的简单按钮", new PushButton("标准按钮", this));

    auto button = new ToolButton(QIcon(":/res/kunkun.png"), this);
    button->setIconSize(QSize(40, 40));
    button->resize(70, 70);
    addExampleCard("带有图标的按钮", button);

    addExampleCard("主题色按钮", new PrimaryPushButton("主题色按钮", this));
    addExampleCard("主题色工具按钮", new PrimaryToolButton(FluentIconType::IconType::BASKETBALL, this));
    addExampleCard("椭圆按钮", new PillPushButton("标签", this, FluentIconType::IconType::TAG));
    addExampleCard("椭圆工具按钮", new PillToolButton(FluentIconType::IconType::BASKETBALL, this));
    addExampleCard("透明按钮", new TransparentPushButton("透明按钮", this, FluentIconType::BOOK_SHELF));
    addExampleCard("透明按钮", new TransparentToolButton(FluentIconType::BOOK_SHELF, this));
    addExampleCard("双态复选框", new CheckBox("双态复选框", this));
}
