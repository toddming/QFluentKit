#pragma once

#include "Define.h"
#include <QIcon>

class IconWidget;
class IconWidgetPrivate {

public:
    Q_D_CREATE(IconWidget)
    Q_PROPERTY_CREATE_D(QIcon, Icon)
    Q_PROPERTY_CREATE_D(bool, IsFluentIcon)
    Q_PROPERTY_CREATE_D(IconType::FLuentIcon, FluentIcon)

    explicit IconWidgetPrivate();
};
