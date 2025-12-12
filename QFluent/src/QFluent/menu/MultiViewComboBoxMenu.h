#pragma once

#include "RoundMenu.h"

class MultiViewComboBoxMenu : public RoundMenu
{
public:
    explicit MultiViewComboBoxMenu(const QString& title = "", QWidget *parent = nullptr);

    void exec(const QPoint& pos, bool animate = true,
              Fluent::MenuAnimation aniType = Fluent::MenuAnimation::DROP_DOWN);

protected:
    int adjustItemText(QListWidgetItem *item, QAction *action) override;
};
