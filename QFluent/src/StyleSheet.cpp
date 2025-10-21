#include "StyleSheet.h"
#include <QApplication>
#include <QRegularExpression>
#include <QTextStream>
#include <QDynamicPropertyChangeEvent>
#include <QWidget>
#include <QColor>
#include <QString>
#include <QEvent>
#include <QFile>
#include <memory>
#include <vector>

#include "Theme.h"

// ==================== StyleSheetHelper 实现 ====================

QString StyleSheetHelper::applyThemeColor(const QString& qss) {
    QString result = qss;

    QMap<QString, QColor> themeColors = {
        {"--ThemeColorPrimary", Theme::instance()->themeColor(ThemeType::ThemeColor::PRIMARY)},
        {"--ThemeColorDark1", Theme::instance()->themeColor(ThemeType::ThemeColor::DARK_1)},
        {"--ThemeColorDark2", Theme::instance()->themeColor(ThemeType::ThemeColor::DARK_2)},
        {"--ThemeColorDark3", Theme::instance()->themeColor(ThemeType::ThemeColor::DARK_3)},
        {"--ThemeColorLight1", Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_1)},
        {"--ThemeColorLight2", Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_2)},
        {"--ThemeColorLight3", Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_3)}
    };

    for (auto it = themeColors.begin(); it != themeColors.end(); ++it) {
        result.replace(it.key(), it.value().name());
    }

    return result;
}

QString StyleSheetHelper::getStyleSheetFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open QSS file:" << filePath;
        return "";
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

QString StyleSheetHelper::getStyleSheet(const std::shared_ptr<StyleSheetBase>& source, ThemeType::ThemeMode theme) {
    if (!source) return "";
    return applyThemeColor(source->content(theme));
}

QString StyleSheetHelper::getStyleSheet(const QString& source, ThemeType::ThemeMode theme) {
    return getStyleSheet(std::make_shared<StyleSheetFile>(source), theme);
}

void StyleSheetHelper::setStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                                   ThemeType::ThemeMode theme, bool registerWidget) {
    if (!widget) return;

    if (registerWidget) {
        StyleSheetManager::instance()->registerWidget(source, widget);
    }

    widget->setStyleSheet(getStyleSheet(source, theme));
}

void StyleSheetHelper::setStyleSheet(QWidget* widget, const QString& source,
                                   ThemeType::ThemeMode theme, bool registerWidget) {
    setStyleSheet(widget, std::make_shared<StyleSheetFile>(source), theme, registerWidget);
}

void StyleSheetHelper::setCustomStyleSheet(QWidget* widget, const QString& lightQss, const QString& darkQss) {
    if (!widget) return;
    CustomStyleSheet(widget).setCustomStyleSheet(lightQss, darkQss);
}

void StyleSheetHelper::addStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                                   ThemeType::ThemeMode theme, bool registerWidget) {
    if (!widget) return;

    if (registerWidget) {
        StyleSheetManager::instance()->registerWidget(source, widget, false);
        QString qss = getStyleSheet(StyleSheetManager::instance()->source(widget), theme);
        widget->setStyleSheet(qss);
    } else {
        QString currentQss = widget->styleSheet();
        QString newQss = currentQss + "\n" + getStyleSheet(source, theme);
        widget->setStyleSheet(newQss);
    }
}

void StyleSheetHelper::addStyleSheet(QWidget* widget, const QString& source,
                                   ThemeType::ThemeMode theme, bool registerWidget) {
    addStyleSheet(widget, std::make_shared<StyleSheetFile>(source), theme, registerWidget);
}

// ==================== StyleSheetBase 实现 ====================
QString StyleSheetBase::path(ThemeType::ThemeMode theme) {
    Q_UNUSED(theme);
    return QString();
}


QString StyleSheetBase::content(ThemeType::ThemeMode theme) {
    return StyleSheetHelper::getStyleSheetFromFile(path(theme));
}

void StyleSheetBase::apply(QWidget* widget, ThemeType::ThemeMode theme) {
    StyleSheetManager::setStyleSheet(widget, std::make_shared<StyleSheetBase>(*this), theme);
}

// ==================== StyleSheetFile 实现 ====================

StyleSheetFile::StyleSheetFile(const QString& path)
    : m_lightPath(path), m_darkPath(path), m_filePath(path) {}

StyleSheetFile::StyleSheetFile(const QString& lightPath, const QString& darkPath)
    : m_lightPath(lightPath), m_darkPath(darkPath) {}

QString StyleSheetFile::path(ThemeType::ThemeMode theme) {
    ThemeType::ThemeMode actualTheme = (theme == ThemeType::ThemeMode::AUTO) ? Theme::instance()->theme() : theme;

    if (isMultiPath()) {
        return (actualTheme == ThemeType::ThemeMode::LIGHT) ? m_lightPath : m_darkPath;
    }

    // 向后兼容：使用单一路径
    return m_filePath.isEmpty() ? m_lightPath : m_filePath;
}

bool StyleSheetFile::isMultiPath() const {
    return m_lightPath != m_darkPath;
}

// ==================== TemplateStyleSheetFile 实现 ====================

TemplateStyleSheetFile::TemplateStyleSheetFile(const QString& templatePath)
    : m_templatePath(templatePath) {}

QString TemplateStyleSheetFile::path(ThemeType::ThemeMode theme) {
    ThemeType::ThemeMode actualTheme = (theme == ThemeType::ThemeMode::AUTO) ? Theme::instance()->theme() : theme;
    QString themeStr = (actualTheme == ThemeType::ThemeMode::LIGHT) ? "light" : "dark";

    QString result = m_templatePath;
    result.replace("{theme}", themeStr);
    return result;
}

// ==================== FluentStyleSheet 实现 ====================

FluentStyleSheet::FluentStyleSheet(ThemeType::ThemeStyle type) : m_type(type) {}

QString FluentStyleSheet::path(ThemeType::ThemeMode theme) {
    ThemeType::ThemeMode actualTheme = (theme == ThemeType::ThemeMode::AUTO) ? Theme::instance()->theme() : theme;
    QString themeStr = (actualTheme == ThemeType::ThemeMode::LIGHT) ? "light" : "dark";
    QString typeStr = typeToString(m_type);

    return QString(":/res/style/%1/%2.qss").arg(themeStr, typeStr);
}

QString FluentStyleSheet::typeToString(ThemeType::ThemeStyle type) {
    static QMap<ThemeType::ThemeStyle, QString> typeMap = {
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
        {ThemeType::ThemeStyle::TITLE_BAR ,"title_bar"}
    };

    return typeMap.value(type, "unknown");
}

// ==================== CustomStyleSheet 实现 ====================

const QString CustomStyleSheet::DARK_QSS_KEY = "darkCustomQss";
const QString CustomStyleSheet::LIGHT_QSS_KEY = "lightCustomQss";

CustomStyleSheet::CustomStyleSheet(QWidget* widget) : m_widget(widget) {}

QString CustomStyleSheet::path(ThemeType::ThemeMode theme) {
    Q_UNUSED(theme)
    return "";
}

QString CustomStyleSheet::content(ThemeType::ThemeMode theme) {
    ThemeType::ThemeMode actualTheme = (theme == ThemeType::ThemeMode::AUTO) ? Theme::instance()->theme() : theme;

    if (actualTheme == ThemeType::ThemeMode::LIGHT) {
        return lightStyleSheet();
    }
    return darkStyleSheet();
}

CustomStyleSheet* CustomStyleSheet::setCustomStyleSheet(const QString& lightQss, const QString& darkQss) {
    setLightStyleSheet(lightQss);
    setDarkStyleSheet(darkQss);
    return this;
}

CustomStyleSheet* CustomStyleSheet::setLightStyleSheet(const QString& qss) {
    if (m_widget) {
        m_widget->setProperty(LIGHT_QSS_KEY.toUtf8().constData(), qss);
    }
    return this;
}

CustomStyleSheet* CustomStyleSheet::setDarkStyleSheet(const QString& qss) {
    if (m_widget) {
        m_widget->setProperty(DARK_QSS_KEY.toUtf8().constData(), qss);
    }
    return this;
}

QString CustomStyleSheet::lightStyleSheet() const {
    if (!m_widget) return "";
    return m_widget->property(LIGHT_QSS_KEY.toUtf8().constData()).toString();
}

QString CustomStyleSheet::darkStyleSheet() const {
    if (!m_widget) return "";
    return m_widget->property(DARK_QSS_KEY.toUtf8().constData()).toString();
}

// ==================== StyleSheetCompose 实现 ====================

StyleSheetCompose::StyleSheetCompose() {}

StyleSheetCompose::StyleSheetCompose(const std::vector<std::shared_ptr<StyleSheetBase>>& sources)
    : m_sources(sources) {}

QString StyleSheetCompose::content(ThemeType::ThemeMode theme) {
    QString result;
    for (const auto& source : m_sources) {
        result += source->content(theme) + "\n";
    }
    return result;
}

void StyleSheetCompose::add(const std::shared_ptr<StyleSheetBase>& source) {
    if (!source || source.get() == this) {
        return;
    }

    for (const auto& existing : m_sources) {
        if (existing.get() == source.get()) {
            return;
        }
    }

    m_sources.push_back(source);
}

void StyleSheetCompose::remove(const std::shared_ptr<StyleSheetBase>& source) {
    m_sources.erase(
        std::remove_if(m_sources.begin(), m_sources.end(),
            [source](const std::shared_ptr<StyleSheetBase>& item) {
                return item.get() == source.get();
            }),
        m_sources.end());
}

// ==================== CustomStyleSheetWatcher 实现 ====================

CustomStyleSheetWatcher::CustomStyleSheetWatcher(QWidget* parent)
    : QObject(parent), m_watchedWidget(parent) {}

bool CustomStyleSheetWatcher::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() != QEvent::DynamicPropertyChange) {
        return QObject::eventFilter(obj, event);
    }

    QDynamicPropertyChangeEvent* propEvent = static_cast<QDynamicPropertyChangeEvent*>(event);
    QString name = QString::fromUtf8(propEvent->propertyName());

    if (name == CustomStyleSheet::LIGHT_QSS_KEY || name == CustomStyleSheet::DARK_QSS_KEY) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            StyleSheetHelper::addStyleSheet(widget, std::make_shared<CustomStyleSheet>(widget));
        }
    }

    return QObject::eventFilter(obj, event);
}

// ==================== StyleSheetManager 实现 ====================

StyleSheetManager* StyleSheetManager::m_instance = nullptr;

StyleSheetManager::StyleSheetManager() : QObject() {}

StyleSheetManager* StyleSheetManager::instance() {
    if (!m_instance) {
        m_instance = new StyleSheetManager();
    }
    return m_instance;
}

void StyleSheetManager::registerWidget(const std::shared_ptr<StyleSheetBase>& source,
                                      QWidget* widget, bool reset) {
    if (!widget) return;

    std::shared_ptr<StyleSheetBase> actualSource = source;
    if (!actualSource) {
        return;
    }

    if (!m_widgets.contains(widget)) {
        connect(widget, &QWidget::destroyed, this, [this, widget]() {
            deregisterWidget(widget);
        });

        widget->installEventFilter(new CustomStyleSheetWatcher(widget));
        auto compose = std::make_shared<StyleSheetCompose>();
        compose->add(actualSource);
        compose->add(std::make_shared<CustomStyleSheet>(widget));
        m_widgets[widget] = compose;
    } else {
        if (reset) {
            auto compose = std::make_shared<StyleSheetCompose>();
            compose->add(actualSource);
            compose->add(std::make_shared<CustomStyleSheet>(widget));
            m_widgets[widget] = compose;
        } else {
            m_widgets[widget]->add(actualSource);
        }
    }

    // 立即应用样式
    widget->setStyleSheet(StyleSheetHelper::getStyleSheet(m_widgets[widget]));
}

void StyleSheetManager::deregisterWidget(QWidget* widget) {
    m_widgets.remove(widget);
}

void StyleSheetManager::registerWidget(QWidget* widget, ThemeType::ThemeStyle type, bool reset)
{
    registerWidget(std::make_shared<FluentStyleSheet>(type), widget, reset);
}

std::shared_ptr<StyleSheetCompose> StyleSheetManager::source(QWidget* widget) const {
    auto it = m_widgets.find(widget);
    if (it != m_widgets.end()) {
        return it.value();
    }
    return std::make_shared<StyleSheetCompose>();
}

QList<QWidget*> StyleSheetManager::widgets() const {
    return m_widgets.keys();
}

void StyleSheetManager::updateStyleSheet(bool lazy) {
    QList<QWidget*> widgetsToRemove;

    for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
        QWidget* widget = it.key();
        if (!widget) {
            widgetsToRemove.append(widget);
            continue;
        }

        try {
            if (!lazy || widget->isVisible()) {
                auto source = it.value();
                setStyleSheet(widget, source, Theme::instance()->theme(), false);
            } else {
                widget->setProperty("dirty-qss", true);
            }
        } catch (...) {
            widgetsToRemove.append(widget);
        }
    }

    for (QWidget* widget : widgetsToRemove) {
        deregisterWidget(widget);
    }
}

// StyleSheetManager 的静态便捷方法
void StyleSheetManager::setStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                                     ThemeType::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::setStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::setStyleSheet(QWidget* widget, const QString& source,
                                     ThemeType::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::setStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::setCustomStyleSheet(QWidget* widget, const QString& lightQss, const QString& darkQss) {
    StyleSheetHelper::setCustomStyleSheet(widget, lightQss, darkQss);
}

void StyleSheetManager::addStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                                     ThemeType::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::addStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::addStyleSheet(QWidget* widget, const QString& source,
                                     ThemeType::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::addStyleSheet(widget, source, theme, registerWidget);
}
