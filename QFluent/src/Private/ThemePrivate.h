#pragma once

#include <QColor>
#include "Theme.h"
#include "FluentGlobal.h"

class ThemePrivate
{
    Q_DECLARE_PUBLIC(Theme)
public:
    Theme *q_ptr{nullptr};
    explicit ThemePrivate();

private:
    Fluent::ThemeMode currentTheme{Fluent::ThemeMode::LIGHT};

    QColor themeColor{0x0066b4};

    bool sysIsDarkMode;

    bool autoTheme{true};

    QColor calculateThemeColor(Fluent::ThemeColor type) const;
};
