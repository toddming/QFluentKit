#include "CheckableMenu.h"

#include "MenuItemDelegate.h"
#include "MenuActionListWidget.h"

CheckableMenu::CheckableMenu(const QString &title, QWidget *parent,
                             Fluent::MenuIndicator indicatorType)
    : RoundMenu(title, parent)
{
    if (indicatorType == Fluent::MenuIndicator::CHECK) {
        view()->setItemDelegate(new CheckIndicatorMenuItemDelegate(this));
    } else {
        view()->setItemDelegate(new RadioIndicatorMenuItemDelegate(this));
    }
    view()->setObjectName("checkableListWidget");
}

int CheckableMenu::adjustItemText(QListWidgetItem *item, QAction *action)
{
    int w = RoundMenu::adjustItemText(item, action);
    item->setSizeHint(QSize(w + 26, itemHeight()));
    return w + 26;
}
