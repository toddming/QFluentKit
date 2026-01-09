#ifndef ACRYLICMENU_H
#define ACRYLICMENU_H

#include <QListWidget>
#include <QPainter>
#include <QPainterPath>
#include <QColor>
#include <QRectF>
#include <QPaintEvent>
#include <QSize>
#include <QMargins>
#include <QHBoxLayout>

#include "QFluent/Menu/MenuActionListWidget.h"
#include "AcrylicLabel.h"
#include "QFluent/Menu/RoundMenu.h"
#include "QFluent/Menu/MenuAnimation.h"
#include "FluentGlobal.h"
#include "Theme.h"

class QFLUENT_EXPORT AcrylicMenuActionListWidget : public MenuActionListWidget {
    Q_OBJECT
public:
    explicit AcrylicMenuActionListWidget(QWidget *parent = nullptr);

    void setItemHeight(int height);
    void addItem(QListWidgetItem *item);
    QListWidgetItem* createPlaceholderItem(int height = 2);
    QPainterPath clipPath() const;

    AcrylicBrush acrylicBrush;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateAcrylicColor();
    int topMargin() const;
    int bottomMargin() const;
};

class QFLUENT_EXPORT AcrylicMenu : public RoundMenu {
    Q_OBJECT
public:
    explicit AcrylicMenu(const QString &title = "", QWidget *parent = nullptr);

    void setItemHeight(int height);

    void setUpMenu(AcrylicMenuActionListWidget *view);
    void exec(const QPoint &pos, bool animate = true,
              Fluent::MenuAnimation aniType = Fluent::MenuAnimation::DROP_DOWN);

private:
    AcrylicMenuActionListWidget *listWidget;
};

#endif // ACRYLICMENU_H
