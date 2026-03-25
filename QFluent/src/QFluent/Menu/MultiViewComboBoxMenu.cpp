#include "MultiViewComboBoxMenu.h"

#include "MenuActionListWidget.h"
#include "MenuItemDelegate.h"

MultiViewComboBoxMenu::MultiViewComboBoxMenu(const QString &title, QWidget *parent)
    : RoundMenu(title, parent)
{
    setItemHeight(36);
    setHideByClick(false);

    view()->setItemDelegate(new CheckIndicatorMenuItemDelegate(this));
    view()->setObjectName("multiListWidget");
}

void MultiViewComboBoxMenu::exec(const QPoint &pos, bool animate, Fluent::MenuAnimation aniType)
{
    view()->adjustSize(pos, aniType);
    adjustMenuSize();
    RoundMenu::exec(pos, animate, aniType);
}

int MultiViewComboBoxMenu::adjustItemText(QListWidgetItem *item, QAction *action)
{
    int w = RoundMenu::adjustItemText(item, action);
    item->setSizeHint(QSize(w + 26, itemHeight()));
    return w + 26;
}
