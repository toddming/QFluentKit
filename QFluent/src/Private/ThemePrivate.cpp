#include "ThemePrivate.h"

#include <QMap>
#include <QWidget>
#include <QFile>

#include "Theme.h"

ThemePrivate::ThemePrivate()
{
    loadStyleMap();
}


void ThemePrivate::loadStyleMap()
{
    const QMap<ThemeType::ThemeStyle, QString> stylePaths {
                                                          {ThemeType::ThemeStyle::MENU ,"menu"},
                                                          {ThemeType::ThemeStyle::LABEL ,"label"},
                                                          {ThemeType::ThemeStyle::PIVOT ,"pivot"},
                                                          {ThemeType::ThemeStyle::BUTTON ,"button"},
                                                          {ThemeType::ThemeStyle::DIALOG ,"dialog"},
                                                          {ThemeType::ThemeStyle::SLIDER ,"slider"},
                                                          {ThemeType::ThemeStyle::INFO_BAR ,"info_bar"},
                                                          {ThemeType::ThemeStyle::SPIN_BOX ,"spin_box"},
                                                          {ThemeType::ThemeStyle::TAB_VIEW ,"tab_view"},
                                                          {ThemeType::ThemeStyle::TOOL_TIP ,"tool_tip"},
                                                          {ThemeType::ThemeStyle::CHECK_BOX ,"check_box"},
                                                          {ThemeType::ThemeStyle::COMBO_BOX ,"combo_box"},
                                                          {ThemeType::ThemeStyle::FLIP_VIEW ,"flip_view"},
                                                          {ThemeType::ThemeStyle::LINE_EDIT ,"line_edit"},
                                                          {ThemeType::ThemeStyle::LIST_VIEW ,"list_view"},
                                                          {ThemeType::ThemeStyle::TREE_VIEW ,"tree_view"},
                                                          {ThemeType::ThemeStyle::INFO_BADGE ,"info_badge"},
                                                          {ThemeType::ThemeStyle::PIPS_PAGER ,"pips_pager"},
                                                          {ThemeType::ThemeStyle::TABLE_VIEW ,"table_view"},
                                                          {ThemeType::ThemeStyle::CARD_WIDGET ,"card_widget"},
                                                          {ThemeType::ThemeStyle::TIME_PICKER ,"time_picker"},
                                                          {ThemeType::ThemeStyle::COLOR_DIALOG ,"color_dialog"},
                                                          {ThemeType::ThemeStyle::MEDIA_PLAYER ,"media_player"},
                                                          {ThemeType::ThemeStyle::SETTING_CARD ,"setting_card"},
                                                          {ThemeType::ThemeStyle::TEACHING_TIP ,"teaching_tip"},
                                                          {ThemeType::ThemeStyle::FLUENT_WINDOW ,"fluent_window"},
                                                          {ThemeType::ThemeStyle::SWITCH_BUTTON ,"switch_button"},
                                                          {ThemeType::ThemeStyle::MESSAGE_DIALOG ,"message_dialog"},
                                                          {ThemeType::ThemeStyle::STATE_TOOL_TIP ,"state_tool_tip"},
                                                          {ThemeType::ThemeStyle::CALENDAR_PICKER ,"calendar_picker"},
                                                          {ThemeType::ThemeStyle::FOLDER_LIST_DIALOG ,"folder_list_dialog"},
                                                          {ThemeType::ThemeStyle::SETTING_CARD_GROUP ,"setting_card_group"},
                                                          {ThemeType::ThemeStyle::EXPAND_SETTING_CARD ,"expand_setting_card"},
                                                          {ThemeType::ThemeStyle::NAVIGATION_INTERFACE ,"navigation_interface"},
                                                          {ThemeType::ThemeStyle::TITLE_BAR ,"title_bar"},
                                                          };

    for (auto it = stylePaths.constBegin(); it != stylePaths.constEnd(); ++it) {
        _StyleMap[it.key()] = getStyleSheetFromFile(it.value());
    }

}

ThemePrivate::StyleSheet ThemePrivate::getStyleSheetFromFile(const QString& fileName)
{
    ThemePrivate::StyleSheet styleString;
    styleString.light = readFile(QString(":/res/style/light/%1.qss").arg(fileName));
    styleString.dark  = readFile(QString(":/res/style/dark/%1.qss").arg(fileName));
    return styleString;
}

void ThemePrivate::applyStyleSheet(QWidget* widget, ThemeType::ThemeStyle styleSheet) {
    if (!widget) return;

    if (styleSheet == ThemeType::ThemeStyle::CUSTOM_STYLE_SHEET) {
        const auto& darkSheet = widget->property("darkSheet").toString();
        const auto& lightSheet = widget->property("lightSheet").toString();

        if (!darkSheet.isEmpty() && !lightSheet.isEmpty()) {
            widget->setStyleSheet(_themeMode == ThemeType::ThemeMode::DARK ? darkSheet : lightSheet);
        }
    } else {
        const auto& sheet = _StyleMap.value(styleSheet);
        widget->setStyleSheet(_themeMode == ThemeType::ThemeMode::DARK ? sheet.dark : sheet.light);
    }
    widget->update();
}


void ThemePrivate::updateStyleSheet() {
    QHash<ThemeType::ThemeStyle, QList<QWidget*>> styleGroups;

    for (auto it = _Widgets.begin(); it != _Widgets.end(); ) {
        if (QWidget* widget = it.key()) {
            styleGroups[it.value()].append(widget);
            ++it;
        } else {
            it = _Widgets.erase(it);
        }
    }

    auto styleIt = styleGroups.constBegin();
    while (styleIt != styleGroups.constEnd()) {
        const ThemeType::ThemeStyle style = styleIt.key();
        const QList<QWidget*> widgets = styleIt.value();

        if (style == ThemeType::ThemeStyle::CUSTOM_STYLE_SHEET) {

            for (QWidget* widget : widgets) {
                const auto& darkSheet = widget->property("darkSheet").toString();
                const auto& lightSheet = widget->property("lightSheet").toString();

                if (!darkSheet.isEmpty() && !lightSheet.isEmpty()) {
                    widget->setStyleSheet(_themeMode == ThemeType::ThemeMode::DARK ? darkSheet : lightSheet);
                }
            }
        } else {
            const QString sheet = _themeMode == ThemeType::ThemeMode::DARK ?
                                      _StyleMap.value(style).dark :
                                      _StyleMap.value(style).light;

            for (QWidget* widget : widgets) {
                widget->setStyleSheet(sheet);
            }
        }
        ++styleIt;
    }
}

QString ThemePrivate::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    file.close();

    content.replace("--ThemeColorPrimary", getThemeColor(ThemeType::ThemeColor::PRIMARY).name());
    content.replace("--ThemeColorDark1",   getThemeColor(ThemeType::ThemeColor::DARK_1).name());
    content.replace("--ThemeColorDark2",   getThemeColor(ThemeType::ThemeColor::DARK_2).name());
    content.replace("--ThemeColorDark3",   getThemeColor(ThemeType::ThemeColor::DARK_3).name());
    content.replace("--ThemeColorLight1",  getThemeColor(ThemeType::ThemeColor::LIGHT_1).name());
    content.replace("--ThemeColorLight2",  getThemeColor(ThemeType::ThemeColor::LIGHT_2).name());
    content.replace("--ThemeColorLight3",  getThemeColor(ThemeType::ThemeColor::LIGHT_3).name());

    return content;
}

QColor ThemePrivate::getThemeColor(ThemeType::ThemeColor themeColor)
{
    QColor baseColor = _themeColor;
    float h, s, v;
    baseColor.getHsvF(&h, &s, &v);

    if (_themeMode == ThemeType::ThemeMode::DARK) {
        s *= 0.84f;
        v = 1.0f;
        switch (themeColor) {
        case ThemeType::ThemeColor::DARK_1: v *= 0.9f; break;
        case ThemeType::ThemeColor::DARK_2: s *= 0.977f; v *= 0.82f; break;
        case ThemeType::ThemeColor::DARK_3: s *= 0.95f; v *= 0.7f; break;
        case ThemeType::ThemeColor::LIGHT_1: s *= 0.92f; break;
        case ThemeType::ThemeColor::LIGHT_2: s *= 0.78f; break;
        case ThemeType::ThemeColor::LIGHT_3: s *= 0.65f; break;
        default: break;
        }
    } else {
        switch (themeColor) {
        case ThemeType::ThemeColor::DARK_1: v *= 0.75f; break;
        case ThemeType::ThemeColor::DARK_2: s *= 1.05f; v *= 0.5f; break;
        case ThemeType::ThemeColor::DARK_3: s *= 1.1f; v *= 0.4f; break;
        case ThemeType::ThemeColor::LIGHT_1: v *= 1.05f; break;
        case ThemeType::ThemeColor::LIGHT_2: s *= 0.75f; v *= 1.05f; break;
        case ThemeType::ThemeColor::LIGHT_3: s *= 0.65f; v *= 1.05f; break;
        default: break;
        }
    }

    return QColor::fromHsvF(h, qMin(s, 1.0), qMin(v, 1.0));
}
