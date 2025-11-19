#include "CheckableMenu.h"

#include "MenuItemDelegate.h"
#include "MenuActionListWidget.h"

CheckableMenu::CheckableMenu(const QString &title, QWidget *parent,
                             Fluent::MenuIndicator indicatorType)
    : RoundMenu(title, parent)
{
    if (indicatorType == Fluent::MenuIndicator::CHECK) {
        this->view()->setItemDelegate(new CheckIndicatorMenuItemDelegate(this));
    } else {
        this->view()->setItemDelegate(new RadioIndicatorMenuItemDelegate(this));
    }
    this->view()->setObjectName("checkableListWidget");
}

int CheckableMenu::adjustItemText(QListWidgetItem *item, QAction *action)
{
    int w = RoundMenu::adjustItemText(item, action);
    item->setSizeHint(QSize(w + 26, this->itemHeight()));
    return w + 26;
}
