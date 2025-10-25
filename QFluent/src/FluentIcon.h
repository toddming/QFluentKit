// FluentIcon.h
#ifndef FLUENTICON_H
#define FLUENTICON_H

#include <QIcon>
#include <QIconEngine>
#include <QColor>
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QMap>
#include <QFont>
#include <QDomDocument>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QCache>
#include <QPair>
#include <QMutex>  // 添加以支持线程安全

#include "../common/Theme.h"

// 前向声明
class FluentIconBase;
class ColoredFluentIcon;

// 获取图标颜色
QString getIconColor(Theme::ThemeType theme = Theme::ThemeType::AUTO, bool reverse = false);

// ============================================================================
// SvgCache - SVG缓存管理器（添加线程安全）
// ============================================================================
class SvgCache {
public:
    static SvgCache& instance();

    QString get(const QString& key) const;
    void insert(const QString& key, const QString& value, int cost = 1);
    void setMaxCost(int cost);
    void clear();

    // 生成缓存键
    static QString generateCacheKey(const QString& iconPath,
                                    const QList<int>& indexes,
                                    const QMap<QString, QString>& attributes);

private:
    SvgCache() = default;
    ~SvgCache() = default;

    QCache<QString, QString> m_cache{1024};  // 增加默认 maxCost 以提高缓存容量
    mutable QMutex m_mutex;  // 添加互斥锁以确保线程安全
};

// ============================================================================
// FluentIconEngine - Fluent图标引擎
// ============================================================================
class FluentIconEngine : public QIconEngine {
public:
    FluentIconEngine(QSharedPointer<FluentIconBase> icon, bool reverse = false);
    ~FluentIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QSharedPointer<FluentIconBase> m_icon;
    bool m_isThemeReversed;
};

// ============================================================================
// SvgIconEngine - SVG图标引擎
// ============================================================================
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

// ============================================================================
// FontIconEngine - 字体图标引擎
// ============================================================================
class FontIconEngine : public QIconEngine {
public:
    FontIconEngine(const QString& fontFamily, const QString& charCode,
                   const QColor& color, bool isBold);
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

// ============================================================================
// FluentIconBase - Fluent图标基类
// ============================================================================
class FluentIconBase {
public:
    using Ptr = QSharedPointer<FluentIconBase>;

    virtual ~FluentIconBase() = default;

    // 克隆方法
    virtual Ptr clone() const = 0;

    // 获取图标路径
    virtual QString path(Theme::ThemeType theme = Theme::ThemeType::AUTO) const = 0;

    // 创建QIcon
    virtual QIcon icon(Theme::ThemeType theme = Theme::ThemeType::AUTO, const QColor& color = QColor()) const;

    // 创建彩色图标（返回智能指针）
    virtual Ptr colored(const QColor& lightColor, const QColor& darkColor);

    // 创建QIcon(带主题反转)
    QIcon qicon(bool reverse = false) const;

    // 渲染图标
    virtual void render(QPainter* painter, const QRect& rect, Theme::ThemeType theme = Theme::ThemeType::AUTO,
                        const QList<int>& indexes = QList<int>(),
                        const QMap<QString, QString>& attributes = QMap<QString, QString>()) const;
};

// ============================================================================
// FluentFontIconBase - Fluent字体图标基类
// ============================================================================
class FluentFontIconBase : public FluentIconBase {
public:
    using Ptr = QSharedPointer<FluentFontIconBase>;

    explicit FluentFontIconBase(const QString& charCode);
    virtual ~FluentFontIconBase() = default;

    // 克隆方法(子类需要实现)
    FluentIconBase::Ptr clone() const override = 0;

    // 提供默认实现,子类可以重写
    QString path(Theme::ThemeType theme = Theme::ThemeType::AUTO) const override;

    // 设置粗体 - 返回派生类智能指针
    Ptr bold();

    // 创建QIcon
    QIcon icon(Theme::ThemeType theme = Theme::ThemeType::AUTO, const QColor& color = QColor()) const override;

    // 设置颜色 - 返回派生类智能指针（不重写基类虚函数）
    Ptr withColor(const QColor& lightColor, const QColor& darkColor);

    // 渲染
    void render(QPainter* painter, const QRect& rect, Theme::ThemeType theme = Theme::ThemeType::AUTO,
                const QList<int>& indexes = QList<int>(),
                const QMap<QString, QString>& attributes = QMap<QString, QString>()) const override;

    // 获取图标名称映射路径
    virtual QString iconNameMapPath() const { return QString(); }

    // 加载字体
    bool loadFont();

    // 清理字体资源
    static void cleanup();

protected:
    QString m_char;
    QColor m_lightColor;
    QColor m_darkColor;
    bool m_isBold;

    static bool s_isFontLoaded;
    static int s_fontId;
    static QString s_fontFamily;

private:
    QColor getIconColor(Theme::ThemeType theme) const;
};

// ============================================================================
// ColoredFluentIcon - 彩色Fluent图标
// ============================================================================
class ColoredFluentIcon : public FluentIconBase {
public:
    ColoredFluentIcon(FluentIconBase::Ptr icon, const QColor& lightColor, const QColor& darkColor);
    ~ColoredFluentIcon() override = default;

    // 克隆方法
    Ptr clone() const override;

    QString path(Theme::ThemeType theme = Theme::ThemeType::AUTO) const override;

    void render(QPainter* painter, const QRect& rect, Theme::ThemeType theme = Theme::ThemeType::AUTO,
                const QList<int>& indexes = QList<int>(),
                const QMap<QString, QString>& attributes = QMap<QString, QString>()) const override;

private:
    FluentIconBase::Ptr m_fluentIcon;
    QColor m_lightColor;
    QColor m_darkColor;
};

// ============================================================================
// FluentIcon - Fluent图标枚举
// ============================================================================
class FluentIcon : public FluentIconBase {
public:
    enum IconType {
        UP,
        ADD,
        BUS,
        CAR,
        CUT,
        DELETE,
        EDIT,
        SEARCH
    };

    explicit FluentIcon(IconType type);
    ~FluentIcon() override = default;

    // 克隆方法
    Ptr clone() const override;

    QString path(Theme::ThemeType theme = Theme::ThemeType::AUTO) const override;
    IconType type() const { return m_type; }

private:
    IconType m_type;
    static QString iconName(IconType type);
};

// ============================================================================
// Icon - QIcon包装类
// ============================================================================
class Icon : public QIcon {
public:
    explicit Icon(const FluentIcon& fluentIcon);
    ~Icon() = default;

    const FluentIcon& fluentIcon() const { return m_fluentIcon; }

private:
    FluentIcon m_fluentIcon;
};

// ============================================================================
// 辅助函数
// ============================================================================

// 绘制SVG图标
bool drawSvgIcon(const QByteArray& svgData, QPainter* painter, const QRect& rect);

// 写入SVG(修改属性)，使用缓存
QString writeSvg(const QString& iconPath, const QList<int>& indexes = QList<int>(),
                 const QMap<QString, QString>& attributes = QMap<QString, QString>());

// 绘制图标
void drawIcon(const QVariant& icon, QPainter* painter, const QRect& rect,
              QIcon::State state = QIcon::Off,
              const QMap<QString, QString>& attributes = QMap<QString, QString>());

// 转换为QIcon
QIcon toQIcon(const QVariant& icon);

#endif // FLUENTICON_H
