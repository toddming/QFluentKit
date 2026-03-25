#include "ComboBoxMenu.h"

#include "MenuActionListWidget.h"
#include "MenuItemDelegate.h"

ComboBoxMenu::ComboBoxMenu(const QString &title, QWidget *parent)
    : RoundMenu(title, parent)
{
    setItemHeight(36);
    view()->setItemDelegate(new IndicatorMenuItemDelegate(this));
    view()->setObjectName("comboListWidget");
}

void ComboBoxMenu::exec(const QPoint &pos, bool animate, Fluent::MenuAnimation aniType)
{
    view()->adjustSize(pos, aniType);
    adjustMenuSize();
    RoundMenu::exec(pos, animate, aniType);
}
