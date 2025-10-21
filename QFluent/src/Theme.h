#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QFont>

#include "Define.h"

class ThemePrivate;
class QFLUENT_EXPORT Theme : public QObject
{
    Q_OBJECT
    Q_Q_CREATE(Theme)

public:
    explicit Theme(QObject* parent = nullptr);
    ~Theme() override;
    static Theme *instance();

    ThemeType::ThemeMode theme() const;
    void setTheme(ThemeType::ThemeMode theme, bool save = false, bool lazy = false);
    void toggleTheme(bool save = false, bool lazy = false);

    // 颜色管理
    QColor themeColor() const;
    QColor themeColor(ThemeType::ThemeColor type) const;
    void setThemeColor(const QColor& color, bool save = false, bool lazy = false);

    bool isDarkTheme() const;


    void setFont(QWidget *widget, int fontSize = 14, QFont::Weight weight = QFont::Normal);
    QFont getFont(int fontSize = 14, QFont::Weight weight = QFont::Normal);

Q_SIGNALS:
    Q_SIGNAL void themeModeChanged(ThemeType::ThemeMode themeType);

};


#endif // THEME_H
