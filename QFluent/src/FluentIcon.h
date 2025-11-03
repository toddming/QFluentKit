#ifndef FLUENTICON_H
#define FLUENTICON_H

#include <QIcon>
#include <QIconEngine>
#include <QColor>
#include <QString>
#include <QRect>
#include <QRectF>
#include <QPainter>
#include <QAction>
#include <QObject>
#include <QMap>

#include "Define.h"

// 前向声明
class FluentIcon;
class FluentIconBase;
class ColoredFluentIcon;
class Icon;


class FluentIconUtils {
public:
    // 获取图标颜色
    static QString getIconColor(ThemeType::ThemeMode theme = ThemeType::AUTO, bool reverse = false);

    // 绘制SVG图标（重载）
    static void drawSvgIcon(const QByteArray& icon, QPainter* painter, const QRectF& rect);
    static void drawSvgIcon(const QString& iconPath, QPainter* painter, const QRectF& rect);

    // 写入SVG
    static QString writeSvg(const QString& iconPath, const QList<int>& indexes = QList<int>(), const QMap<QString, QString>& attributes = QMap<QString, QString>());

    // toQIcon 辅助函数
    static QIcon toQIcon(const QVariant& icon);

    static void drawIcon(const FluentIconBase& icon, QPainter* painter, const QRectF& rect, QIcon::State state = QIcon::Off, const QMap<QString, QString>& attributes = QMap<QString, QString>());

};

// 绘制图标

// FluentIconEngine 类
class FluentIconEngine : public QIconEngine {
public:
    FluentIconEngine(FluentIconType::IconType iconType, bool reverse = false);
    FluentIconEngine(const QString& templatePath, bool reverse = false);
    ~FluentIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    FluentIconType::IconType m_iconType;
    QString m_templatePath;
    bool m_isThemeReversed;
    std::unique_ptr<FluentIconBase> m_iconBase;
};

// SvgIconEngine 类
class SvgIconEngine : public QIconEngine {
public:
    explicit SvgIconEngine(const QString& svg);
    ~SvgIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QString m_svg;
};

// FontIconEngine 类
class FontIconEngine : public QIconEngine {
public:
    FontIconEngine(const QString& fontFamily, const QString& ch, const QColor& color, bool isBold);
    ~FontIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QString m_fontFamily;
    QString m_char;
    QColor m_color;
    bool m_isBold;
};

// FluentIconBase 基类
class QFLUENT_EXPORT FluentIconBase {
public:
    virtual ~FluentIconBase() = default;

    virtual QString path(ThemeType::ThemeMode theme = ThemeType::AUTO) const = 0;
    virtual QIcon icon(ThemeType::ThemeMode theme = ThemeType::AUTO, const QColor& color = QColor()) const;
    virtual ColoredFluentIcon colored(const QColor& lightColor, const QColor& darkColor) const;
    virtual QIcon qicon(bool reverse = false) const;
    virtual void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::AUTO,
                        const QList<int>& indexes = QList<int>(),
                        const QMap<QString, QString>& attributes = QMap<QString, QString>()) const;
    virtual FluentIconBase* clone() const = 0;
};

// FluentFontIconBase 类
class QFLUENT_EXPORT FluentFontIconBase : public FluentIconBase {
public:
    explicit FluentFontIconBase(const QString& ch = QString());
    ~FluentFontIconBase() override = default;

    static FluentFontIconBase fromName(const QString& name);

    FluentFontIconBase& bold();

    // 实现纯虚函数
    QString path(ThemeType::ThemeMode theme = ThemeType::AUTO) const override;

    QIcon icon(ThemeType::ThemeMode theme = ThemeType::AUTO, const QColor& color = QColor()) const override;
    ColoredFluentIcon colored(const QColor& lightColor, const QColor& darkColor) const override;
    void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::AUTO,
                const QList<int>& indexes = QList<int>(),
                const QMap<QString, QString>& attributes = QMap<QString, QString>()) const override;

    virtual QString fontPath() const { return QString(); }
    virtual QString iconNameMapPath() const { return QString(); }
    void loadFont();
    void loadIconNames();

    FluentIconBase* FluentFontIconBase::clone() const {
        FluentFontIconBase* copy = new FluentFontIconBase(m_char);
        copy->m_lightColor = m_lightColor;
        copy->m_darkColor = m_darkColor;
        copy->m_isBold = m_isBold;
        return copy;
    }

protected:
    QString m_char;
    QColor m_lightColor;
    QColor m_darkColor;
    bool m_isBold;

    static bool s_isFontLoaded;
    static int s_fontId;
    static QString s_fontFamily;
    static QMap<QString, QString> s_iconNames;

    QColor getIconColor(ThemeType::ThemeMode theme) const;
};

// ColoredFluentIcon 类
class QFLUENT_EXPORT ColoredFluentIcon : public FluentIconBase {
public:
    ColoredFluentIcon(const FluentIconBase& icon, const QColor& lightColor, const QColor& darkColor);
    ~ColoredFluentIcon() override = default;

    QString path(ThemeType::ThemeMode theme = ThemeType::AUTO) const override;
    void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::AUTO,
                const QList<int>& indexes = QList<int>(),
                const QMap<QString, QString>& attributes = QMap<QString, QString>()) const override;
    FluentIconBase* ColoredFluentIcon::clone() const {
        return new ColoredFluentIcon(*m_fluentIcon->clone(), m_lightColor, m_darkColor);
    }

private:
    const FluentIconBase* m_fluentIcon;
    QColor m_lightColor;
    QColor m_darkColor;    
};

// FluentIcon 类
class QFLUENT_EXPORT FluentIcon : public FluentIconBase {
public:
    explicit FluentIcon(FluentIconType::IconType iconEnum);
    explicit FluentIcon(const QString& templatePath);

    ~FluentIcon() override = default;

    QString path(ThemeType::ThemeMode theme = ThemeType::AUTO) const override;
    FluentIconType::IconType value() const { return m_iconEnum; }
    static QMap<FluentIconType::IconType, QString> fluentIcons();
    QIcon qicon(bool reverse = false) const override;

    FluentIconBase* FluentIcon::clone() const {
        if (m_iconEnum == FluentIconType::CUSTOM_PATH) {
            return new FluentIcon(m_templatePath);  // 拷贝自定义路径版本
        } else {
            return new FluentIcon(m_iconEnum);  // 拷贝枚举版本
        }
    }

private:
    QString m_templatePath;
    FluentIconType::IconType m_iconEnum;
    QString enumToString(FluentIconType::IconType e) const;

};

// Icon 类
class QFLUENT_EXPORT Icon : public QIcon {
public:
    explicit Icon(const FluentIcon& fluentIcon);

    const FluentIcon& fluentIcon() const { return m_fluentIcon; }

private:
    FluentIcon m_fluentIcon;
};

// toQIcon 辅助函数
QIcon toQIcon(const QVariant& icon);

// Action 类
class QFLUENT_EXPORT Action : public QAction {
    Q_OBJECT

public:
    // 构造函数重载
    explicit Action(QObject* parent = nullptr);
    Action(const QString& text, QObject* parent = nullptr);
    Action(const QIcon& icon, const QString& text, QObject* parent = nullptr);
    Action(const FluentIconBase& icon, const QString& text, QObject* parent = nullptr);

    ~Action() override = default;

    QIcon icon() const;
    void setIcon(const QIcon& icon);
    void setFluentIcon(const FluentIconBase& icon);

private:
    const FluentIconBase* m_fluentIcon;
};

#endif // FLUENTICON_H
