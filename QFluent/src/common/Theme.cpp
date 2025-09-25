#include "../common/Theme.h"

#include <QPainter>
#include <QPainterPath>

#include <QApplication>
#include <QStyleHints>
#include <QDebug>
#include <QFile>

Q_GLOBAL_STATIC(Theme, qtheme)

Theme::Theme(QObject* parent) : QObject(parent)
{
    Qt::ColorScheme currentScheme = QApplication::styleHints()->colorScheme();
    qDebug() << "当前主题:" << (currentScheme == Qt::ColorScheme::Dark ? "深色" : "浅色");

    QStyleHints *styleHints = QApplication::styleHints();

    // 连接 colorSchemeChanged 信号
    connect(styleHints, &QStyleHints::colorSchemeChanged, [](Qt::ColorScheme scheme) {
        if (scheme == Qt::ColorScheme::Dark) {
            qDebug() << "系统切换到深色模式";
        } else if (scheme == Qt::ColorScheme::Light) {
            qDebug() << "系统切换到浅色模式";
        } else {
            qDebug() << "未知主题模式";
        }
    });

    loadStyleMap();
}

Theme *Theme::instance()
{
    return qtheme();
}


void Theme::setThemeMode(ThemeType themeType)
{
    m_themeType = themeType;

    updateStyleSheet();

    Q_EMIT themeTypeChanged(m_themeType);
}


bool Theme::isDarkMode() const
{
    return (m_themeType == ThemeType::DARK);
}


void Theme::setThemeColor(QColor color)
{
    m_themeColor = color;
}

QColor Theme::themeColor(ThemeColorType colorType)
{

    QColor baseColor = m_themeColor;
    float h, s, v;
    baseColor.getHsvF(&h, &s, &v);

    if (isDarkMode()) {
        s *= 0.84f;
        v = 1.0f;
        switch (colorType) {
        case ThemeColorType::DARK_1: v *= 0.9f; break;
        case ThemeColorType::DARK_2: s *= 0.977f; v *= 0.82f; break;
        case ThemeColorType::DARK_3: s *= 0.95f; v *= 0.7f; break;
        case ThemeColorType::LIGHT_1: s *= 0.92f; break;
        case ThemeColorType::LIGHT_2: s *= 0.78f; break;
        case ThemeColorType::LIGHT_3: s *= 0.65f; break;
        default: break;
        }
    } else {
        switch (colorType) {
        case ThemeColorType::DARK_1: v *= 0.75f; break;
        case ThemeColorType::DARK_2: s *= 1.05f; v *= 0.5f; break;
        case ThemeColorType::DARK_3: s *= 1.1f; v *= 0.4f; break;
        case ThemeColorType::LIGHT_1: v *= 1.05f; break;
        case ThemeColorType::LIGHT_2: s *= 0.75f; v *= 1.05f; break;
        case ThemeColorType::LIGHT_3: s *= 0.65f; v *= 1.05f; break;
        default: break;
        }
    }

    return QColor::fromHsvF(h, qMin(s, 1.0), qMin(v, 1.0));
}

void Theme::registerWidget(QWidget* widget, StyleSheetType styleSheet)
{
    if (!widget || m_Widgets.contains(widget))
        return;

    connect(widget, &QObject::destroyed, this, [this, widget]() { m_Widgets.remove(widget); });
    m_Widgets.insert(widget, styleSheet);

    applyStyleSheet(widget, styleSheet);
}

QString Theme::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    file.close();

    content.replace("--ThemeColorPrimary", themeColor(ThemeColorType::PRIMARY).name());
    content.replace("--ThemeColorDark1",   themeColor(ThemeColorType::DARK_1).name());
    content.replace("--ThemeColorDark2",   themeColor(ThemeColorType::DARK_2).name());
    content.replace("--ThemeColorDark3",   themeColor(ThemeColorType::DARK_3).name());
    content.replace("--ThemeColorLight1",  themeColor(ThemeColorType::LIGHT_1).name());
    content.replace("--ThemeColorLight2",  themeColor(ThemeColorType::LIGHT_2).name());
    content.replace("--ThemeColorLight3",  themeColor(ThemeColorType::LIGHT_3).name());

    return content;
}

Theme::StyleSheet Theme::getStyleSheetFromFile(const QString& fileName)
{
    Theme::StyleSheet styleString;
    styleString.light = readFile(QString(":/res/style/light/%1.qss").arg(fileName));
    styleString.dark  = readFile(QString(":/res/style/dark/%1.qss").arg(fileName));
    return styleString;
}

void Theme::loadStyleMap()
{
    m_StyleMap.clear();

    const QMap<StyleSheetType, QString> stylePaths {
                                                   {StyleSheetType::MENU ,"menu"},
                                                   {StyleSheetType::LABEL ,"label"},
                                                   {StyleSheetType::PIVOT ,"pivot"},
                                                   {StyleSheetType::BUTTON ,"button"},
                                                   {StyleSheetType::DIALOG ,"dialog"},
                                                   {StyleSheetType::SLIDER ,"slider"},
                                                   {StyleSheetType::INFO_BAR ,"info_bar"},
                                                   {StyleSheetType::SPIN_BOX ,"spin_box"},
                                                   {StyleSheetType::TAB_VIEW ,"tab_view"},
                                                   {StyleSheetType::TOOL_TIP ,"tool_tip"},
                                                   {StyleSheetType::CHECK_BOX ,"check_box"},
                                                   {StyleSheetType::COMBO_BOX ,"combo_box"},
                                                   {StyleSheetType::FLIP_VIEW ,"flip_view"},
                                                   {StyleSheetType::LINE_EDIT ,"line_edit"},
                                                   {StyleSheetType::LIST_VIEW ,"list_view"},
                                                   {StyleSheetType::TREE_VIEW ,"tree_view"},
                                                   {StyleSheetType::INFO_BADGE ,"info_badge"},
                                                   {StyleSheetType::PIPS_PAGER ,"pips_pager"},
                                                   {StyleSheetType::TABLE_VIEW ,"table_view"},
                                                   {StyleSheetType::CARD_WIDGET ,"card_widget"},
                                                   {StyleSheetType::TIME_PICKER ,"time_picker"},
                                                   {StyleSheetType::COLOR_DIALOG ,"color_dialog"},
                                                   {StyleSheetType::MEDIA_PLAYER ,"media_player"},
                                                   {StyleSheetType::SETTING_CARD ,"setting_card"},
                                                   {StyleSheetType::TEACHING_TIP ,"teaching_tip"},
                                                   {StyleSheetType::FLUENT_WINDOW ,"fluent_window"},
                                                   {StyleSheetType::SWITCH_BUTTON ,"switch_button"},
                                                   {StyleSheetType::MESSAGE_DIALOG ,"message_dialog"},
                                                   {StyleSheetType::STATE_TOOL_TIP ,"state_tool_tip"},
                                                   {StyleSheetType::CALENDAR_PICKER ,"calendar_picker"},
                                                   {StyleSheetType::FOLDER_LIST_DIALOG ,"folder_list_dialog"},
                                                   {StyleSheetType::SETTING_CARD_GROUP ,"setting_card_group"},
                                                   {StyleSheetType::EXPAND_SETTING_CARD ,"expand_setting_card"},
                                                   {StyleSheetType::NAVIGATION_INTERFACE ,"navigation_interface"},
                                                   {StyleSheetType::TITLE_BAR ,"title_bar"},
                                                   };

    for (auto it = stylePaths.constBegin(); it != stylePaths.constEnd(); ++it) {
        m_StyleMap[it.key()] = getStyleSheetFromFile(it.value());
    }

}

void Theme::updateStyleSheet() {
    QHash<StyleSheetType, QList<QWidget*>> styleGroups;

    for (auto it = m_Widgets.begin(); it != m_Widgets.end(); ) {
        if (QWidget* widget = it.key()) {
            styleGroups[it.value()].append(widget);
            ++it;
        } else {
            it = m_Widgets.erase(it);
        }
    }

    auto styleIt = styleGroups.constBegin();
    while (styleIt != styleGroups.constEnd()) {
        const StyleSheetType style = styleIt.key();
        const QList<QWidget*> widgets = styleIt.value();

        if (style == StyleSheetType::CUSTOM_STYLE_SHEET) {

            for (QWidget* widget : widgets) {
                const auto& darkSheet = widget->property("darkSheet").toString();
                const auto& lightSheet = widget->property("lightSheet").toString();

                if (!darkSheet.isEmpty() && !lightSheet.isEmpty()) {
                    widget->setStyleSheet(isDarkMode() ? darkSheet : lightSheet);
                }
            }


        } else {
            const QString sheet = isDarkMode() ?
                                      m_StyleMap.value(style).dark :
                                      m_StyleMap.value(style).light;

            for (QWidget* widget : widgets) {
                widget->setStyleSheet(sheet);
            }
        }



        ++styleIt;
    }
}

void Theme::applyStyleSheet(QWidget* widget, StyleSheetType styleSheet) {
    if (!widget) return;

    if (styleSheet == StyleSheetType::CUSTOM_STYLE_SHEET) {
        const auto& darkSheet = widget->property("darkSheet").toString();
        const auto& lightSheet = widget->property("lightSheet").toString();

        if (!darkSheet.isEmpty() && !lightSheet.isEmpty()) {
            widget->setStyleSheet(isDarkMode() ? darkSheet : lightSheet);
        }
    } else {
        const auto& sheet = m_StyleMap.value(styleSheet);
        widget->setStyleSheet(isDarkMode() ? sheet.dark : sheet.light);
    }
    widget->update();
}

void Theme::setFont(QWidget *widget, int fontSize, QFont::Weight weight)
{
    widget->setFont(getFont(fontSize, weight));
}

QFont Theme::getFont(int fontSize, QFont::Weight weight)
{
    QFont font = QFont();
    font.setFamilies({"Microsoft YaHei", "PingFang SC", "Segoe UI"});
    font.setPixelSize(fontSize);
    font.setWeight(weight);
    return font;
}
