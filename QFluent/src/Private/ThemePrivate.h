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
    ThemeType::ThemeMode _currentTheme{ThemeType::ThemeMode::DARK};

    QColor _themeColor{0x0066b4};

    bool _sysIsDarkMode;

    QColor calculateThemeColor(ThemeType::ThemeColor type) const;
};
