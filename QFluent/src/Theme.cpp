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
        d->sysIsDarkMode = (currentScheme == Qt::ColorScheme::Dark);

        QPointer<Theme> self = this;
        QStyleHints *styleHints = QApplication::styleHints();
        connect(styleHints, &QStyleHints::colorSchemeChanged, [d, self](Qt::ColorScheme scheme) {
            // 检查 Theme 对象是否仍然有效
            if (!self) {
                return;
            }
            if (scheme == Qt::ColorScheme::Dark) {
                d->sysIsDarkMode = true;
            } else if (scheme == Qt::ColorScheme::Light) {
                d->sysIsDarkMode = false;
            }
            if (d->autoTheme) {
                self->setTheme(Fluent::ThemeMode::AUTO);
            }
        });
    }
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
    return d->currentTheme;
}

void Theme::setTheme(Fluent::ThemeMode theme, bool lazy) {
    Q_D(Theme);

    if(theme == Fluent::ThemeMode::AUTO) {
        d->autoTheme = true;
        theme = (d->sysIsDarkMode ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);
    } else {
        d->autoTheme = false;
    }

    if (d->currentTheme != theme) {
        d->currentTheme = theme;
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
    return d->themeColor;
}

QColor Theme::themeColor(Fluent::ThemeColor type) const {
    Q_D(const Theme);
    return d->calculateThemeColor(type);
}

void Theme::setThemeColor(const QColor& color, bool lazy) {
    Q_D(Theme);

    if (d->themeColor != color) {
        d->themeColor = color;
        if (!lazy) {
            StyleSheetManager::instance()->updateStyleSheet(lazy);
            // emit themeColorChanged(color);
        }
    }
}

bool Theme::isDarkTheme() const {
    Q_D(const Theme);
    return d->currentTheme == Fluent::ThemeMode::DARK;
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
