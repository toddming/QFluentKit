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
    Fluent::ThemeMode m_currentTheme{Fluent::ThemeMode::LIGHT};

    QColor m_themeColor{0x0066b4};

    bool m_sysIsDarkMode{false};

    bool m_autoTheme{true};

    QColor calculateThemeColor(Fluent::ThemeColor type) const;
};
