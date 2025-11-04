#pragma once

#include "Define.h"
#include <QIcon>

class IconWidget;
class FluentIconBase;
class IconWidgetPrivate {

public:
    Q_D_CREATE(IconWidget)
    Q_PROPERTY_CREATE_D(QIcon, Icon)
    Q_PROPERTY_CREATE_D(ThemeType::ThemeMode, IconTheme)

private:
    FluentIconBase* m_fluentIcon;
};
