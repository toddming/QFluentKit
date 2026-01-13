#pragma once

#include "RoundMenu.h"

class QAction;
class QListWidgetItem;
class QFLUENT_EXPORT CheckableMenu : public RoundMenu {
    Q_OBJECT

public:
    explicit CheckableMenu(const QString& title = "", QWidget* parent = nullptr,
                           Fluent::MenuIndicator indicatorType = Fluent::MenuIndicator::CHECK);

protected:
    int adjustItemText(QListWidgetItem *item, QAction *action) override;
};
