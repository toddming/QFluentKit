#include "Theme.h"

#include <QWidget>
#include <QApplication>
#include <QStyleHints>
#include <QPointer>

#include "StyleSheet.h"
#include "Private/ThemePrivate.h"

Q_GLOBAL_STATIC(Theme, qtheme)

Theme::Theme(QObject* parent) : QObject(parent)
  , d_ptr(new ThemePrivate)
{
    Q_D(Theme);
    d->q_ptr = this;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    if (qApp) {
        Qt::ColorScheme currentScheme = QApplication::styleHints()->colorScheme();
        d->m_sysIsDarkMode = (currentScheme == Qt::ColorScheme::Dark);

        QPointer<Theme> self = this;
        QStyleHints *styleHints = QApplication::styleHints();
        connect(styleHints, &QStyleHints::colorSchemeChanged, [d, self](Qt::ColorScheme scheme) {
            if (!self) {
                return;
            }
            if (scheme == Qt::ColorScheme::Dark) {
                d->m_sysIsDarkMode = true;
            } else if (scheme == Qt::ColorScheme::Light) {
                d->m_sysIsDarkMode = false;
            }
            if (d->m_autoTheme) {
                self->setTheme(Fluent::ThemeMode::AUTO);
            }
        });
    }
#endif
}

Theme::~Theme()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    if (qApp) {
        disconnect(QApplication::styleHints(), nullptr, this, nullptr);
    }
#endif
}

Theme *Theme::instance()
{
    return qtheme();
}

Fluent::ThemeMode Theme::theme() const {
    Q_D(const Theme);
    return d->m_currentTheme;
}

void Theme::setTheme(Fluent::ThemeMode theme, bool lazy) {
    Q_D(Theme);

    if(theme == Fluent::ThemeMode::AUTO) {
        d->m_autoTheme = true;
        theme = (d->m_sysIsDarkMode ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);
    } else {
        d->m_autoTheme = false;
    }

    if (d->m_currentTheme != theme) {
        d->m_currentTheme = theme;
        if (!lazy) {
            StyleSheetManager::instance()->updateStyleSheet(lazy);
            emit themeModeChanged(theme);
        }
    }
}

void Theme::toggleTheme(bool lazy) {
    Fluent::ThemeMode newTheme = isDarkTheme() ? Fluent::ThemeMode::LIGHT : Fluent::ThemeMode::DARK;
    Q_D(Theme);
    d->m_autoTheme = false;
    setTheme(newTheme, lazy);
}

QColor Theme::themeColor() const {
    Q_D(const Theme);
    return d->m_themeColor;
}

QColor Theme::themeColor(Fluent::ThemeColor type) const {
    Q_D(const Theme);
    return d->calculateThemeColor(type);
}

void Theme::setThemeColor(const QColor& color, bool lazy) {
    Q_D(Theme);

    if (d->m_themeColor != color) {
        d->m_themeColor = color;
        if (!lazy) {
            StyleSheetManager::instance()->updateStyleSheet(lazy);
            emit themeColorChanged(color);
        }
    }
}

bool Theme::isDarkTheme() const {
    Q_D(const Theme);
    if (d->m_autoTheme) {
        return d->m_sysIsDarkMode;
    }
    return d->m_currentTheme == Fluent::ThemeMode::DARK;
}


void Theme::setFont(QWidget *widget, int fontSize, QFont::Weight weight)
{
    widget->setFont(font(fontSize, weight));
}

QFont Theme::font(int fontSize, QFont::Weight weight)
{
    QFont font;
    font.setFamilies({"Microsoft YaHei", "PingFang SC", "Segoe UI"});
    font.setPixelSize(fontSize);
    font.setWeight(weight);
    return font;
}
