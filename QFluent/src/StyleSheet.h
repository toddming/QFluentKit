#pragma once

#include <QObject>
#include <QMap>
#include <QHash>
#include <QPointer>
#include <memory>
#include <vector>

#include "FluentGlobal.h"

class QWidget;
class StyleSheetBase;
class StyleSheetCompose;
class StyleSheetManager;

class QFLUENT_EXPORT StyleSheet {
public:
    // 注册 — widget 永远在第一个参数
    static void registerWidget(QWidget* widget, Fluent::ThemeStyle type, bool reset = true);
    static void registerWidget(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source, bool reset = true);
    static void registerWidget(QWidget* widget, const QString& templatePath, bool reset = true);

    // 反注册
    static void deregisterWidget(QWidget* widget);

    // 设置/添加样式表
    static void setStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                             Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                             bool registerWidget = true);
    static void setStyleSheet(QWidget* widget, const QString& source,
                             Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                             bool registerWidget = true);

    static void setCustomStyleSheet(QWidget* widget, const QString& lightQss,
                                   const QString& darkQss);

    static void addStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                             Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                             bool registerWidget = true);
    static void addStyleSheet(QWidget* widget, const QString& source,
                             Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                             bool registerWidget = true);

    // 查询
    static bool isRegistered(QWidget* widget);
    static std::shared_ptr<StyleSheetCompose> source(QWidget* widget);

    // 全局更新
    static void updateStyleSheet(bool lazy = false);

    // 缓存管理
    static void clearThemeColorCache();

    // 样式表内容处理
    static QString applyThemeColor(const QString& qss);
    static QString styleSheetFromFile(const QString& filePath);

    static QString styleSheet(const std::shared_ptr<StyleSheetBase>& source,
                              Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);
    static QString styleSheet(const QString& source,
                              Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);

private:
    static QHash<QString, QString> themeColorMap();
};

class QFLUENT_EXPORT StyleSheetBase {
public:
    virtual ~StyleSheetBase() = default;
    virtual QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const;
    virtual QString content(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const;

    // 应用样式表到控件 - 使用clone()避免切片问题
    virtual void apply(QWidget* widget, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);

    // 克隆接口 - 修复切片问题
    virtual std::shared_ptr<StyleSheetBase> clone() const = 0;
};

class QFLUENT_EXPORT StyleSheetFile : public StyleSheetBase {
private:
    QString m_lightPath;
    QString m_darkPath;

public:
    // 单路径构造函数 - 用于亮色和暗色使用同一文件的情况
    explicit StyleSheetFile(const QString& path);

    // 双路径构造函数 - 分别指定亮色和暗色样式文件
    StyleSheetFile(const QString& lightPath, const QString& darkPath);

    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;
    inline bool hasSeparatePaths() const { return !m_lightPath.isEmpty() && !m_darkPath.isEmpty() && m_lightPath != m_darkPath; }

    const QString& lightPath() const { return m_lightPath; }
    const QString& darkPath() const { return m_darkPath; }

    std::shared_ptr<StyleSheetBase> clone() const override;
};

class QFLUENT_EXPORT TemplateStyleSheetFile : public StyleSheetBase {
private:
    QString m_templatePath;
    mutable QString m_cachedLightPath;
    mutable QString m_cachedDarkPath;

public:
    explicit TemplateStyleSheetFile(const QString& templatePath);
    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;

    std::shared_ptr<StyleSheetBase> clone() const override;
};

class QFLUENT_EXPORT FluentStyleSheet : public StyleSheetBase {
private:
    Fluent::ThemeStyle m_type;

    // 静态缓存类型到字符串的映射
    static const QMap<Fluent::ThemeStyle, QString>& typeMap();

public:
    explicit FluentStyleSheet(Fluent::ThemeStyle type);
    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;
    static QString typeToString(Fluent::ThemeStyle type);

    std::shared_ptr<StyleSheetBase> clone() const override;
};

class QFLUENT_EXPORT CustomStyleSheet : public StyleSheetBase {
private:
    QPointer<QWidget> m_widget;

public:
    explicit CustomStyleSheet(QWidget* widget);
    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;
    QString content(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;

    CustomStyleSheet* setCustomStyleSheet(const QString& lightQss, const QString& darkQss);
    CustomStyleSheet* setLightStyleSheet(const QString& qss);
    CustomStyleSheet* setDarkStyleSheet(const QString& qss);

    QString lightStyleSheet() const;
    QString darkStyleSheet() const;

    static const char* DARK_QSS_KEY;
    static const char* LIGHT_QSS_KEY;

    std::shared_ptr<StyleSheetBase> clone() const override;
    void apply(QWidget* widget, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
};

class QFLUENT_EXPORT StyleSheetCompose : public StyleSheetBase {
private:
    std::vector<std::shared_ptr<StyleSheetBase>> m_sources;

public:
    StyleSheetCompose();
    explicit StyleSheetCompose(const std::vector<std::shared_ptr<StyleSheetBase>>& sources);

    // 移动构造函数
    StyleSheetCompose(std::vector<std::shared_ptr<StyleSheetBase>>&& sources);

    QString content(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;
    void add(const std::shared_ptr<StyleSheetBase>& source);
    void remove(const std::shared_ptr<StyleSheetBase>& source);

    // 预留空间以减少重新分配
    void reserve(size_t capacity);
    size_t size() const { return m_sources.size(); }

    std::shared_ptr<StyleSheetBase> clone() const override;
};

class QFLUENT_EXPORT CustomStyleSheetWatcher : public QObject {
    Q_OBJECT

private:
    QWidget* m_watchedWidget;
    bool m_isDirty;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

public:
    explicit CustomStyleSheetWatcher(QWidget* parent = nullptr);

    void markDirty() { m_isDirty = true; }
    bool isDirty() const { return m_isDirty; }
    void clearDirty() { m_isDirty = false; }

    void applyStyleSheetIfNeeded();
};

class QFLUENT_EXPORT StyleSheetManager : public QObject {
    Q_OBJECT

private:
    QHash<QWidget*, std::shared_ptr<StyleSheetCompose>> m_widgets;

public:
    StyleSheetManager();
    ~StyleSheetManager() override;

    // 禁止拷贝和移动
    StyleSheetManager(const StyleSheetManager&) = delete;
    StyleSheetManager& operator=(const StyleSheetManager&) = delete;

    static StyleSheetManager* instance();

    // 控件注册管理（widget 在第一个参数）
    void registerWidget(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source, bool reset = true);
    void registerWidget(QWidget* widget, Fluent::ThemeStyle type, bool reset = true);
    void deregisterWidget(QWidget* widget);

    // 查询方法
    std::shared_ptr<StyleSheetCompose> source(QWidget* widget) const;
    QList<QWidget*> widgets() const;
    bool isRegistered(QWidget* widget) const;

    // 样式表更新
    void updateStyleSheet(bool lazy = false);
};
