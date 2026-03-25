#pragma once

#include "RoundMenu.h"

class QFLUENT_EXPORT ComboBoxMenu : public RoundMenu
{
    Q_OBJECT

public:
    explicit ComboBoxMenu(const QString &title = QString(), QWidget *parent = nullptr);

    void exec(const QPoint &pos, bool animate = true,
              Fluent::MenuAnimation aniType = Fluent::MenuAnimation::DROP_DOWN) override;
};
