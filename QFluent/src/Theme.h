#pragma once

#include <QtGlobal>
#include <QObject>
#include <QFont>
#include <QScopedPointer>

#include "FluentGlobal.h"

class ThemePrivate;
class QFLUENT_EXPORT Theme : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, Theme)

public:
    explicit Theme(QObject* parent = nullptr);
    ~Theme() override;
    static Theme *instance();

    Fluent::ThemeMode theme() const;
    void setTheme(Fluent::ThemeMode theme, bool save = false, bool lazy = false);
    void toggleTheme(bool save = false, bool lazy = false);

    // 颜色管理
    QColor themeColor() const;
    QColor themeColor(Fluent::ThemeColor type) const;
    void setThemeColor(const QColor& color, bool save = false, bool lazy = false);

    bool isDarkTheme() const;


    void setFont(QWidget *widget, int fontSize = 14, QFont::Weight weight = QFont::Normal);
    QFont getFont(int fontSize = 14, QFont::Weight weight = QFont::Normal);

Q_SIGNALS:
    Q_SIGNAL void themeModeChanged(Fluent::ThemeMode themeType);

private:
    QScopedPointer<ThemePrivate> d_ptr;
};
