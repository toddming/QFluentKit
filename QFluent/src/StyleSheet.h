#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QObject>
#include <QMap>
#include <QHash>
#include <memory>
#include <vector>
#include <QMutex>

#include "FluentGlobal.h"

// 前向声明
class QWidget;
class StyleSheetBase;
class StyleSheetManager;

class QFLUENT_EXPORT StyleSheetHelper {
public:
    // 样式表内容处理
    static QString applyThemeColor(const QString& qss);
    static QString getStyleSheetFromFile(const QString& filePath);

    // 样式表应用 - 返回const引用避免不必要的拷贝
    static QString getStyleSheet(const std::shared_ptr<StyleSheetBase>& source,
                                Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);
    static QString getStyleSheet(const QString& source,
                                Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);

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

private:
    // 缓存主题颜色映射以提高性能
    static QHash<QString, QString> getThemeColorMap();
};

class QFLUENT_EXPORT StyleSheetBase {
public:
    virtual ~StyleSheetBase() = default;
    virtual QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);
    virtual QString content(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);
    virtual void apply(QWidget* widget, Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO);
};

class QFLUENT_EXPORT StyleSheetFile : public StyleSheetBase {
private:
    QString m_lightPath;
    QString m_darkPath;
    QString m_filePath;
    bool m_isMultiPath;

public:
    explicit StyleSheetFile(const QString& path);
    StyleSheetFile(const QString& lightPath, const QString& darkPath);

    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
    inline bool isMultiPath() const { return m_isMultiPath; }
};

class QFLUENT_EXPORT TemplateStyleSheetFile : public StyleSheetBase {
private:
    QString m_templatePath;
    mutable QString m_cachedLightPath;
    mutable QString m_cachedDarkPath;

public:
    explicit TemplateStyleSheetFile(const QString& templatePath);
    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
};

class QFLUENT_EXPORT FluentStyleSheet : public StyleSheetBase {
private:
    Fluent::ThemeStyle m_type;

    // 静态缓存类型到字符串的映射
    static const QHash<Fluent::ThemeStyle, QString>& getTypeMap();

public:
    explicit FluentStyleSheet(Fluent::ThemeStyle type);
    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
    static QString typeToString(Fluent::ThemeStyle type);
};

class QFLUENT_EXPORT CustomStyleSheet : public StyleSheetBase {
private:
    QWidget* m_widget;

public:
    explicit CustomStyleSheet(QWidget* widget);
    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
    QString content(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;

    CustomStyleSheet* setCustomStyleSheet(const QString& lightQss, const QString& darkQss);
    CustomStyleSheet* setLightStyleSheet(const QString& qss);
    CustomStyleSheet* setDarkStyleSheet(const QString& qss);

    QString lightStyleSheet() const;
    QString darkStyleSheet() const;

    static const char* DARK_QSS_KEY;
    static const char* LIGHT_QSS_KEY;
};

class QFLUENT_EXPORT StyleSheetCompose : public StyleSheetBase {
private:
    std::vector<std::shared_ptr<StyleSheetBase>> m_sources;

public:
    StyleSheetCompose();
    explicit StyleSheetCompose(const std::vector<std::shared_ptr<StyleSheetBase>>& sources);

    // 移动构造函数
    StyleSheetCompose(std::vector<std::shared_ptr<StyleSheetBase>>&& sources);

    QString content(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) override;
    void add(const std::shared_ptr<StyleSheetBase>& source);
    void remove(const std::shared_ptr<StyleSheetBase>& source);

    // 预留空间以减少重新分配
    void reserve(size_t capacity);
    size_t size() const { return m_sources.size(); }
};

class QFLUENT_EXPORT CustomStyleSheetWatcher : public QObject {
    Q_OBJECT

private:
    QWidget* m_watchedWidget;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

public:
    explicit CustomStyleSheetWatcher(QWidget* parent = nullptr);
};

class QFLUENT_EXPORT StyleSheetManager : public QObject {
    Q_OBJECT

private:
    // 使用QHash代替QMap以获得更好的查找性能
    QHash<QWidget*, std::shared_ptr<StyleSheetCompose>> m_widgets;
    static StyleSheetManager* m_instance;
    static QMutex m_mutex; // 线程安全

    StyleSheetManager();

public:
    static StyleSheetManager* instance();

    // 控件注册管理
    void registerWidget(const std::shared_ptr<StyleSheetBase>& source,
                       QWidget* widget, bool reset = true);
    void deregisterWidget(QWidget* widget);

    // 使用预定义Fluent样式注册
    void registerWidget(QWidget* widget, Fluent::ThemeStyle type, bool reset = true);

    // 查询方法
    std::shared_ptr<StyleSheetCompose> source(QWidget* widget) const;
    QList<QWidget*> widgets() const;

    // 检查widget是否已注册
    bool isRegistered(QWidget* widget) const;

    // 样式表更新
    void updateStyleSheet(bool lazy = false);

    // 便捷方法
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
};

#endif // STYLESHEET_H
