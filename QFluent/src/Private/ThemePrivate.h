#pragma once

#include <QColor>
#include <QHash>

#include "Define.h"

class Theme;
class ThemePrivate
{
public:
    explicit ThemePrivate();
    Q_D_CREATE(Theme)

private:
    struct StyleSheet {
        QString light;
        QString dark;
    };


    QHash<ThemeType::ThemeStyle, StyleSheet> _StyleMap;
    QHash<QWidget *, ThemeType::ThemeStyle> _Widgets;
    StyleSheet getStyleSheetFromFile(const QString& fileName);
    ThemeType::ThemeMode _themeMode {ThemeType::ThemeMode::DARK};
    QColor _themeColor {0x009faa};

    void loadStyleMap();
    QString readFile(const QString &filePath);
    void updateStyleSheet();
    void applyStyleSheet(QWidget* widget, ThemeType::ThemeStyle styleSheet);
    QColor getThemeColor(ThemeType::ThemeColor themeColor);
};
