#pragma once

#include "GalleryInterface.h"
#include "QFluent/TabView.h"

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
    int tabCount = 0;
};
