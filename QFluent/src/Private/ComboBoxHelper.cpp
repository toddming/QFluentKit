#include "ComboBoxHelper.h"
#include "QFluent/Menu/RoundMenu.h"
#include "QFluent/Menu/MenuActionListWidget.h"
#include "FluentGlobal.h"

namespace ComboBoxHelper {

void showComboMenu(QWidget *parent, RoundMenu *menu, int maxVisibleItems)
{
    if (maxVisibleItems > 0) {
        menu->setMaxVisibleItems(maxVisibleItems);
    }

    if (menu->view()->width() < parent->width()) {
        menu->view()->setMinimumWidth(parent->width());
        menu->adjustMenuSize();
    }

    int x = -menu->width() / 2 + menu->hBoxLayout()->contentsMargins().left() + parent->width() / 2;
    QPoint pd = parent->mapToGlobal(QPoint(x, parent->height()));
    int hd = menu->view()->heightForAnimation(pd, Fluent::MenuAnimation::DROP_DOWN);

    QPoint pu = parent->mapToGlobal(QPoint(x, 0));
    int hu = menu->view()->heightForAnimation(pu, Fluent::MenuAnimation::PULL_UP);

    if (hd >= hu) {
        menu->view()->adjustSize(pd, Fluent::MenuAnimation::DROP_DOWN);
        menu->exec(pd, true, Fluent::MenuAnimation::DROP_DOWN);
    } else {
        menu->view()->adjustSize(pu, Fluent::MenuAnimation::PULL_UP);
        menu->exec(pu, true, Fluent::MenuAnimation::PULL_UP);
    }
}

void closeComboMenu(RoundMenu *menu)
{
    if (!menu)
        return;
}

void toggleComboMenu(QWidget *parent, RoundMenu *menu, int maxVisibleItems)
{
    if (menu) {
        closeComboMenu(menu);
    } else {
        showComboMenu(parent, menu, maxVisibleItems);
    }
}

} // namespace ComboBoxHelper