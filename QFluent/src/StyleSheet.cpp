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

// 文件作用域的缓存版本号，确保 getThemeColorMap 和 clearThemeColorCache 使用同一个变量
namespace {
    static int s_cacheVersion = 0;
}

QHash<QString, QString> StyleSheetHelper::themeColorMap() {
    // 真正的静态缓存，只在第一次调用时初始化
    static QHash<QString, QString> s_colorMap;

    // 检查缓存是否需要更新
    static QColor s_cachedPrimaryColor;
    static int s_localVersion = -1;  // 初始化为 -1，确保第一次必定更新

    Theme* theme = Theme::instance();
    const QColor currentPrimaryColor = theme->themeColor(Fluent::ThemeColor::PRIMARY);

    // 如果主题色改变或版本号变化，更新缓存
    if (s_colorMap.isEmpty() || s_cachedPrimaryColor != currentPrimaryColor || s_localVersion != s_cacheVersion) {
        s_colorMap.clear();
        s_colorMap.reserve(7);

        s_colorMap.insert("--ThemeColorPrimary", currentPrimaryColor.name());
        s_colorMap.insert("--ThemeColorDark1", theme->themeColor(Fluent::ThemeColor::DARK_1).name());
        s_colorMap.insert("--ThemeColorDark2", theme->themeColor(Fluent::ThemeColor::DARK_2).name());
        s_colorMap.insert("--ThemeColorDark3", theme->themeColor(Fluent::ThemeColor::DARK_3).name());
        s_colorMap.insert("--ThemeColorLight1", theme->themeColor(Fluent::ThemeColor::LIGHT_1).name());
        s_colorMap.insert("--ThemeColorLight2", theme->themeColor(Fluent::ThemeColor::LIGHT_2).name());
        s_colorMap.insert("--ThemeColorLight3", theme->themeColor(Fluent::ThemeColor::LIGHT_3).name());

        s_cachedPrimaryColor = currentPrimaryColor;
        s_localVersion = s_cacheVersion;
    }

    return s_colorMap;
}

// 清除主题色缓存（在主题色改变时调用）
void StyleSheetHelper::clearThemeColorCache()
{
    // 递增版本号以强制缓存更新
    ++s_cacheVersion;
}

QString StyleSheetHelper::applyThemeColor(const QString& qss) {
    if (qss.isEmpty()) {
        return qss;
    }

    const QHash<QString, QString>& colorMap = themeColorMap();
    if (colorMap.isEmpty()) {
        return qss;
    }

    // 使用正则表达式一次性查找所有颜色变量
    static QRegularExpression colorVarRegex(
        "--ThemeColor(?:Primary|Dark[1-3]|Light[1-3])"
    );

    QString result = qss;
    // 预估结果大小以减少重新分配
    result.reserve(qss.size() + colorMap.size() * 10);

    // 查找所有匹配
    QRegularExpressionMatchIterator it = colorVarRegex.globalMatch(result);
    if (!it.hasNext()) {
        return result; // 没有颜色变量，直接返回
    }

    // 收集所有匹配和替换
    struct Replacement {
        int pos;
        int length;
        QString replacement;
    };
    QVector<Replacement> replacements;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString varName = match.captured();
        if (colorMap.contains(varName)) {
            replacements.append({
                static_cast<int>(match.capturedStart()),
                static_cast<int>(match.capturedLength()),
                colorMap.value(varName)
            });
        }
    }

    // 从后向前替换，避免位置变化
    for (int i = replacements.size() - 1; i >= 0; --i) {
        const auto& repl = replacements[i];
        result.replace(repl.pos, repl.length, repl.replacement);
    }

    return result;
}

QString StyleSheetHelper::styleSheetFromFile(const QString& filePath) {
    static QHash<QString, QString> s_cache;
    static QMutex s_cacheMutex;

    {
        QMutexLocker locker(&s_cacheMutex);
        if (s_cache.contains(filePath)) {
            return s_cache[filePath];
        }
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // qWarning() << "QSS文件打开失败:" << filePath;
        return QString();
    }

    QString content = file.readAll();
    file.close();

    {
        QMutexLocker locker(&s_cacheMutex);
        s_cache[filePath] = content;
    }
    return content;
}

QString StyleSheetHelper::styleSheet(const std::shared_ptr<StyleSheetBase>& source,
                                        Fluent::ThemeMode theme) {
    if (!source) {
        return QString();
    }
    return applyThemeColor(source->content(theme));
}

QString StyleSheetHelper::styleSheet(const QString& source, Fluent::ThemeMode theme) {
    return styleSheet(std::make_shared<StyleSheetFile>(source), theme);
}

void StyleSheetHelper::setStyleSheet(QWidget* widget,
                                     const std::shared_ptr<StyleSheetBase>& source,
                                     Fluent::ThemeMode theme, bool registerWidget) {
    if (!widget) {
        return;
    }

    if (registerWidget) {
        // 注册到管理器，管理器会负责设置样式表
        StyleSheetManager::instance()->registerWidget(source, widget);
    } else {
        // 直接设置样式表，不注册
        widget->setStyleSheet(styleSheet(source, theme));
    }
}

void StyleSheetHelper::setStyleSheet(QWidget* widget, const QString& source,
                                     Fluent::ThemeMode theme, bool registerWidget) {
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
                                     Fluent::ThemeMode theme, bool registerWidget) {
    if (!widget) {
        return;
    }

    if (registerWidget) {
        StyleSheetManager::instance()->registerWidget(source, widget, false);
        QString qss = styleSheet(StyleSheetManager::instance()->source(widget), theme);
        widget->setStyleSheet(qss);
    } else {
        const QString& currentQss = widget->styleSheet();
        QString newQss;
        newQss.reserve(currentQss.size() + source->content(theme).size() + 1);
        newQss = currentQss + "\n" + styleSheet(source, theme);
        widget->setStyleSheet(newQss);
    }
}

void StyleSheetHelper::addStyleSheet(QWidget* widget, const QString& source,
                                     Fluent::ThemeMode theme, bool registerWidget) {
    addStyleSheet(widget, std::make_shared<StyleSheetFile>(source), theme, registerWidget);
}

// ==================== StyleSheetBase 实现 ====================

QString StyleSheetBase::path(Fluent::ThemeMode theme) {
    Q_UNUSED(theme);
    return QString();
}

QString StyleSheetBase::content(Fluent::ThemeMode theme) {
    return StyleSheetHelper::styleSheetFromFile(path(theme));
}

void StyleSheetBase::apply(QWidget* widget, Fluent::ThemeMode theme) {
    StyleSheetHelper::setStyleSheet(widget, clone(), theme);
}


// ==================== StyleSheetFile 实现 ====================

StyleSheetFile::StyleSheetFile(const QString& path)
    : m_lightPath(path), m_darkPath(path) {}

StyleSheetFile::StyleSheetFile(const QString& lightPath, const QString& darkPath)
    : m_lightPath(lightPath), m_darkPath(darkPath) {}

QString StyleSheetFile::path(Fluent::ThemeMode theme) {
    Fluent::ThemeMode actualTheme = (theme == Fluent::ThemeMode::AUTO)
            ? Theme::instance()->theme() : theme;

    // 如果有分别的亮色和暗色路径，根据主题返回对应路径
    if (hasSeparatePaths()) {
        return (actualTheme == Fluent::ThemeMode::LIGHT) ? m_lightPath : m_darkPath;
    }

    // 否则使用亮色路径（两者相同）
    return m_lightPath;
}

std::shared_ptr<StyleSheetBase> StyleSheetFile::clone() const {
    return std::make_shared<StyleSheetFile>(*this);
}


// ==================== TemplateStyleSheetFile 实现 ====================

TemplateStyleSheetFile::TemplateStyleSheetFile(const QString& templatePath)
    : m_templatePath(templatePath) {}

QString TemplateStyleSheetFile::path(Fluent::ThemeMode theme) {
    Fluent::ThemeMode actualTheme = (theme == Fluent::ThemeMode::AUTO)
            ? Theme::instance()->theme() : theme;

    if (actualTheme == Fluent::ThemeMode::LIGHT) {
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

std::shared_ptr<StyleSheetBase> TemplateStyleSheetFile::clone() const {
    return std::make_shared<TemplateStyleSheetFile>(*this);
}


// ==================== FluentStyleSheet 实现 ====================

FluentStyleSheet::FluentStyleSheet(Fluent::ThemeStyle type) : m_type(type) {}

const QHash<Fluent::ThemeStyle, QString>& FluentStyleSheet::typeMap() {
    static QHash<Fluent::ThemeStyle, QString> typeMap;
    static bool initialized = false;

    if (!initialized) {
        typeMap.reserve(35); // 预留足够空间

        typeMap.insert(Fluent::ThemeStyle::MENU, "menu");
        typeMap.insert(Fluent::ThemeStyle::LABEL, "label");
        typeMap.insert(Fluent::ThemeStyle::PIVOT, "pivot");
        typeMap.insert(Fluent::ThemeStyle::BUTTON, "button");
        typeMap.insert(Fluent::ThemeStyle::DIALOG, "dialog");
        typeMap.insert(Fluent::ThemeStyle::SLIDER, "slider");
        typeMap.insert(Fluent::ThemeStyle::INFO_BAR, "info_bar");
        typeMap.insert(Fluent::ThemeStyle::SPIN_BOX, "spin_box");
        typeMap.insert(Fluent::ThemeStyle::TAB_VIEW, "tab_view");
        typeMap.insert(Fluent::ThemeStyle::TOOL_TIP, "tool_tip");
        typeMap.insert(Fluent::ThemeStyle::CHECK_BOX, "check_box");
        typeMap.insert(Fluent::ThemeStyle::COMBO_BOX, "combo_box");
        typeMap.insert(Fluent::ThemeStyle::FLIP_VIEW, "flip_view");
        typeMap.insert(Fluent::ThemeStyle::LINE_EDIT, "line_edit");
        typeMap.insert(Fluent::ThemeStyle::LIST_VIEW, "list_view");
        typeMap.insert(Fluent::ThemeStyle::TREE_VIEW, "tree_view");
        typeMap.insert(Fluent::ThemeStyle::INFO_BADGE, "info_badge");
        typeMap.insert(Fluent::ThemeStyle::PIPS_PAGER, "pips_pager");
        typeMap.insert(Fluent::ThemeStyle::TABLE_VIEW, "table_view");
        typeMap.insert(Fluent::ThemeStyle::CARD_WIDGET, "card_widget");
        typeMap.insert(Fluent::ThemeStyle::TIME_PICKER, "time_picker");
        typeMap.insert(Fluent::ThemeStyle::COLOR_DIALOG, "color_dialog");
        typeMap.insert(Fluent::ThemeStyle::MEDIA_PLAYER, "media_player");
        typeMap.insert(Fluent::ThemeStyle::SETTING_CARD, "setting_card");
        typeMap.insert(Fluent::ThemeStyle::TEACHING_TIP, "teaching_tip");
        typeMap.insert(Fluent::ThemeStyle::SWITCH_BUTTON, "switch_button");
        typeMap.insert(Fluent::ThemeStyle::MESSAGE_DIALOG, "message_dialog");
        typeMap.insert(Fluent::ThemeStyle::STATE_TOOL_TIP, "state_tool_tip");
        typeMap.insert(Fluent::ThemeStyle::CALENDAR_PICKER, "calendar_picker");
        typeMap.insert(Fluent::ThemeStyle::FOLDER_LIST_DIALOG, "folder_list_dialog");
        typeMap.insert(Fluent::ThemeStyle::SETTING_CARD_GROUP, "setting_card_group");
        typeMap.insert(Fluent::ThemeStyle::EXPAND_SETTING_CARD, "expand_setting_card");
        typeMap.insert(Fluent::ThemeStyle::NAVIGATION_INTERFACE, "navigation_interface");

        initialized = true;
    }

    return typeMap;
}

QString FluentStyleSheet::path(Fluent::ThemeMode theme) {
    Fluent::ThemeMode actualTheme = (theme == Fluent::ThemeMode::AUTO)
            ? Theme::instance()->theme() : theme;

    const QString& themeStr = (actualTheme == Fluent::ThemeMode::LIGHT) ?
                QStringLiteral("light") : QStringLiteral("dark");
    const QString& typeStr = typeToString(m_type);

    return QString(":/qfluent/style/%1/%2.qss").arg(themeStr, typeStr);
}

QString FluentStyleSheet::typeToString(Fluent::ThemeStyle type) {
    return typeMap().value(type, "unknown");
}

std::shared_ptr<StyleSheetBase> FluentStyleSheet::clone() const {
    return std::make_shared<FluentStyleSheet>(*this);
}


// ==================== CustomStyleSheet 实现 ====================

const char* CustomStyleSheet::DARK_QSS_KEY = "darkCustomQss";
const char* CustomStyleSheet::LIGHT_QSS_KEY = "lightCustomQss";

CustomStyleSheet::CustomStyleSheet(QWidget* widget) : m_widget(widget) {}

QString CustomStyleSheet::path(Fluent::ThemeMode theme) {
    Q_UNUSED(theme)
    return QString();
}

QString CustomStyleSheet::content(Fluent::ThemeMode theme) {
    Fluent::ThemeMode actualTheme = (theme == Fluent::ThemeMode::AUTO)
            ? Theme::instance()->theme() : theme;

    return (actualTheme == Fluent::ThemeMode::LIGHT) ?
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

std::shared_ptr<StyleSheetBase> CustomStyleSheet::clone() const {
    return std::make_shared<CustomStyleSheet>(*this);
}

void CustomStyleSheet::apply(QWidget* widget, Fluent::ThemeMode theme) {
    if (!widget) {
        return;
    }

    // 如果目标widget不是当前widget，复制自定义样式表属性
    if (widget != m_widget) {
        if (m_widget) {
            // 复制当前的自定义样式表到目标widget
            widget->setProperty(LIGHT_QSS_KEY, lightStyleSheet());
            widget->setProperty(DARK_QSS_KEY, darkStyleSheet());
        }
    }

    // 创建新的CustomStyleSheet实例并应用
    StyleSheetHelper::setStyleSheet(widget, std::make_shared<CustomStyleSheet>(widget), theme);
}

// ==================== StyleSheetCompose 实现 ====================

StyleSheetCompose::StyleSheetCompose() {
    m_sources.reserve(4); // 预留合理的初始容量
}

StyleSheetCompose::StyleSheetCompose(const std::vector<std::shared_ptr<StyleSheetBase>>& sources)
    : m_sources(sources) {}

StyleSheetCompose::StyleSheetCompose(std::vector<std::shared_ptr<StyleSheetBase>>&& sources)
    : m_sources(std::move(sources)) {}

QString StyleSheetCompose::content(Fluent::ThemeMode theme) {
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

std::shared_ptr<StyleSheetBase> StyleSheetCompose::clone() const {
    auto clone = std::make_shared<StyleSheetCompose>();
    clone->m_sources.reserve(m_sources.size());
    for (const auto& source : m_sources) {
        if (source) {
            clone->m_sources.push_back(source->clone());
        }
    }
    return clone;
}


// ==================== CustomStyleSheetWatcher 实现 ====================

CustomStyleSheetWatcher::CustomStyleSheetWatcher(QWidget* parent)
    : QObject(parent), m_watchedWidget(parent), m_isDirty(false) {}

bool CustomStyleSheetWatcher::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() != QEvent::DynamicPropertyChange) {
        // 处理显示事件，用于 lazy 模式下的样式表更新
        if (event->type() == QEvent::Show) {
            applyStyleSheetIfNeeded();
        }
        return QObject::eventFilter(obj, event);
    }

    auto* propEvent = static_cast<QDynamicPropertyChangeEvent*>(event);
    const QByteArray& propName = propEvent->propertyName();

    if (propName == CustomStyleSheet::LIGHT_QSS_KEY ||
            propName == CustomStyleSheet::DARK_QSS_KEY) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            // 标记为脏，触发样式表更新
            // CustomStyleSheet 已经存在于组合中，会读取新的属性值
            markDirty();
            applyStyleSheetIfNeeded();
        }
    }

    return QObject::eventFilter(obj, event);
}

void CustomStyleSheetWatcher::applyStyleSheetIfNeeded()
{
    if (!m_isDirty || !m_watchedWidget) {
        return;
    }

    auto* widget = m_watchedWidget;
    auto compose = StyleSheetManager::instance()->source(widget);
    if (compose && compose->size() > 0) {
        const QString qss = StyleSheetHelper::styleSheet(compose, Theme::instance()->theme());
        widget->setStyleSheet(qss);
    }

    clearDirty();
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

    QMutexLocker locker(&m_mutex);

    if (!m_widgets.contains(widget)) {
        connect(widget, &QWidget::destroyed, this, [this, widget]() {
            deregisterWidget(widget);
        });

        auto watcher = new CustomStyleSheetWatcher(widget);
        widget->installEventFilter(watcher);

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
    widget->setStyleSheet(StyleSheetHelper::styleSheet(m_widgets[widget]));
}

void StyleSheetManager::deregisterWidget(QWidget* widget) {
    if (!widget) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    // 移除所有 CustomStyleSheetWatcher 事件过滤器
    QList<CustomStyleSheetWatcher*> watchers = widget->findChildren<CustomStyleSheetWatcher*>();
    for (auto* watcher : watchers) {
        widget->removeEventFilter(watcher);
        watcher->deleteLater();
    }

    m_widgets.remove(widget);
}

void StyleSheetManager::registerWidget(QWidget* widget, Fluent::ThemeStyle type, bool reset) {
    registerWidget(std::make_shared<FluentStyleSheet>(type), widget, reset);
}

std::shared_ptr<StyleSheetCompose> StyleSheetManager::source(QWidget* widget) const {
    QMutexLocker locker(&m_mutex);
    auto it = m_widgets.constFind(widget);
    if (it != m_widgets.constEnd()) {
        return it.value();
    }
    return std::make_shared<StyleSheetCompose>();
}

QList<QWidget*> StyleSheetManager::widgets() const {
    QMutexLocker locker(&m_mutex);
    return m_widgets.keys();
}

bool StyleSheetManager::isRegistered(QWidget* widget) const {
    QMutexLocker locker(&m_mutex);
    return m_widgets.contains(widget);
}

void StyleSheetManager::updateStyleSheet(bool lazy) {
    QList<QWidget*> widgetsToRemove;
    widgetsToRemove.reserve(10);

    // 复制数据以避免持有锁时进行复杂操作
    QList<QWidget*> widgetKeys;
    QList<std::shared_ptr<StyleSheetCompose>> widgetValues;

    {
        QMutexLocker locker(&m_mutex);
        widgetKeys = m_widgets.keys();
        widgetValues.reserve(widgetKeys.size());
        for (const auto& widget : widgetKeys) {
            widgetValues.append(m_widgets.value(widget));
        }
    }

    for (int i = 0; i < widgetKeys.size(); ++i) {
        QWidget* widget = widgetKeys[i];
        const auto& source = widgetValues[i];

        if (!widget) {
            widgetsToRemove.append(widget);
            continue;
        }

        if (!lazy || widget->isVisible()) {
            // 可见或非懒加载模式：立即更新样式表
            setStyleSheet(widget, source, Theme::instance()->theme(), false);
        } else {
            // 修复：懒加载模式 - 设置 dirty 标记，等待 widget 变为可见时更新
            // 通过 CustomStyleSheetWatcher 的 eventFilter 在 showEvent 时触发更新
            if (auto* watcher = widget->findChild<CustomStyleSheetWatcher*>()) {
                watcher->markDirty();
            }
        }
    }

    for (QWidget* widget : widgetsToRemove) {
        deregisterWidget(widget);
    }
}

// StyleSheetManager 的静态便捷方法
void StyleSheetManager::setStyleSheet(QWidget* widget,
                                      const std::shared_ptr<StyleSheetBase>& source,
                                      Fluent::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::setStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::setStyleSheet(QWidget* widget, const QString& source,
                                      Fluent::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::setStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::setCustomStyleSheet(QWidget* widget, const QString& lightQss,
                                            const QString& darkQss) {
    StyleSheetHelper::setCustomStyleSheet(widget, lightQss, darkQss);
}

void StyleSheetManager::addStyleSheet(QWidget* widget,
                                      const std::shared_ptr<StyleSheetBase>& source,
                                      Fluent::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::addStyleSheet(widget, source, theme, registerWidget);
}

void StyleSheetManager::addStyleSheet(QWidget* widget, const QString& source,
                                      Fluent::ThemeMode theme, bool registerWidget) {
    StyleSheetHelper::addStyleSheet(widget, source, theme, registerWidget);
}
