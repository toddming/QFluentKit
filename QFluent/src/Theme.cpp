#include "Theme.h"

#include <QWidget>
#include <QApplication>
#include <QStyleHints>

#include "StyleSheet.h"
#include "Private/ThemePrivate.h"

Q_GLOBAL_STATIC(Theme, qtheme)

Theme::Theme(QObject* parent) : QObject(parent)
  , d_ptr(new ThemePrivate)
{
    Q_D(Theme);
    d->q_ptr = this;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    Qt::ColorScheme currentScheme = QApplication::styleHints()->colorScheme();
    qWarning() << "当前主题:" << (currentScheme == Qt::ColorScheme::Dark ? "深色" : "浅色");
    d->_sysIsDarkMode = (currentScheme == Qt::ColorScheme::Dark);

    QStyleHints *styleHints = QApplication::styleHints();
    connect(styleHints, &QStyleHints::colorSchemeChanged, [d, this](Qt::ColorScheme scheme) {
        if (scheme == Qt::ColorScheme::Dark) {
            d->_sysIsDarkMode = true;
        } else if (scheme == Qt::ColorScheme::Light) {
            d->_sysIsDarkMode = false;
        }
        if (d->_autoTheme) {
            setTheme(Fluent::ThemeMode::AUTO);
        }
    });
#endif
}

Theme::~Theme()
{

}

Theme *Theme::instance()
{
    return qtheme();
}

Fluent::ThemeMode Theme::theme() const {
    Q_D(const Theme);
    return d->_currentTheme;
}

void Theme::setTheme(Fluent::ThemeMode theme, bool lazy) {
    Q_D(Theme);

    if(theme == Fluent::ThemeMode::AUTO) {
        d->_autoTheme = true;
        theme = (d->_sysIsDarkMode ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);
    } else {
        d->_autoTheme = false;
    }

    if (d->_currentTheme != theme) {
        d->_currentTheme = theme;
        if (!lazy) {
            StyleSheetManager::instance()->updateStyleSheet(lazy);
            emit themeModeChanged(theme);
        }
    }
}

void Theme::toggleTheme(bool lazy) {
    Fluent::ThemeMode newTheme = isDarkTheme() ? Fluent::ThemeMode::LIGHT : Fluent::ThemeMode::DARK;
    setTheme(newTheme, lazy);
}

QColor Theme::themeColor() const {
    Q_D(const Theme);
    return d->_themeColor;
}

QColor Theme::themeColor(Fluent::ThemeColor type) const {
    Q_D(const Theme);
    return d->calculateThemeColor(type);
}

void Theme::setThemeColor(const QColor& color, bool lazy) {
    Q_D(Theme);

    if (d->_themeColor != color) {
        d->_themeColor = color;
        if (!lazy) {
            StyleSheetManager::instance()->updateStyleSheet(lazy);
            // emit themeColorChanged(color);
        }
    }
}

bool Theme::isDarkTheme() const {
    Q_D(const Theme);
    return d->_currentTheme == Fluent::ThemeMode::DARK;
}





void Theme::setFont(QWidget *widget, int fontSize, QFont::Weight weight)
{
    widget->setFont(getFont(fontSize, weight));
}

QFont Theme::getFont(int fontSize, QFont::Weight weight)
{
    QFont font;
    font.setFamilies({"Microsoft YaHei", "PingFang SC", "Segoe UI"});
    font.setPixelSize(fontSize);
    font.setWeight(weight);
    return font;
}
