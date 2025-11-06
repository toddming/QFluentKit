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
#include <QPointer>
#include <QCache>
#include <memory>

#include "Define.h"

// 前向声明
class FluentIconBase;
class ColoredFluentIcon;
class Icon;

class FluentIconUtils {
public:
    static QString getIconColor(ThemeType::ThemeMode theme = ThemeType::AUTO, bool reverse = false);
    static void drawSvgIcon(const QByteArray& icon, QPainter* painter, const QRectF& rect);
    static void drawSvgIcon(const QString& iconPath, QPainter* painter, const QRectF& rect);
    static QString writeSvg(const QString& iconPath, const QList<int>& indexes = {}, const QMap<QString, QString>& attributes = {});
    static QIcon toQIcon(const QVariant& icon);
    static void drawIcon(const FluentIconBase& icon, QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::AUTO,
                         QIcon::State state = QIcon::Off, const QMap<QString, QString>& attributes = {});

private:
    // 缓存着色后的 SVG 字节数组
    static QCache<QString, QByteArray> s_svgCache;
};

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

class SvgIconEngine : public QIconEngine {
public:
    explicit SvgIconEngine(const QByteArray& svgData);
    ~SvgIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QByteArray m_svgData;
};

class FontIconEngine : public QIconEngine {
public:
    FontIconEngine(const QString& fontFamily, const QChar& ch, const QColor& color, bool isBold);
    ~FontIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QString m_fontFamily;
    QChar m_char;
    QColor m_color;
    bool m_isBold;
};

class QFLUENT_EXPORT FluentIconBase {
public:
    virtual ~FluentIconBase() = default;

    virtual QString path(ThemeType::ThemeMode theme = ThemeType::AUTO) const = 0;
    virtual QIcon icon(ThemeType::ThemeMode theme = ThemeType::AUTO, const QColor& color = QColor()) const;
    virtual ColoredFluentIcon colored(const QColor& lightColor, const QColor& darkColor) const;
    virtual void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::AUTO,
                                const QList<int>& indexes = {}, const QMap<QString, QString>& attributes = {}) const;
    virtual FluentIconBase* clone() const = 0;

    // 移除无效的 qicon(bool)
};

class QFLUENT_EXPORT FluentFontIconBase : public FluentIconBase {
public:
    explicit FluentFontIconBase(const QChar& ch = QChar());
    ~FluentFontIconBase() override = default;

    static FluentFontIconBase fromName(const QString& name);
    FluentFontIconBase& bold();

    QString path(ThemeType::ThemeMode theme = ThemeType::AUTO) const override;
    QIcon icon(ThemeType::ThemeMode theme = ThemeType::AUTO, const QColor& color = QColor()) const override;
    ColoredFluentIcon colored(const QColor& lightColor, const QColor& darkColor) const override;
    void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::AUTO,
                const QList<int>& indexes = {}, const QMap<QString, QString>& attributes = {}) const override;

    virtual QString fontPath() const { return QString(); }
    virtual QString iconNameMapPath() const { return QString(); }
    void loadFont();
    void loadIconNames();

    FluentIconBase* clone() const override {
        auto* copy = new FluentFontIconBase(m_char);
        copy->m_lightColor = m_lightColor;
        copy->m_darkColor = m_darkColor;
        copy->m_isBold = m_isBold;
        return copy;
    }

protected:
    QChar m_char;
    QColor m_lightColor;
    QColor m_darkColor;
    bool m_isBold = false;

    static bool s_isFontLoaded;
    static int s_fontId;
    static QString s_fontFamily;
    static QMap<QString, QChar> s_iconNames;

    QColor getIconColor(ThemeType::ThemeMode theme) const;
};

class QFLUENT_EXPORT ColoredFluentIcon : public FluentIconBase {
public:
    ColoredFluentIcon(const FluentIconBase& icon, const QColor& lightColor, const QColor& darkColor);
    ~ColoredFluentIcon() override = default;

    QString path(ThemeType::ThemeMode theme = ThemeType::AUTO) const override;
    void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::AUTO,
                const QList<int>& indexes = {}, const QMap<QString, QString>& attributes = {}) const override;
    FluentIconBase* clone() const override {
        return new ColoredFluentIcon(*m_fluentIcon, m_lightColor, m_darkColor);
    }

private:
    std::unique_ptr<FluentIconBase> m_fluentIcon;
    QColor m_lightColor;
    QColor m_darkColor;
};

class QFLUENT_EXPORT FluentIcon : public FluentIconBase {
public:
    explicit FluentIcon(FluentIconType::IconType iconEnum);
    explicit FluentIcon(const QString& templatePath);
    ~FluentIcon() override = default;

    QString path(ThemeType::ThemeMode theme = ThemeType::AUTO) const override;
    FluentIconType::IconType value() const { return m_iconEnum; }
    static const QMap<FluentIconType::IconType, QString>& fluentIcons();
    QIcon qicon(bool reverse = false) const;

    FluentIconBase* clone() const override {
        return (m_iconEnum == FluentIconType::CUSTOM_PATH)
            ? new FluentIcon(m_templatePath)
            : new FluentIcon(m_iconEnum);
    }

private:
    QString m_templatePath;
    FluentIconType::IconType m_iconEnum;
    mutable QString m_cachedLightPath;
    mutable QString m_cachedDarkPath;

    QString enumToString(FluentIconType::IconType e) const;
};

class QFLUENT_EXPORT Icon : public QIcon {
public:
    explicit Icon(const FluentIcon& fluentIcon);
    const FluentIcon& fluentIcon() const { return m_fluentIcon; }

private:
    FluentIcon m_fluentIcon;
};

QIcon toQIcon(const QVariant& icon);

class QFLUENT_EXPORT Action : public QAction {
    Q_OBJECT
public:
    explicit Action(QObject* parent = nullptr);
    explicit Action(const QString& text, QObject* parent = nullptr);
    explicit Action(const QIcon& icon, const QString& text, QObject* parent = nullptr);
    explicit Action(const FluentIconBase& icon, const QString& text, QObject* parent = nullptr);
    ~Action() override = default;

    QIcon icon() const;
    void setIcon(const QIcon& icon);
    void setFluentIcon(const FluentIconBase& icon);

private:
    std::unique_ptr<const FluentIconBase> m_fluentIcon;
};

#endif // FLUENTICON_H
