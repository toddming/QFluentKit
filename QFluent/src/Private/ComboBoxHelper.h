#pragma once

#include <QPoint>

class QWidget;
class RoundMenu;

namespace ComboBoxHelper {

void showComboMenu(QWidget *parent, RoundMenu *menu, int maxVisibleItems);
void closeComboMenu(RoundMenu *menu);
void toggleComboMenu(QWidget *parent, RoundMenu *menu, int maxVisibleItems);

} // namespace ComboBoxHelper
