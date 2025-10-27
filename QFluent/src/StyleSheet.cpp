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
#include <QMutex>
#include <QMutexLocker>
#include <memory>
#include <vector>
#include <algorithm>

#include "Theme.h"

// ==================== StyleSheetHelper 实现 ====================

QHash<QString, QString> StyleSheetHelper::getThemeColorMap() {
    static QHash<QString, QString> colorMap;
    static bool initialized = false;

    if (!initialized) {
        Theme* theme = Theme::instance();
        colorMap.reserve(7); // 预留空间

        colorMap.insert("--ThemeColorPrimary",
            theme->themeColor(ThemeType::ThemeColor::PRIMARY).name());
        colorMap.insert("--ThemeColorDark1",
            theme->themeColor(ThemeType::ThemeColor::DARK_1).name());
        colorMap.insert("--ThemeColorDark2",
            theme->themeColor(ThemeType::ThemeColor::DARK_2).name());
        colorMap.insert("--ThemeColorDark3",
            theme->themeColor(ThemeType::ThemeColor::DARK_3).name());
        colorMap.insert("--ThemeColorLight1",
            theme->themeColor(ThemeType::ThemeColor::LIGHT_1).name());
        colorMap.insert("--ThemeColorLight2",
            theme->themeColor(ThemeType::ThemeColor::LIGHT_2).name());
        colorMap.insert("--ThemeColorLight3",
            theme->themeColor(ThemeType::ThemeColor::LIGHT_3).name());

        initialized = true;
    }

    return colorMap;
}

QString StyleSheetHelper::applyThemeColor(const QString& qss) {
    if (qss.isEmpty()) {
        return qss;
    }

    QString result = qss;
    const QHash<QString, QString>& colorMap = getThemeColorMap();

    // 预估结果大小以减少重新分配
    result.reserve(qss.size() + colorMap.size() * 10);

    // 使用QHash的迭代器进行高效查找和替换
    for (auto it = colorMap.constBegin(); it != colorMap.constEnd(); ++it) {
        if (result.contains(it.key())) {
            result.replace(it.key(), it.value());
        }
    }

    return result;
}

QString StyleSheetHelper::getStyleSheetFromFile(const QString& filePath) {
    if (filePath.isEmpty()) {
        return QString();
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open QSS file:" << filePath;
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

QString StyleSheetHelper::getStyleSheet(const std::shared_ptr<StyleSheetBase>& source,
                                       ThemeType::ThemeMode theme) {
    if (!source) {
        return QString();
    }
    return applyThemeColor(source->content(theme));
}

QString StyleSheetHelper::getStyleSheet(const QString& source, ThemeType::ThemeMode theme) {
    return getStyleSheet(std::make_shared<StyleSheetFile>(source), theme);
}

void StyleSheetHelper::setStyleSheet(QWidget* widget,
                                    const std::shared_ptr<StyleSheetBase>& source,
                                    ThemeType::ThemeMode theme, bool registerWidget) {
    if (!widget) {
        return;
    }

    if (registerWidget) {
        StyleSheetManager::instance()->registerWidget(source, widget);
    }

    widget->setStyleSheet(getStyleSheet(source, theme));
}

void StyleSheetHelper::setStyleSheet(QWidget* widget, const QString& source,
                                    ThemeType::ThemeMode theme, bool registerWidget) {
    setStyleSheet(widget, std::make_shared<StyleSheetFile>(source), theme, registerWidget);
}

void StyleSheetHelper::setCustomStyleSheet(QWidget* widget, const QString& lightQss,
                                          const QString& darkQss) {
    if (!widget) {
        return;
    }
    CustomStyleSheet(widget).setCustomStyleSheet(lightQss, darkQss);
}

void StyleSheetHelper::addStyleSheet(QWidget* widget,
                                    const std::shared_ptr<StyleSheetBase>& source,
                                    ThemeType::ThemeMode theme, bool registerWidget) {
    if (!widget) {
        return;
    }

    if (registerWidget) {
        StyleSheetManager::instance()->registerWidget(source, widget, false);
        QString qss = getStyleSheet(StyleSheetManager::instance()->source(widget), theme);
        widget->setStyleSheet(qss);
    } else {
        const QString& currentQss = widget->styleSheet();
        QString newQss;
        newQss.reserve(currentQss.size() + source->content(theme).size() + 1);
        newQss = currentQss + "\n" + getStyleSheet(source, theme);
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
    : m_lightPath(path), m_darkPath(path), m_filePath(path), m_isMultiPath(false) {}

StyleSheetFile::StyleSheetFile(const QString& lightPath, const QString& darkPath)
    : m_lightPath(lightPath), m_darkPath(darkPath), m_isMultiPath(lightPath != darkPath) {}

QString StyleSheetFile::path(ThemeType::ThemeMode theme) {
    ThemeType::ThemeMode actualTheme = (theme == ThemeType::ThemeMode::AUTO)
        ? Theme::instance()->theme() : theme;

    if (m_isMultiPath) {
        return (actualTheme == ThemeType::ThemeMode::LIGHT) ? m_lightPath : m_darkPath;
    }

    // 向后兼容：使用单一路径
    return m_filePath.isEmpty() ? m_lightPath : m_filePath;
}

// ==================== TemplateStyleSheetFile 实现 ====================

TemplateStyleSheetFile::TemplateStyleSheetFile(const QString& templatePath)
    : m_templatePath(templatePath) {}

QString TemplateStyleSheetFile::path(ThemeType::ThemeMode theme) {
    ThemeType::ThemeMode actualTheme = (theme == ThemeType::ThemeMode::AUTO)
        ? Theme::instance()->theme() : theme;

    // 使用缓存避免重复字符串操作
    if (actualTheme == ThemeType::ThemeMode::LIGHT) {
        if (m_cachedLightPath.isEmpty()) {
            m_cachedLightPath = m_templatePath;
            m_cachedLightPath.replace("{theme}", "light");
        }
        return m_cachedLightPath;
    } else {
        if (m_cachedDarkPath.isEmpty()) {
            m_cachedDarkPath = m_templatePath;
            m_cachedDarkPath.replace("{theme}", "dark");
        }
        return m_cachedDarkPath;
    }
}

// ==================== FluentStyleSheet 实现 ====================

FluentStyleSheet::FluentStyleSheet(ThemeType::ThemeStyle type) : m_type(type) {}

const QHash<ThemeType::ThemeStyle, QString>& FluentStyleSheet::getTypeMap() {
    static QHash<ThemeType::ThemeStyle, QString> typeMap;
    static bool initialized = false;

    if (!initialized) {
        typeMap.reserve(35); // 预留足够空间

        typeMap.insert(ThemeType::ThemeStyle::MENU, "menu");
        typeMap.insert(ThemeType::ThemeStyle::LABEL, "label");
        typeMap.insert(ThemeType::ThemeStyle::PIVOT, "pivot");
        typeMap.insert(ThemeType::ThemeStyle::BUTTON, "button");
        typeMap.insert(ThemeType::ThemeStyle::DIALOG, "dialog");
        typeMap.insert(ThemeType::ThemeStyle::SLIDER, "slider");
        typeMap.insert(ThemeType::ThemeStyle::INFO_BAR, "info_bar");
        typeMap.insert(ThemeType::ThemeStyle::SPIN_BOX, "spin_box");
        typeMap.insert(ThemeType::ThemeStyle::TAB_VIEW, "tab_view");
        typeMap.insert(ThemeType::ThemeStyle::TOOL_TIP, "tool_tip");
        typeMap.insert(ThemeType::ThemeStyle::CHECK_BOX, "check_box");
        typeMap.insert(ThemeType::ThemeStyle::COMBO_BOX, "combo_box");
        typeMap.insert(ThemeType::ThemeStyle::FLIP_VIEW, "flip_view");
        typeMap.insert(ThemeType::ThemeStyle::LINE_EDIT, "line_edit");
        typeMap.insert(ThemeType::ThemeStyle::LIST_VIEW, "list_view");
        typeMap.insert(ThemeType::ThemeStyle::TREE_VIEW, "tree_view");
        typeMap.insert(ThemeType::ThemeStyle::INFO_BADGE, "info_badge");
        typeMap.insert(ThemeType::ThemeStyle::PIPS_PAGER, "pips_pager");
        typeMap.insert(ThemeType::ThemeStyle::TABLE_VIEW, "table_view");
        typeMap.insert(ThemeType::ThemeStyle::CARD_WIDGET, "card_widget");
        typeMap.insert(ThemeType::ThemeStyle::TIME_PICKER, "time_picker");
        typeMap.insert(ThemeType::ThemeStyle::COLOR_DIALOG, "color_dialog");
        typeMap.insert(ThemeType::ThemeStyle::MEDIA_PLAYER, "media_player");
        typeMap.insert(ThemeType::ThemeStyle::SETTING_CARD, "setting_card");
        typeMap.insert(ThemeType::ThemeStyle::TEACHING_TIP, "teaching_tip");
        typeMap.insert(ThemeType::ThemeStyle::FLUENT_WINDOW, "fluent_window");
        typeMap.insert(ThemeType::ThemeStyle::SWITCH_BUTTON, "switch_button");
        typeMap.insert(ThemeType::ThemeStyle::MESSAGE_DIALOG, "message_dialog");
        typeMap.insert(ThemeType::ThemeStyle::STATE_TOOL_TIP, "state_tool_tip");
        typeMap.insert(ThemeType::ThemeStyle::CALENDAR_PICKER, "calendar_picker");
        typeMap.insert(ThemeType::ThemeStyle::FOLDER_LIST_DIALOG, "folder_list_dialog");
        typeMap.insert(ThemeType::ThemeStyle::SETTING_CARD_GROUP, "setting_card_group");
        typeMap.insert(ThemeType::ThemeStyle::EXPAND_SETTING_CARD, "expand_setting_card");
        typeMap.insert(ThemeType::ThemeStyle::NAVIGATION_INTERFACE, "navigation_interface");
        typeMap.insert(ThemeType::ThemeStyle::TITLE_BAR, "title_bar");

        initialized = true;
    }

    return typeMap;
}

QString FluentStyleSheet::path(ThemeType::ThemeMode theme) {
    ThemeType::ThemeMode actualTheme = (theme == ThemeType::ThemeMode::AUTO)
        ? Theme::instance()->theme() : theme;

    const QString& themeStr = (actualTheme == ThemeType::ThemeMode::LIGHT) ?
        QStringLiteral("light") : QStringLiteral("dark");
    const QString& typeStr = typeToString(m_type);

    return QString(":/res/style/%1/%2.qss").arg(themeStr, typeStr);
}

QString FluentStyleSheet::typeToString(ThemeType::ThemeStyle type) {
    return getTypeMap().value(type, "unknown");
}

// ==================== CustomStyleSheet 实现 ====================

const char* CustomStyleSheet::DARK_QSS_KEY = "darkCustomQss";
const char* CustomStyleSheet::LIGHT_QSS_KEY = "lightCustomQss";

CustomStyleSheet::CustomStyleSheet(QWidget* widget) : m_widget(widget) {}

QString CustomStyleSheet::path(ThemeType::ThemeMode theme) {
    Q_UNUSED(theme)
    return QString();
}

QString CustomStyleSheet::content(ThemeType::ThemeMode theme) {
    ThemeType::ThemeMode actualTheme = (theme == ThemeType::ThemeMode::AUTO)
        ? Theme::instance()->theme() : theme;

    return (actualTheme == ThemeType::ThemeMode::LIGHT) ?
        lightStyleSheet() : darkStyleSheet();
}

CustomStyleSheet* CustomStyleSheet::setCustomStyleSheet(const QString& lightQss,
                                                        const QString& darkQss) {
    setLightStyleSheet(lightQss);
    setDarkStyleSheet(darkQss);
    return this;
}

CustomStyleSheet* CustomStyleSheet::setLightStyleSheet(const QString& qss) {
    if (m_widget) {
        m_widget->setProperty(LIGHT_QSS_KEY, qss);
    }
    return this;
}

CustomStyleSheet* CustomStyleSheet::setDarkStyleSheet(const QString& qss) {
    if (m_widget) {
        m_widget->setProperty(DARK_QSS_KEY, qss);
    }
    return this;
}

QString CustomStyleSheet::lightStyleSheet() const {
    return m_widget ? m_widget->property(LIGHT_QSS_KEY).toString() : QString();
}

QString CustomStyleSheet::darkStyleSheet() const {
    return m_widget ? m_widget->property(DARK_QSS_KEY).toString() : QString();
}

// ==================== StyleSheetCompose 实现 ====================

StyleSheetCompose::StyleSheetCompose() {
    m_sources.reserve(4); // 预留合理的初始容量
}

StyleSheetCompose::StyleSheetCompose(const std::vector<std::shared_ptr<StyleSheetBase>>& sources)
    : m_sources(sources) {}

StyleSheetCompose::StyleSheetCompose(std::vector<std::shared_ptr<StyleSheetBase>>&& sources)
    : m_sources(std::move(sources)) {}

QString StyleSheetCompose::content(ThemeType::ThemeMode theme) {
    if (m_sources.empty()) {
        return QString();
    }

    QString result;
    // 预估总大小以减少重新分配
    result.reserve(m_sources.size() * 500);

    for (const auto& source : m_sources) {
        if (source) {
            result += source->content(theme);
            result += '\n';
        }
    }

    return result;
}

void StyleSheetCompose::add(const std::shared_ptr<StyleSheetBase>& source) {
    if (!source || source.get() == this) {
        return;
    }

    // 使用std::find_if代替手动循环
    auto it = std::find_if(m_sources.begin(), m_sources.end(),
        [source](const std::shared_ptr<StyleSheetBase>& existing) {
            return existing.get() == source.get();
        });

    if (it == m_sources.end()) {
        m_sources.push_back(source);
    }
}

void StyleSheetCompose::remove(const std::shared_ptr<StyleSheetBase>& source) {
    auto new_end = std::remove_if(m_sources.begin(), m_sources.end(),
        [source](const std::shared_ptr<StyleSheetBase>& item) {
            return item.get() == source.get();
        });
    m_sources.erase(new_end, m_sources.end());
}

void StyleSheetCompose::reserve(size_t capacity) {
    m_sources.reserve(capacity);
}

// ==================== CustomStyleSheetWatcher 实现 ====================

CustomStyleSheetWatcher::CustomStyleSheetWatcher(QWidget* parent)
    : QObject(parent), m_watchedWidget(parent) {}

bool CustomStyleSheetWatcher::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() != QEvent::DynamicPropertyChange) {
        return QObject::eventFilter(obj, event);
    }

    auto* propEvent = static_cast<QDynamicPropertyChangeEvent*>(event);
    const QByteArray& propName = propEvent->propertyName();

    if (propName == CustomStyleSheet::LIGHT_QSS_KEY ||
        propName == CustomStyleSheet::DARK_QSS_KEY) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            StyleSheetHelper::addStyleSheet(widget,
                std::make_shared<CustomStyleSheet>(widget));
        }
    }

    return QObject::eventFilter(obj, event);
}

// ==================== StyleSheetManager 实现 ====================

StyleSheetManager* StyleSheetManager::m_instance = nullptr;
QMutex StyleSheetManager::m_mutex;

StyleSheetManager::StyleSheetManager() : QObject() {
    m_widgets.reserve(100); // 预留合理的初始容量
}

StyleSheetManager* StyleSheetManager::instance() {
    if (!m_instance) {
        QMutexLocker locker(&m_mutex);
        if (!m_instance) { // 双重检查锁定
            m_instance = new StyleSheetManager();
        }
    }
    return m_instance;
}

void StyleSheetManager::registerWidget(const std::shared_ptr<StyleSheetBase>& source,
                                      QWidget* widget, bool reset) {
    if (!widget || !source) {
        return;
    }

    if (!m_widgets.contains(widget)) {
        connect(widget, &QWidget::destroyed, this, [this, widget]() {
            deregisterWidget(widget);
        });

        widget->installEventFilter(new CustomStyleSheetWatcher(widget));

        auto compose = std::make_shared<StyleSheetCompose>();
        compose->reserve(2);
        compose->add(source);
        compose->add(std::make_shared<CustomStyleSheet>(widget));
        m_widgets.insert(widget, compose);
    } else {
        auto& compose = m_widgets[widget];
        if (reset) {
            auto newCompose = std::make_shared<StyleSheetCompose>();
            newCompose->reserve(2);
            newCompose->add(source);
            newCompose->add(std::make_shared<CustomStyleSheet>(widget));
            m_widgets[widget] = newCompose;
        } else {
            compose->add(source);
        }
    }

    // 立即应用样式
    widget->setStyleSheet(StyleSheetHelper::getStyleSheet(m_widgets[widget]));
}

void StyleSheetManager::deregisterWidget(QWidget* widget) {
    m_widgets.remove(widget);
}

void StyleSheetManager::registerWidget(QWidget* widget, ThemeType::ThemeStyle type, bool reset) {
    registerWidget(std::make_shared<FluentStyleSheet>(type), widget, reset);
}

std::shared_ptr<StyleSheetCompose> StyleSheetManager::source(QWidget* widget) const {
    auto it = m_widgets.constFind(widget);
    if (it != m_widgets.constEnd()) {
        return it.value();
    }
    return std::make_shared<StyleSheetCompose>();
}

QList<QWidget*> StyleSheetManager::widgets() const {
    return m_widgets.keys();
}

bool StyleSheetManager::isRegistered(QWidget* widget) const {
    return m_widgets.contains(widget);
}

void StyleSheetManager::updateStyleSheet(bool lazy) {
    QList<QWidget*> widgetsToRemove;
    widgetsToRemove.reserve(10);

    for (auto it = m_widgets.constBegin(); it != m_widgets.constEnd(); ++it) {
        QWidget* widget = it.key();
        if (!widget) {
            widgetsToRemove.append(widget);
            continue;
        }

        try {
            if (!lazy || widget->isVisible()) {
                const auto& source = it.value();
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
void StyleSheetManager::setStyleSheet(QWidget* widget,
                                     const std::shared_ptr<StyleSheetBase>& source,
                                     ThemeType::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::setStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::setStyleSheet(QWidget* widget, const QString& source,
                                     ThemeType::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::setStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::setCustomStyleSheet(QWidget* widget, const QString& lightQss,
                                           const QString& darkQss) {
    StyleSheetHelper::setCustomStyleSheet(widget, lightQss, darkQss);
}

void StyleSheetManager::addStyleSheet(QWidget* widget,
                                     const std::shared_ptr<StyleSheetBase>& source,
                                     ThemeType::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::addStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::addStyleSheet(QWidget* widget, const QString& source,
                                     ThemeType::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::addStyleSheet(widget, source, theme, registerWidget);
}
