#include "Theme.h"

#include <QWidget>
#include <QApplication>
#include <QStyleHints>

#include "Private/ThemePrivate.h"

Q_GLOBAL_STATIC(Theme, qtheme)

Theme::Theme(QObject* parent) : QObject(parent), d_ptr(new ThemePrivate())
{
    Q_D(Theme);
    d->q_ptr = this;

    Qt::ColorScheme currentScheme = QApplication::styleHints()->colorScheme();
    qWarning() << "当前主题:" << (currentScheme == Qt::ColorScheme::Dark ? "深色" : "浅色");

    QStyleHints *styleHints = QApplication::styleHints();

    connect(styleHints, &QStyleHints::colorSchemeChanged, [](Qt::ColorScheme scheme) {
        if (scheme == Qt::ColorScheme::Dark) {
            qWarning() << "系统切换到深色模式";
        } else if (scheme == Qt::ColorScheme::Light) {
            qWarning() << "系统切换到浅色模式";
        } else {
            qWarning() << "未知主题模式";
        }
    });

}

Theme::~Theme()
{

}

Theme *Theme::instance()
{
    return qtheme();
}


void Theme::setThemeMode(ThemeType::ThemeMode themeMode)
{
    Q_D(Theme);
    d->_themeMode = themeMode;
    d->updateStyleSheet();
    Q_EMIT themeModeChanged(themeMode);
}


bool Theme::isDarkMode()
{
    Q_D(Theme);
    return (d->_themeMode == ThemeType::DARK);
}


void Theme::setThemeColor(QColor color)
{
    Q_D(Theme);
    d->_themeColor = color;
}

QColor Theme::themeColor(ThemeType::ThemeColor themeColor)
{
    Q_D(Theme);
    return d->getThemeColor(themeColor);
}

void Theme::registerWidget(QWidget* widget, ThemeType::ThemeStyle styleSheet)
{
    Q_D(Theme);

    if (!widget || d->_Widgets.contains(widget))
        return;

    connect(widget, &QObject::destroyed, this, [d, widget]() { d->_Widgets.remove(widget); });
    d->_Widgets.insert(widget, styleSheet);

    d->applyStyleSheet(widget, styleSheet);
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
