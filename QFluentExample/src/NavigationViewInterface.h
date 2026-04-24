#pragma once

#include "GalleryInterface.h"
#include "QFluent/TabBar.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/Navigation/Pivot.h"

class NavigationViewInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit NavigationViewInterface(QWidget *parent = nullptr);

private:
    QWidget* createPivotWidget();

    QWidget* createTabWidget();

    void addPage(const QString &routeKey, const QString &text, const QIcon &icon);

    void addTab();
    void removeTab(int index);

    Pivot *pivot;
    TabBar* tabBar;
    StackedWidget* pivotStacked;
    StackedWidget* tabStacked;

    QMap<int, QWidget*> tabMap;

    int tabCount = 0;
};
