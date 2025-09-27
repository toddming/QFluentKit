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

    bool isDarkMode();

    void setThemeColor(QColor color);

    void setThemeMode(ThemeType::ThemeMode themeMode);

    QColor themeColor(ThemeType::ThemeColor themeColor = ThemeType::ThemeColor::PRIMARY);

    void registerWidget(QWidget* widget, ThemeType::ThemeStyle styleSheet);

    void setFont(QWidget *widget, int fontSize = 14, QFont::Weight weight = QFont::Normal);

    QFont getFont(int fontSize = 14, QFont::Weight weight = QFont::Normal);

Q_SIGNALS:
    Q_SIGNAL void themeModeChanged(ThemeType::ThemeMode themeType);

};


#endif // THEME_H
