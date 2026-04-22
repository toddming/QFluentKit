#pragma once

#include <QtGlobal>
#include <QObject>
#include <QFont>
#include <QScopedPointer>
#include <QColor>
#include <functional>

#include "FluentGlobal.h"

class ThemePrivate;
class QFLUENT_EXPORT Theme : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, Theme)
    Q_DISABLE_COPY(Theme)

public:
    explicit Theme(QObject* parent = nullptr);
    ~Theme() override;

    static Fluent::ThemeMode themeMode();
    static void setThemeMode(Fluent::ThemeMode mode, bool lazy = false);
    static void toggleTheme(bool lazy = false);

    static bool isDark();

    static QColor themeColor();
    static QColor themeColor(Fluent::ThemeColor type);
    static void setThemeColor(const QColor& color, bool lazy = false);

    static QFont font(int fontSize = 14, QFont::Weight weight = QFont::Normal);
    static void setFont(QWidget *widget, int fontSize = 14, QFont::Weight weight = QFont::Normal);

    static void onThemeModeChanged(QObject* receiver, const std::function<void(Fluent::ThemeMode)>& slot);
    static void onThemeColorChanged(QObject* receiver, const std::function<void(const QColor&)>& slot);

Q_SIGNALS:
    void themeModeChanged(Fluent::ThemeMode themeType);
    void themeColorChanged(const QColor& color);

private:
    QScopedPointer<ThemePrivate> d_ptr;
};
