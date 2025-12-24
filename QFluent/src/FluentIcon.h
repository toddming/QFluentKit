#pragma once
#include <QIcon>
#include <QIconEngine>
#include <QColor>
#include <QString>
#include <QRect>
#include <QRectF>
#include <QPainter>
#include <QAction>
#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include <QCache>

#include "FluentGlobal.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtCore/qhashfunctions.h>
inline uint qHash(Fluent::IconType key, uint seed = 0) noexcept
{
    return ::qHash(static_cast<std::underlying_type_t<Fluent::IconType>>(key), seed);
}
#endif

// Forward declarations
class FluentIconBase;
class ColoredFluentIcon;
class Icon;

/**
 * @brief Utility class for Fluent icon operations
 */
class QFLUENT_EXPORT FluentIconUtils
{
public:
    /**
     * @brief Get icon color based on theme mode
     * @param theme Theme mode (AUTO, LIGHT, DARK)
     * @param reverse Whether to reverse the color scheme
     * @return Color string ("black" or "white")
     */
    static QString iconColor(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                            bool reverse = false);

    /**
     * @brief Draw SVG icon from byte array
     */
    static void drawSvgIcon(const QByteArray& iconData,
                           QPainter* painter,
                           const QRectF& rect);

    /**
     * @brief Draw SVG icon from file path
     */
    static void drawSvgIcon(const QString& iconPath,
                           QPainter* painter,
                           const QRectF& rect);

    /**
     * @brief Generate modified SVG with custom attributes
     */
    static QString writeSvg(const QString& iconPath,
                          const QList<int>& indexes = QList<int>(),
                          const QHash<QString, QString>& attributes = QHash<QString, QString>());

    /**
     * @brief Convert QVariant to QIcon
     */
    static QIcon toQIcon(const QVariant& icon);

    /**
     * @brief Draw icon using FluentIconBase
     */
    static void drawIcon(const FluentIconBase& icon,
                        QPainter* painter,
                        const QRectF& rect,
                        Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                        QIcon::State state = QIcon::Off,
                        const QHash<QString, QString>& attributes = QHash<QString, QString>());

    /**
     * @brief Get map of all fluent icons
     */
    static const QHash<Fluent::IconType, QString>& fluentIconsMap();

    /**
     * @brief Convert icon enum to string
     */
    static QString enumToString(Fluent::IconType iconType);

private:
    // Thread-safe cache for colored SVG data
    static QCache<QString, QByteArray> s_svgCache;
};

/**
 * @brief Icon engine for FluentIcon rendering
 */
class FluentIconEngine : public QIconEngine
{
public:
    explicit FluentIconEngine(Fluent::IconType iconType, bool reverse = false);
    explicit FluentIconEngine(const QString& templatePath, bool reverse = false);
    ~FluentIconEngine() override;

    void paint(QPainter* painter, const QRect& rect,
              QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode,
                  QIcon::State state) override;

private:
    Fluent::IconType m_iconType;
    QString m_templatePath;
    bool m_isThemeReversed;
    QSharedPointer<FluentIconBase> m_iconBase;
};

/**
 * @brief Icon engine for SVG rendering
 */
class SvgIconEngine : public QIconEngine
{
public:
    explicit SvgIconEngine(const QByteArray& svgData);
    ~SvgIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect,
              QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode,
                  QIcon::State state) override;

private:
    QByteArray m_svgData;
};

/**
 * @brief Icon engine for font-based icon rendering
 */
class FontIconEngine : public QIconEngine
{
public:
    FontIconEngine(const QString& fontFamily, QChar character,
                  const QColor& color, bool isBold);
    ~FontIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect,
              QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode,
                  QIcon::State state) override;

private:
    QString m_fontFamily;
    QChar m_character;
    QColor m_color;
    bool m_isBold;
};

/**
 * @brief Base class for Fluent icons
 */
class QFLUENT_EXPORT FluentIconBase
{
public:
    virtual ~FluentIconBase() = default;

    /**
     * @brief Get icon file path based on theme
     */
    virtual QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const = 0;

    /**
     * @brief Get QIcon representation
     */
    virtual QIcon icon(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                      const QColor& color = QColor()) const;

    /**
     * @brief Create colored version of icon
     * @note Default implementation throws warning. Override in subclasses that support coloring.
     */
    virtual ColoredFluentIcon colored(const QColor& lightColor,
                                     const QColor& darkColor) const;

    /**
     * @brief Render icon to painter
     */
    virtual void render(QPainter* painter, const QRectF& rect,
                       Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                       const QList<int>& indexes = QList<int>(),
                       const QHash<QString, QString>& attributes = QHash<QString, QString>()) const;

    /**
     * @brief Clone this icon
     */
    virtual FluentIconBase* clone() const = 0;
};

/**
 * @brief Base class for font-based Fluent icons
 */
class QFLUENT_EXPORT FluentFontIconBase : public FluentIconBase
{
public:
    explicit FluentFontIconBase(QChar character = QChar());
    ~FluentFontIconBase() override = default;

    static FluentFontIconBase fromName(const QString& name);
    FluentFontIconBase& bold();

    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;
    QIcon icon(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
              const QColor& color = QColor()) const override;
    void render(QPainter* painter, const QRectF& rect,
               Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
               const QList<int>& indexes = QList<int>(),
               const QHash<QString, QString>& attributes = QHash<QString, QString>()) const override;

    virtual QString fontPath() const { return QString(); }
    virtual QString iconNameMapPath() const { return QString(); }

    FluentIconBase* clone() const override;

protected:
    void loadFont();
    void loadIconNames();
    QColor iconColor(Fluent::ThemeMode theme) const;

    QChar m_character;
    QColor m_lightColor;
    QColor m_darkColor;
    bool m_isBold;

    static bool s_isFontLoaded;
    static int s_fontId;
    static QString s_fontFamily;
    static QHash<QString, QChar> s_iconNames;
};

/**
 * @brief Colored variant of FluentIcon
 */
class QFLUENT_EXPORT ColoredFluentIcon : public FluentIconBase
{
public:
    explicit ColoredFluentIcon(Fluent::IconType iconEnum,
                              const QColor& lightColor,
                              const QColor& darkColor);
    explicit ColoredFluentIcon(const QString& templatePath,
                              const QColor& lightColor,
                              const QColor& darkColor);
    ~ColoredFluentIcon() override = default;

    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;
    void render(QPainter* painter, const QRectF& rect,
               Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
               const QList<int>& indexes = QList<int>(),
               const QHash<QString, QString>& attributes = QHash<QString, QString>()) const override;
    ColoredFluentIcon colored(const QColor& lightColor,
                             const QColor& darkColor) const override;

    Fluent::IconType value() const { return m_iconEnum; }
    FluentIconBase* clone() const override;

private:
    Fluent::IconType m_iconEnum;
    QString m_templatePath;
    QColor m_lightColor;
    QColor m_darkColor;
    mutable QString m_cachedLightPath;
    mutable QString m_cachedDarkPath;
};

/**
 * @brief Main Fluent icon class
 */
class QFLUENT_EXPORT FluentIcon : public FluentIconBase
{
public:
    explicit FluentIcon(Fluent::IconType iconEnum);
    explicit FluentIcon(const QString& templatePath);
    ~FluentIcon() override = default;

    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO) const override;
    Fluent::IconType value() const { return m_iconEnum; }
    QIcon qicon(bool reverse = false) const;
    ColoredFluentIcon colored(const QColor& lightColor,
                             const QColor& darkColor) const override;

    FluentIconBase* clone() const override;

private:
    QString m_templatePath;
    Fluent::IconType m_iconEnum;
    mutable QString m_cachedLightPath;
    mutable QString m_cachedDarkPath;
};

/**
 * @brief QIcon wrapper for FluentIcon
 */
class QFLUENT_EXPORT Icon : public QIcon
{
public:
    explicit Icon(const FluentIcon& fluentIcon);
    const FluentIcon& fluentIcon() const { return m_fluentIcon; }

private:
    FluentIcon m_fluentIcon;
};

/**
 * @brief Convert QVariant to QIcon (global function)
 */
QFLUENT_EXPORT QIcon toQIcon(const QVariant& icon);

/**
 * @brief QAction with FluentIcon support
 */
class QFLUENT_EXPORT Action : public QAction
{
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
    QSharedPointer<FluentIconBase> m_fluentIcon;
};
