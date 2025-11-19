#pragma once

#include <QIcon>
#include "FluentGlobal.h"
#include "QFluent/IconWidget.h"

class FluentIconBase;
class IconWidgetPrivate {
    Q_DECLARE_PUBLIC(IconWidget)

public:
    IconWidget *q_ptr{nullptr};

private:
    std::unique_ptr<FluentIconBase> _fluentIcon;
    QIcon _Icon;
    ThemeType::ThemeMode _IconTheme;
};
