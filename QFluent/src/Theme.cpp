#include "Theme.h"

#include <QWidget>
#include <QApplication>
#include <QStyleHints>
#include <QPointer>
#include <QHash>
#include <QPair>
#include <QThread>

#include "Private/ThemePrivate.h"

Q_GLOBAL_STATIC(Theme, qtheme)

Theme::Theme(QObject* parent) : QObject(parent)
  , d_ptr(new ThemePrivate)
{
    Q_D(Theme);
    d->q_ptr = this;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    if (qApp) {
        QStyleHints *styleHints = QApplication::styleHints();
        if (!styleHints) {
            return;
        }

        Qt::ColorScheme currentScheme = styleHints->colorScheme();
        d->m_sysIsDarkMode = (currentScheme == Qt::ColorScheme::Dark);

        QPointer<Theme> self = this;
        connect(styleHints, &QStyleHints::colorSchemeChanged, this, [d, self](Qt::ColorScheme scheme) {
            if (!self) {
                return;
            }
            if (scheme == Qt::ColorScheme::Dark) {
                d->m_sysIsDarkMode = true;
            } else if (scheme == Qt::ColorScheme::Light) {
                d->m_sysIsDarkMode = false;
            }
            if (d->m_autoTheme) {
                self->setThemeMode(Fluent::ThemeMode::AUTO);
            }
        }, Qt::QueuedConnection);
    }
#endif
}

Theme::~Theme()
{
}

Fluent::ThemeMode Theme::themeMode() {
    auto *self = qtheme();
    return self->d_func()->m_currentTheme;
}

void Theme::setThemeMode(Fluent::ThemeMode mode, bool lazy) {
    auto *self = qtheme();
    auto *d = self->d_func();

    if(mode == Fluent::ThemeMode::AUTO) {
        d->m_autoTheme = true;
        mode = (d->m_sysIsDarkMode ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);
    } else {
        d->m_autoTheme = false;
    }

    if (d->m_currentTheme != mode) {
        d->m_currentTheme = mode;
        if (!lazy) {
            emit self->themeModeChanged(mode);
        }
    }
}

void Theme::toggleTheme(bool lazy) {
    Fluent::ThemeMode newMode = isDark() ? Fluent::ThemeMode::LIGHT : Fluent::ThemeMode::DARK;
    auto *d = qtheme()->d_func();
    d->m_autoTheme = false;
    setThemeMode(newMode, lazy);
}

QColor Theme::themeColor() {
    return qtheme()->d_func()->m_themeColor;
}

QColor Theme::themeColor(Fluent::ThemeColor type) {
    return qtheme()->d_func()->calculateThemeColor(type);
}

void Theme::setThemeColor(const QColor& color, bool lazy) {
    auto *self = qtheme();
    auto *d = self->d_func();

    if (d->m_themeColor != color) {
        d->m_themeColor = color;
        if (!lazy) {
            emit self->themeColorChanged(color);
        }
    }
}

bool Theme::isDark() {
    auto *d = qtheme()->d_func();
    if (d->m_autoTheme) {
        return d->m_sysIsDarkMode;
    }
    return d->m_currentTheme == Fluent::ThemeMode::DARK;
}

QFont Theme::font(int fontSize, QFont::Weight weight)
{
    Q_ASSERT(qApp && QThread::currentThread() == qApp->thread());

    static QHash<QPair<int, int>, QFont> fontCache;
    auto key = qMakePair(fontSize, static_cast<int>(weight));
    auto it = fontCache.constFind(key);
    if (it != fontCache.constEnd()) {
        return it.value();
    }

    QFont font;
    font.setFamilies({"Microsoft YaHei", "PingFang SC", "Segoe UI"});
    font.setPixelSize(fontSize);
    font.setWeight(weight);
    fontCache.insert(key, font);
    return font;
}

void Theme::setFont(QWidget *widget, int fontSize, QFont::Weight weight)
{
    widget->setFont(font(fontSize, weight));
}

void Theme::onThemeModeChanged(QObject* receiver, const std::function<void(Fluent::ThemeMode)>& slot)
{
    connect(qtheme(), &Theme::themeModeChanged, receiver, slot);
}

void Theme::onThemeColorChanged(QObject* receiver, const std::function<void(const QColor&)>& slot)
{
    connect(qtheme(), &Theme::themeColorChanged, receiver, slot);
}
