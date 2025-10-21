#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QString>
#include <QMap>
#include <QEvent>
#include <QFile>
#include <memory>
#include <vector>

#include "Define.h"

// 前向声明
class StyleSheetBase;
class StyleSheetManager;

class QFLUENT_EXPORT StyleSheetHelper {
public:
    // 样式表内容处理
    static QString applyThemeColor(const QString& qss);
    static QString getStyleSheetFromFile(const QString& filePath);

    // 样式表应用
    static QString getStyleSheet(const std::shared_ptr<StyleSheetBase>& source, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);
    static QString getStyleSheet(const QString& source, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);

    static void setStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                             ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool registerWidget = true);
    static void setStyleSheet(QWidget* widget, const QString& source,
                             ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool registerWidget = true);

    static void setCustomStyleSheet(QWidget* widget, const QString& lightQss, const QString& darkQss);

    static void addStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                             ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool registerWidget = true);
    static void addStyleSheet(QWidget* widget, const QString& source,
                             ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool registerWidget = true);
};

class QFLUENT_EXPORT StyleSheetBase {
public:
    virtual ~StyleSheetBase() = default;
    virtual QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);
    virtual QString content(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);
    virtual void apply(QWidget* widget, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO);
};

class QFLUENT_EXPORT StyleSheetFile : public StyleSheetBase {
private:
    QString m_lightPath;
    QString m_darkPath;
    QString m_filePath;

public:
    explicit StyleSheetFile(const QString& path);
    StyleSheetFile(const QString& lightPath, const QString& darkPath);

    QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
    bool isMultiPath() const;
};

class QFLUENT_EXPORT TemplateStyleSheetFile : public StyleSheetBase {
private:
    QString m_templatePath;

public:
    explicit TemplateStyleSheetFile(const QString& templatePath);
    QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
};


class QFLUENT_EXPORT FluentStyleSheet : public StyleSheetBase {
private:
    ThemeType::ThemeStyle m_type;

public:
    explicit FluentStyleSheet(ThemeType::ThemeStyle type);
    QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
    static QString typeToString(ThemeType::ThemeStyle type);
};

class QFLUENT_EXPORT CustomStyleSheet : public StyleSheetBase {
private:
    QWidget* m_widget;

public:
    explicit CustomStyleSheet(QWidget* widget);
    QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
    QString content(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;

    CustomStyleSheet* setCustomStyleSheet(const QString& lightQss, const QString& darkQss);
    CustomStyleSheet* setLightStyleSheet(const QString& qss);
    CustomStyleSheet* setDarkStyleSheet(const QString& qss);

    QString lightStyleSheet() const;
    QString darkStyleSheet() const;

    static const QString DARK_QSS_KEY;
    static const QString LIGHT_QSS_KEY;

};

class QFLUENT_EXPORT StyleSheetCompose : public StyleSheetBase {
private:
    std::vector<std::shared_ptr<StyleSheetBase>> m_sources;

public:
    StyleSheetCompose();
    explicit StyleSheetCompose(const std::vector<std::shared_ptr<StyleSheetBase>>& sources);

    QString content(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) override;
    void add(const std::shared_ptr<StyleSheetBase>& source);
    void remove(const std::shared_ptr<StyleSheetBase>& source);
};

class QFLUENT_EXPORT CustomStyleSheetWatcher : public QObject {
private:
    QWidget* m_watchedWidget;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

public:
    explicit CustomStyleSheetWatcher(QWidget* parent = nullptr);
};

class QFLUENT_EXPORT StyleSheetManager : public QObject {
private:
    QMap<QWidget*, std::shared_ptr<StyleSheetCompose>> m_widgets;
    static StyleSheetManager* m_instance;

    StyleSheetManager();

public:
    static StyleSheetManager* instance();

    // 控件注册管理
    void registerWidget(const std::shared_ptr<StyleSheetBase>& source, QWidget* widget, bool reset = true);
    void deregisterWidget(QWidget* widget);

    // 使用预定义Fluent样式注册
    void registerWidget(QWidget* widget, ThemeType::ThemeStyle type, bool reset = true);

    // 查询方法
    std::shared_ptr<StyleSheetCompose> source(QWidget* widget) const;
    QList<QWidget*> widgets() const;

    // 样式表更新
    void updateStyleSheet(bool lazy = false);

    // 便捷方法
    static void setStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                             ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool registerWidget = true);
    static void setStyleSheet(QWidget* widget, const QString& source,
                             ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool registerWidget = true);

    static void setCustomStyleSheet(QWidget* widget, const QString& lightQss, const QString& darkQss);

    static void addStyleSheet(QWidget* widget, const std::shared_ptr<StyleSheetBase>& source,
                             ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool registerWidget = true);
    static void addStyleSheet(QWidget* widget, const QString& source,
                             ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool registerWidget = true);
};


#endif // STYLESHEET_H
