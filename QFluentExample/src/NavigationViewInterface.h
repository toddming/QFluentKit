#pragma once

#include "GalleryInterface.h"
#include "QFluent/TabBar.h"
#include "QFluent/StackedWidget.h"

class NavigationViewInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit NavigationViewInterface(QWidget *parent = nullptr);

private:
    QWidget* createTabWidget();

    void addTab();
    void removeTab(int index);

    TabBar* tabBar;
    StackedWidget* stacked;

    QMap<int, QWidget*> tabMap;

    int tabCount = 0;
};
