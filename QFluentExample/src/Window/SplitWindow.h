#pragma once

#include <QObject>

#include "QFluent/FluentWidget.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/Navigation/NavigationPanel.h"
#include "QFluent/Navigation/NavigationWidget.h"

class SplitWidget : public FluentWidget
{
    Q_OBJECT
public:
    explicit SplitWidget();

private:
    StackedWidget *m_stacked;

    NavigationPanel *m_navPanel;

    void initWidget();

    QWidget* createWidget(const QString &text, QWidget* parent);

    void addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                         QWidget* widget, bool selectable = true,
                         Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP,
                         const QString& tooltip = QString(), const QString& parentRouteKey = QString());

};

