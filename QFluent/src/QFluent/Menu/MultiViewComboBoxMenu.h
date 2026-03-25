#pragma once

#include "RoundMenu.h"

class QAction;
class QListWidgetItem;

class QFLUENT_EXPORT MultiViewComboBoxMenu : public RoundMenu
{
    Q_OBJECT

public:
    explicit MultiViewComboBoxMenu(const QString &title = QString(), QWidget *parent = nullptr);

    void exec(const QPoint &pos, bool animate = true,
              Fluent::MenuAnimation aniType = Fluent::MenuAnimation::DROP_DOWN) override;

protected:
    int adjustItemText(QListWidgetItem *item, QAction *action) override;
};
