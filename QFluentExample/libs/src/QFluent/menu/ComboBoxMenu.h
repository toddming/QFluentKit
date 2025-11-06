#pragma once

#include "RoundMenu.h"

class ComboBoxMenu : public RoundMenu
{
public:
    explicit ComboBoxMenu(const QString& title = "", QWidget *parent = nullptr);

    void exec(const QPoint& pos, bool animate = true,
              MenuAnimationType::MenuAnimation aniType = MenuAnimationType::MenuAnimation::DROP_DOWN);

};
