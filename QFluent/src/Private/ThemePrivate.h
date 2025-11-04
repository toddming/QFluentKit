#pragma once

#include <QColor>

#include "Define.h"

class Theme;
class ThemePrivate
{
public:
    explicit ThemePrivate();
    Q_D_CREATE(Theme)

private:
    ThemeType::ThemeMode _currentTheme{ThemeType::ThemeMode::DARK};

    QColor _themeColor{0x0066b4};

    bool _sysIsDarkMode;

    QColor calculateThemeColor(ThemeType::ThemeColor type) const;
};
