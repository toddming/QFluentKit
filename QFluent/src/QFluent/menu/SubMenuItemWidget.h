#pragma once

#include <QWidget>

class QMenu;
class RoundMenu;
class QEnterEvent;
class QPaintEvent;
class QListWidgetItem;
class SubMenuItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SubMenuItemWidget(QMenu* menu, QListWidgetItem* item, QWidget* parent = nullptr);
    explicit SubMenuItemWidget(RoundMenu* menu, QListWidgetItem* item, QWidget* parent = nullptr);

signals:
    void showMenuSig(QListWidgetItem* item);

protected:
    void enterEvent(QEnterEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QMenu* menuAsQMenu = nullptr;
    RoundMenu* menuAsRoundMenu = nullptr;
    QListWidgetItem* item = nullptr;
};
