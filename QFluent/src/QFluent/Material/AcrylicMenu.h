#pragma once

#include "QFluent/Menu/MenuActionListWidget.h"
#include "QFluent/Menu/RoundMenu.h"
#include "FluentGlobal.h"
#include "AcrylicLabel.h"

class QPainterPath;

class QFLUENT_EXPORT AcrylicMenuActionListWidget : public MenuActionListWidget
{
    Q_OBJECT

public:
    explicit AcrylicMenuActionListWidget(QWidget *parent = nullptr);

    void setItemHeight(int height);
    void addItem(QListWidgetItem *item) override;
    QListWidgetItem* createPlaceholderItem(int height = 2);
    QPainterPath clipPath() const;

    AcrylicBrush& acrylicBrush() { return m_acrylicBrush; }
    const AcrylicBrush& acrylicBrush() const { return m_acrylicBrush; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateAcrylicColor();
    int topMargin() const;
    int bottomMargin() const;
    AcrylicBrush m_acrylicBrush;
};

class QFLUENT_EXPORT AcrylicMenu : public RoundMenu
{
    Q_OBJECT

public:
    explicit AcrylicMenu(const QString &title = QString(), QWidget *parent = nullptr);

    void setItemHeight(int height);

    void setUpMenu(AcrylicMenuActionListWidget *view);
    void exec(const QPoint &pos, bool animate = true,
              Fluent::MenuAnimation aniType = Fluent::MenuAnimation::DROP_DOWN) override;

private:
    AcrylicMenuActionListWidget *m_listWidget;
};
