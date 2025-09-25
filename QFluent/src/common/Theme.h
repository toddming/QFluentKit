#ifndef THEME_H
#define THEME_H

#include <QObject>
#include <QColor>
#include <QMap>
#include <QWidget>

#include "../Property.h"

#define sTheme Theme::instance()

class QPainter;
class QFLUENT_EXPORT Theme : public QObject
{
    Q_OBJECT

public:
    explicit Theme(QObject* parent = nullptr);
    static Theme *instance();


public:

    enum class ThemeType {
        AUTO,
        LIGHT,
        DARK
    };
    Q_ENUM(ThemeType)

    enum class ThemeColorType {
        PRIMARY,
        DARK_1,
        DARK_2,
        DARK_3,
        LIGHT_1,
        LIGHT_2,
        LIGHT_3
    };
    Q_ENUM(ThemeColorType)

    enum class StyleSheetType {
        MENU,
        LABEL,
        PIVOT,
        BUTTON,
        DIALOG,
        SLIDER,
        INFO_BAR,
        SPIN_BOX,
        TAB_VIEW,
        TOOL_TIP,
        CHECK_BOX,
        COMBO_BOX,
        FLIP_VIEW,
        LINE_EDIT,
        LIST_VIEW,
        TREE_VIEW,
        INFO_BADGE,
        PIPS_PAGER,
        TABLE_VIEW,
        CARD_WIDGET,
        TIME_PICKER,
        COLOR_DIALOG,
        MEDIA_PLAYER,
        SETTING_CARD,
        TEACHING_TIP,
        FLUENT_WINDOW,
        SWITCH_BUTTON,
        MESSAGE_DIALOG,
        STATE_TOOL_TIP,
        CALENDAR_PICKER,
        FOLDER_LIST_DIALOG,
        SETTING_CARD_GROUP,
        EXPAND_SETTING_CARD,
        NAVIGATION_INTERFACE,
        TITLE_BAR,
        CUSTOM_STYLE_SHEET
    };
    Q_ENUM(StyleSheetType)

    void setThemeMode(ThemeType themeType);

    bool isDarkMode() const;

    void setThemeColor(QColor color);

    QColor themeColor(ThemeColorType colorType = ThemeColorType::PRIMARY);

    void registerWidget(QWidget* widget, StyleSheetType styleSheet);

    void setFont(QWidget *widget, int fontSize = 14, QFont::Weight weight = QFont::Normal);

    QFont getFont(int fontSize = 14, QFont::Weight weight = QFont::Normal);

Q_SIGNALS:
    Q_SIGNAL void themeTypeChanged(ThemeType themeType);


private:
    struct StyleSheet
    {
        QString light;
        QString dark;
    };

    QHash<StyleSheetType, StyleSheet> m_StyleMap;

    QHash<QWidget *, StyleSheetType> m_Widgets;

    ThemeType m_themeType{ThemeType::DARK};

    QColor m_themeColor{0x009faa};

    StyleSheet getStyleSheetFromFile(const QString& fileName);

    void loadStyleMap();

    QString readFile(const QString &filePath);

    void updateStyleSheet();

    void applyStyleSheet(QWidget* widget, StyleSheetType styleSheet);
};


#endif // THEME_H
