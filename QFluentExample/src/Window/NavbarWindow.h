#pragma once

#include <QObject>

#include "QFluent/FluentWidget.h"

class NavbarWidget : public FluentWidget
{
    Q_OBJECT
public:
    explicit NavbarWidget();

private:
    void initUI();

    QWidget* createWidget(const QString &text, QWidget* parent);

};

