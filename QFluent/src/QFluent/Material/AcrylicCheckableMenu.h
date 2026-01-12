#pragma once

#include "QFluent/Menu/RoundMenu.h"
#include "FluentGlobal.h"
#include "AcrylicLabel.h"
#include "AcrylicMenu.h"


class QFLUENT_EXPORT AcrylicCheckableMenu : public RoundMenu {
    Q_OBJECT

public:
    explicit AcrylicCheckableMenu(const QString& title = "", QWidget* parent = nullptr,
                                  Fluent::MenuIndicator indicatorType = Fluent::MenuIndicator::CHECK);

    void setItemHeight(int height);

    void setUpMenu(AcrylicMenuActionListWidget *view);
    void exec(const QPoint &pos, bool animate = true,
              Fluent::MenuAnimation aniType = Fluent::MenuAnimation::DROP_DOWN);

protected:
    int adjustItemText(QListWidgetItem *item, QAction *action) override;

private:
    AcrylicMenuActionListWidget *listWidget;
};
