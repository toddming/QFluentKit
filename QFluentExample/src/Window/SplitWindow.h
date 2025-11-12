#pragma once

#include <QObject>

#include "QFluent/FluentSplitWindow.h"

class SplitWidget : public FluentSplitWindow
{
    Q_OBJECT
public:
    explicit SplitWidget();

private:
    void initUI();

    QWidget* createWidget(const QString &text, QWidget* parent);

};

