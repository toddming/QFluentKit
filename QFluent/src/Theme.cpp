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
    connect(styleHints, &QStyleHints::colorSchemeChanged, [d](Qt::ColorScheme scheme) {
        if (scheme == Qt::ColorScheme::Dark) {
            d->_sysIsDarkMode = true;
        } else if (scheme == Qt::ColorScheme::Light) {
            d->_sysIsDarkMode = false;
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

void Theme::setTheme(Fluent::ThemeMode theme, bool save, bool lazy) {
    Q_D(Theme);
    Q_UNUSED(save) // 在实际应用中，这里可以保存设置到配置文件

    if (d->_currentTheme != theme) {
        d->_currentTheme = theme;
        StyleSheetManager::instance()->updateStyleSheet(lazy);
        emit themeModeChanged(theme);
    }
}

void Theme::toggleTheme(bool save, bool lazy) {
    Fluent::ThemeMode newTheme = isDarkTheme() ? Fluent::ThemeMode::LIGHT : Fluent::ThemeMode::DARK;
    setTheme(newTheme, save, lazy);
}

QColor Theme::themeColor() const {
    Q_D(const Theme);
    return d->_themeColor;
}

QColor Theme::themeColor(Fluent::ThemeColor type) const {
    Q_D(const Theme);
    return d->calculateThemeColor(type);
}

void Theme::setThemeColor(const QColor& color, bool save, bool lazy) {
    Q_D(Theme);
    Q_UNUSED(save) // 在实际应用中，这里可以保存设置到配置文件

    if (d->_themeColor != color) {
        d->_themeColor = color;
        StyleSheetManager::instance()->updateStyleSheet(lazy);
        // 这里可以发射主题颜色改变信号
        // emit themeColorChanged(color);
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
