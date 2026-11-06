#include "Theme.h"

#include <QWidget>
#include <QApplication>
#include <QStyleHints>
#include <QPointer>
#include <QHash>
#include <QPair>
#include <QThread>
#include <QMutex>

#include "Private/ThemePrivate.h"

Q_GLOBAL_STATIC(Theme, qtheme)

namespace {

// Theme 是全局静态对象，程序退出阶段可能已经析构。
// 所有静态入口都通过这里取实例，必须容忍返回空指针，
// 否则静态对象销毁顺序不同就会在退出时崩溃。
inline Theme* themeInstance()
{
    return qtheme();
}

const QColor& defaultThemeColor()
{
    static const QColor color(0x00, 0x66, 0xb4);
    return color;
}

} // namespace

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
    auto *self = themeInstance();
    if (!self) {
        return Fluent::ThemeMode::LIGHT;
    }
    return self->d_func()->m_currentTheme;
}

void Theme::setThemeMode(Fluent::ThemeMode mode, bool lazy) {
    auto *self = themeInstance();
    if (!self) {
        return;
    }
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
    auto *self = themeInstance();
    if (!self) {
        return;
    }
    Fluent::ThemeMode newMode = isDark() ? Fluent::ThemeMode::LIGHT : Fluent::ThemeMode::DARK;
    auto *d = self->d_func();
    d->m_autoTheme = false;
    setThemeMode(newMode, lazy);
}

QColor Theme::themeColor() {
    auto *self = themeInstance();
    if (!self) {
        return defaultThemeColor();
    }
    return self->d_func()->m_themeColor;
}

QColor Theme::themeColor(Fluent::ThemeColor type) {
    auto *self = themeInstance();
    if (!self) {
        return defaultThemeColor();
    }
    return self->d_func()->calculateThemeColor(type);
}

void Theme::setThemeColor(const QColor& color, bool lazy) {
    auto *self = themeInstance();
    if (!self) {
        return;
    }
    auto *d = self->d_func();

    if (d->m_themeColor != color) {
        d->m_themeColor = color;
        if (!lazy) {
            emit self->themeColorChanged(color);
        }
    }
}

bool Theme::isDark() {
    auto *self = themeInstance();
    if (!self) {
        return false;
    }
    auto *d = self->d_func();
    if (d->m_autoTheme) {
        return d->m_sysIsDarkMode;
    }
    return d->m_currentTheme == Fluent::ThemeMode::DARK;
}

QFont Theme::font(int fontSize, QFont::Weight weight)
{
    Q_ASSERT(qApp && QThread::currentThread() == qApp->thread());

    // 缓存可能被非主线程访问（例如后台线程里构造控件），
    // release 下 Q_ASSERT 不生效，这里用互斥量保证安全。
    static QMutex mutex;
    static QHash<QPair<int, int>, QFont> fontCache;

    const auto key = qMakePair(fontSize, static_cast<int>(weight));

    {
        QMutexLocker locker(&mutex);
        const auto it = fontCache.constFind(key);
        if (it != fontCache.constEnd()) {
            return it.value();
        }
    }

    QFont font;
    font.setFamilies({"Microsoft YaHei", "PingFang SC", "Segoe UI"});
    font.setPixelSize(fontSize);
    font.setWeight(weight);

    QMutexLocker locker(&mutex);
    return fontCache.insert(key, font).value();
}

void Theme::setFont(QWidget *widget, int fontSize, QFont::Weight weight)
{
    if (!widget) {
        return;
    }
    widget->setFont(font(fontSize, weight));
}

void Theme::onThemeModeChanged(QObject* receiver, const std::function<void(Fluent::ThemeMode)>& slot)
{
    auto *self = themeInstance();
    if (!self || !receiver) {
        return;
    }
    connect(self, &Theme::themeModeChanged, receiver, slot);
}

void Theme::onThemeColorChanged(QObject* receiver, const std::function<void(const QColor&)>& slot)
{
    auto *self = themeInstance();
    if (!self || !receiver) {
        return;
    }
    connect(self, &Theme::themeColorChanged, receiver, slot);
}
