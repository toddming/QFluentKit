#pragma once

#include "Window/FluentWidget.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/Navigation/NavigationPanel.h"
#include "QFluent/Navigation/NavigationWidget.h"


class MainWindow : public FluentWidget {
    Q_OBJECT
public:
    explicit MainWindow();

    void setCurrentInterface(const QString &routeKey, int index);

private:
    StackedWidget *m_stacked;

    NavigationPanel *m_navPanel;

    void initWidget();

    void showDialog();

    void addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                         QWidget* widget, bool selectable = true,
                         NavigationPanel::ItemPosition position = NavigationPanel::ItemPosition::TOP,
                         const QString& tooltip = QString(), const QString& parentRouteKey = QString());

};
