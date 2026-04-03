#pragma once

#include <QIcon>
#include <memory>
#include "FluentGlobal.h"
#include "QFluent/IconWidget.h"

class FluentIconBase;
class IconWidgetPrivate {
    Q_DECLARE_PUBLIC(IconWidget)

public:
    IconWidget *q_ptr{nullptr};

private:
    std::unique_ptr<FluentIconBase> m_fluentIcon;
    QIcon m_icon;
    Fluent::ThemeMode m_iconTheme;
};
