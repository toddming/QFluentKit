#pragma once
#include <QIcon>
#include <QIconEngine>
#include <QColor>
#include <QString>
#include <QRect>
#include <QRectF>
#include <QAction>
#include <QObject>
#include <QMap>
#include <QCache>

#include "FluentGlobal.h"

class QPainter;
class FluentIconBase;

/**
 * @brief Utility class for Fluent icon operations
 */
class QFLUENT_EXPORT FluentIconUtils
{
public:
    static QString iconColor(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                             bool reverse = false);

    static void drawSvgIcon(const QByteArray& iconData,
                            QPainter* painter,
                            const QRectF& rect);

    static void drawSvgIcon(const QString& iconPath,
                            QPainter* painter,
                            const QRectF& rect);

    static QString writeSvg(const QString& iconPath,
                            const QList<int>& indexes = QList<int>(),
                            const QHash<QString, QString>& attributes = QHash<QString, QString>());

    static QIcon toQIcon(const QVariant& icon);

    static void drawIcon(const FluentIconBase& icon,
                         QPainter* painter,
                         const QRectF& rect,
                         Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                         bool reverse = false,
                         const QHash<QString, QString>& attributes = QHash<QString, QString>());

    /**
     * @brief Draw theme-aware icon by IconType enum
     */
    static void drawThemeIcon(Fluent::IconType iconType,
                              QPainter* painter,
                              const QRectF& rect,
                              Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                              bool reverse = false);

    /**
     * @brief Draw theme-aware icon by template path
     */
    static void drawTemplateIcon(const QString& templatePath,
                                 QPainter* painter,
                                 const QRectF& rect,
                                 Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                                 bool reverse = false);

    static const QMap<Fluent::IconType, QString>& fluentIconsMap();

    static QString enumToString(Fluent::IconType iconType);

    static QString buildIconPath(Fluent::IconType iconEnum,
                                 const QString& templatePath,
                                 Fluent::ThemeMode theme,
                                 bool reverse,
                                 QString& cachedLightPath,
                                 QString& cachedDarkPath);

private:
    static QCache<QString, QByteArray> s_svgCache;
};

/**
 * @brief Icon engine for FluentIcon rendering with automatic theme adaptation
 */
class FluentIconEngine : public QIconEngine
{
public:
    explicit FluentIconEngine(Fluent::IconType iconType, bool reverse = false);
    explicit FluentIconEngine(const QString& templatePath, bool reverse = false);
    explicit FluentIconEngine(Fluent::IconType iconType, const QColor& lightColor,
                              const QColor& darkColor, bool reverse = false);
    explicit FluentIconEngine(const QString& templatePath, const QColor& lightColor,
                              const QColor& darkColor, bool reverse = false);
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
    bool m_isColored;
    QColor m_lightColor;
    QColor m_darkColor;
    mutable QString m_cachedLightPath;
    mutable QString m_cachedDarkPath;
};

/**
 * @brief Icon engine for SVG rendering
 */
class SvgIconEngine : public QIconEngine
{
public:
    explicit SvgIconEngine(const QByteArray& svgData);
    SvgIconEngine(SvgIconEngine&& other) noexcept;
    SvgIconEngine& operator=(SvgIconEngine&& other) noexcept;
    ~SvgIconEngine() override;

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
    FontIconEngine(FontIconEngine&& other) noexcept;
    FontIconEngine& operator=(FontIconEngine&& other) noexcept;
    ~FontIconEngine() override;

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
 * @brief Base class for Fluent icons (internal use only)
 */
class FluentIconBase
{
public:
    virtual ~FluentIconBase();

    virtual QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                         bool reverse = false) const = 0;

    virtual void render(QPainter* painter, const QRectF& rect,
                        Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                        bool reverse = false,
                        const QList<int>& indexes = QList<int>(),
                        const QHash<QString, QString>& attributes = QHash<QString, QString>()) const;
};

/**
 * @brief Colored variant of FluentIcon (internal use only)
 */
class ColoredFluentIcon : public FluentIconBase
{
public:
    explicit ColoredFluentIcon(Fluent::IconType iconEnum,
                               const QColor& lightColor,
                               const QColor& darkColor);
    explicit ColoredFluentIcon(const QString& templatePath,
                               const QColor& lightColor,
                               const QColor& darkColor);
    ~ColoredFluentIcon() override;

    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                 bool reverse = false) const override;
    void render(QPainter* painter, const QRectF& rect,
                Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                bool reverse = false,
                const QList<int>& indexes = QList<int>(),
                const QHash<QString, QString>& attributes = QHash<QString, QString>()) const override;

private:
    Fluent::IconType m_iconEnum;
    QString m_templatePath;
    QColor m_lightColor;
    QColor m_darkColor;
    mutable QString m_cachedLightPath;
    mutable QString m_cachedDarkPath;
};

/**
 * @brief Fluent icon (internal use only)
 */
class FluentIcon : public FluentIconBase
{
public:
    explicit FluentIcon(Fluent::IconType iconEnum);
    explicit FluentIcon(const QString& templatePath);
    ~FluentIcon() override;

    QString path(Fluent::ThemeMode theme = Fluent::ThemeMode::AUTO,
                 bool reverse = false) const override;
    Fluent::IconType value() const { return m_iconEnum; }

private:
    QString m_templatePath;
    Fluent::IconType m_iconEnum;
    mutable QString m_cachedLightPath;
    mutable QString m_cachedDarkPath;
};

/**
 * @brief FluentQIcon — QIcon wrapper that carries FluentIcon metadata
 *
 * Inherits QIcon so it can be used anywhere QIcon is expected.
 * Carries IconType/templatePath + reverse + color metadata so it can
 * generate reversed() and colored() variants on demand.
 */
class QFLUENT_EXPORT FluentQIcon : public QIcon
{
public:
    FluentQIcon() = default;
    explicit FluentQIcon(Fluent::IconType type, bool reverse = false);
    explicit FluentQIcon(const QString& templatePath, bool reverse = false);
    explicit FluentQIcon(const QIcon& icon);
    FluentQIcon(Fluent::IconType type, const QColor& lightColor,
                const QColor& darkColor, bool reverse = false);
    FluentQIcon(const QString& templatePath, const QColor& lightColor,
                const QColor& darkColor, bool reverse = false);

    FluentQIcon reversed() const;
    FluentQIcon colored(const QColor& lightColor, const QColor& darkColor) const;

    bool hasType() const { return m_hasType; }
    bool isReversed() const { return m_reverse; }
    bool isColored() const { return m_isColored; }
    Fluent::IconType iconType() const { return m_type; }
    QString templatePath() const { return m_templatePath; }
    QColor lightColor() const { return m_lightColor; }
    QColor darkColor() const { return m_darkColor; }

private:
    Fluent::IconType m_type = {};
    QString m_templatePath;
    bool m_reverse = false;
    bool m_hasType = false;
    bool m_isColored = false;
    QColor m_lightColor;
    QColor m_darkColor;
};

/**
 * @brief Convert QVariant to QIcon (global function)
 */
QFLUENT_EXPORT QIcon toQIcon(const QVariant& icon);

/**
 * @brief QAction (no longer holds FluentIconBase, uses QIcon directly)
 */
class QFLUENT_EXPORT Action : public QAction
{
    Q_OBJECT
public:
    explicit Action(QObject* parent = nullptr);
    explicit Action(const QString& text, QObject* parent = nullptr);
    explicit Action(const QIcon& icon, const QString& text, QObject* parent = nullptr);
    ~Action() override;
};

// ====================== Fluent Icon Factory Functions ======================

namespace Fluent {

QFLUENT_EXPORT FluentQIcon icon(IconType type, bool reverse = false);
QFLUENT_EXPORT FluentQIcon icon(const QString& templatePath, bool reverse = false);
QFLUENT_EXPORT FluentQIcon coloredIcon(IconType type, const QColor& lightColor, const QColor& darkColor);
QFLUENT_EXPORT FluentQIcon coloredIcon(const QString& templatePath, const QColor& lightColor, const QColor& darkColor);

} // namespace Fluent