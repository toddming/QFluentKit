#pragma once

#include <QtGlobal>
#include <QObject>
#include <QFont>
#include <QScopedPointer>
#include <QColor>

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
    static Theme *instance();

    Fluent::ThemeMode theme() const;
    void setTheme(Fluent::ThemeMode theme, bool lazy = false);
    void toggleTheme(bool lazy = false);

    // 颜色管理
    QColor themeColor() const;
    QColor themeColor(Fluent::ThemeColor type) const;
    void setThemeColor(const QColor& color, bool lazy = false);

    bool isDarkTheme() const;

    /**
     * @brief 静态便捷方法 — 减少组件对 Theme::instance() 的直接调用
     *
     * 组件 paintEvent 中可直接调用 Theme::isDark() 代替 Theme::isDark()
     * 未来可扩展为从线程局部存储或上下文获取，便于测试替换
     */
    static bool isDark() { return instance()->isDarkTheme(); }

    /**
     * @brief 静态便捷方法 — 获取当前主题色
     */
    static QColor color() { return instance()->themeColor(); }

    /**
     * @brief 静态便捷方法 — 获取指定类型主题色
     */
    static QColor color(Fluent::ThemeColor type) { return instance()->themeColor(type); }

    void setFont(QWidget *widget, int fontSize = 14, QFont::Weight weight = QFont::Normal);
    QFont font(int fontSize = 14, QFont::Weight weight = QFont::Normal) const;

Q_SIGNALS:
    Q_SIGNAL void themeModeChanged(Fluent::ThemeMode themeType);
    Q_SIGNAL void themeColorChanged(const QColor& color);

private:
    QScopedPointer<ThemePrivate> d_ptr;
};
