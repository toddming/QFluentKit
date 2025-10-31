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
#include <QAction>
#include <QHash>

#include "Define.h"

// 前向声明
class FluentIconBase;
class ColoredFluentIcon;

// 获取图标颜色
QString getIconColor(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, bool reverse = false);

// ============================================================================
// SvgCache - SVG缓存管理器（优化版）
// ============================================================================
// 注意：此类设计为单线程使用（UI线程），不提供线程安全保证
// Qt的GUI类（QIcon、QPainter等）本身就要求在主线程使用
class SvgCache {
public:
    static SvgCache& instance();

    QByteArray get(const QString& key) const;
    void insert(const QString& key, const QByteArray& value, int cost = 1);
    void setMaxCost(int cost);
    void clear();

    // 生成缓存键
    static QString generateCacheKey(const QString& iconPath,
                                    const QList<int>& indexes,
                                    const QMap<QString, QString>& attributes);

private:
    SvgCache();
    ~SvgCache() = default;

    // 直接存储 QByteArray 避免二次转换
    QCache<QString, QByteArray> m_cache;

    // 禁止拷贝
    Q_DISABLE_COPY(SvgCache)
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
// SvgIconEngine - SVG图标引擎（优化版）
// ============================================================================
class SvgIconEngine : public QIconEngine {
public:
    explicit SvgIconEngine(const QByteArray& svg);
    ~SvgIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QByteArray m_svg;  // 直接存储字节数组
};

// ============================================================================
// FontIconEngine - 字体图标引擎
// ============================================================================
class FontIconEngine : public QIconEngine {
public:
    FontIconEngine(const QString& fontFamily, QChar charCode,
                   const QColor& color, bool isBold);
    ~FontIconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QString m_fontFamily;
    QChar m_char;  // 使用 QChar 而不是 QString
    QColor m_color;
    bool m_isBold;
};

// ============================================================================
// FluentIconBase - Fluent图标基类
// ============================================================================
class QFLUENT_EXPORT FluentIconBase {
public:
    using Ptr = QSharedPointer<FluentIconBase>;

    virtual ~FluentIconBase() = default;

    // 克隆方法
    virtual Ptr clone() const = 0;

    // 获取图标路径 - 返回 const 引用避免拷贝
    virtual QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) const = 0;

    // 创建QIcon
    virtual QIcon icon(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, const QColor& color = QColor()) const;

    // 创建彩色图标（返回智能指针）
    virtual Ptr colored(const QColor& lightColor, const QColor& darkColor);

    // 创建QIcon(带主题反转)
    QIcon qicon(bool reverse = false) const;

    // 渲染图标
    virtual void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO,
                        const QList<int>& indexes = QList<int>(),
                        const QMap<QString, QString>& attributes = QMap<QString, QString>()) const;
};

// ============================================================================
// FluentFontIconBase - Fluent字体图标基类（优化版）
// ============================================================================
class QFLUENT_EXPORT FluentFontIconBase : public FluentIconBase {
public:
    using Ptr = QSharedPointer<FluentFontIconBase>;

    explicit FluentFontIconBase(QChar charCode);
    virtual ~FluentFontIconBase() = default;

    // 克隆方法(子类需要实现)
    FluentIconBase::Ptr clone() const override = 0;

    // 提供默认实现,子类可以重写
    QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) const override;

    // 设置粗体 - 返回派生类智能指针
    Ptr bold();

    // 创建QIcon
    QIcon icon(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO, const QColor& color = QColor()) const override;

    // 设置颜色 - 返回派生类智能指针
    Ptr withColor(const QColor& lightColor, const QColor& darkColor);

    // 渲染
    void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO,
                const QList<int>& indexes = QList<int>(),
                const QMap<QString, QString>& attributes = QMap<QString, QString>()) const override;

    // 加载字体
    bool loadFont();

    // 清理字体资源
    static void cleanup();

protected:
    QChar m_char;  // 使用 QChar
    QColor m_lightColor;
    QColor m_darkColor;
    bool m_isBold;

    static bool s_isFontLoaded;
    static int s_fontId;
    static QString s_fontFamily;

private:
    QColor getIconColor(ThemeType::ThemeMode theme) const;
};

// ============================================================================
// ColoredFluentIcon - 彩色Fluent图标
// ============================================================================
class QFLUENT_EXPORT ColoredFluentIcon : public FluentIconBase {
public:
    ColoredFluentIcon(FluentIconBase::Ptr icon, const QColor& lightColor, const QColor& darkColor);
    ~ColoredFluentIcon() override = default;

    // 克隆方法
    Ptr clone() const override;

    QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) const override;

    void render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO,
                const QList<int>& indexes = QList<int>(),
                const QMap<QString, QString>& attributes = QMap<QString, QString>()) const override;

private:
    FluentIconBase::Ptr m_fluentIcon;
    QColor m_lightColor;
    QColor m_darkColor;
};

// ============================================================================
// FluentIcon - Fluent图标枚举（优化版）
// ============================================================================
class QFLUENT_EXPORT FluentIcon : public FluentIconBase {
public:
    explicit FluentIcon(FluentIconType::IconType type);
    explicit FluentIcon(const QString& templatePath);
    ~FluentIcon() override = default;

    // 克隆方法
    Ptr clone() const override;

    QString path(ThemeType::ThemeMode theme = ThemeType::ThemeMode::AUTO) const override;
    FluentIconType::IconType type() const { return m_type; }

    static QHash<FluentIconType::IconType, QString> fluentIcons();

private:
    FluentIconType::IconType m_type;
    QString m_templatePath;

    // 使用静态哈希表优化图标名称查找
    static const QHash<FluentIconType::IconType, QString>& iconNameMap();
    static QString iconName(FluentIconType::IconType type);
};

// ============================================================================
// Icon - QIcon包装类
// ============================================================================
class QFLUENT_EXPORT Icon : public QIcon {
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

// 绘制SVG图标 - 接受 QByteArray 引用
bool drawSvgIcon(const QByteArray& svgData, QPainter* painter, const QRectF& rect);

// 写入SVG(修改属性)，使用缓存 - 返回 QByteArray
QByteArray writeSvg(const QString& iconPath, const QList<int>& indexes = QList<int>(),
                    const QMap<QString, QString>& attributes = QMap<QString, QString>());

// void drawIcon(const QIcon& icon, QPainter* painter, const QRectF& rect, QIcon::State state = QIcon::Off);
// void drawIcon(const QString& iconPath, QPainter* painter, const QRectF& rect, QIcon::State state = QIcon::Off);
// void drawIcon(const FluentIconBase* icon, QPainter* painter, const QRectF& rect, QIcon::State state = QIcon::Off, const QMap<QString, QString>& attributes = QMap<QString, QString>());

// QIcon toIcon(const QIcon& icon);
// QIcon toIcon(const QString& iconPath);
// QIcon toIcon(const FluentIconBase* icon);

// ============================================================================
// Action - QAction包装类
// ============================================================================
class QFLUENT_EXPORT Action : public QAction {
    Q_OBJECT

public:
    explicit Action(QObject* parent = nullptr);
    explicit Action(const QString& text, QObject* parent = nullptr);
    Action(const QIcon& icon, const QString& text, QObject* parent = nullptr);
    Action(const FluentIconType::IconType icon, const QString& text, QObject* parent = nullptr);
    ~Action() override;

    QIcon icon() const;
    void setIcon(const FluentIconType::IconType icon);
};

#endif // FLUENTICON_H
