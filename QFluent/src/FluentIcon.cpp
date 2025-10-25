// FluentIcon.cpp
#include "FluentIcon.h"
#include <QApplication>
#include <QPalette>
#include <QSvgRenderer>
#include <QDomDocument>
#include <QFile>
#include <QFontDatabase>
#include <QPainterPath>
#include <QImage>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <algorithm>  // 用于 std::sort

// ============================================================================
// SvgCache 实现（添加线程安全）
// ============================================================================

SvgCache& SvgCache::instance() {
    static SvgCache instance;
    return instance;
}

QString SvgCache::get(const QString& key) const {
    QMutexLocker locker(&m_mutex);
    QString* cached = m_cache.object(key);
    return cached ? *cached : QString();
}

void SvgCache::insert(const QString& key, const QString& value, int cost) {
    QMutexLocker locker(&m_mutex);
    m_cache.insert(key, new QString(value), cost);
}

void SvgCache::setMaxCost(int cost) {
    QMutexLocker locker(&m_mutex);
    m_cache.setMaxCost(cost);
}

void SvgCache::clear() {
    QMutexLocker locker(&m_mutex);
    m_cache.clear();
}

QString SvgCache::generateCacheKey(const QString& iconPath,
                                   const QList<int>& indexes,
                                   const QMap<QString, QString>& attributes) {
    // 对 indexes 排序以确保哈希稳定
    QList<int> sortedIndexes = indexes;
    std::sort(sortedIndexes.begin(), sortedIndexes.end());

    // 使用更高效的键生成方式
    QString attrStr;
    for (auto it = attributes.constBegin(); it != attributes.constEnd(); ++it) {
        attrStr += it.key() + "=" + it.value() + ";";
    }

    // 组合所有部分生成唯一键
    return QString("%1|%2|%3")
        .arg(iconPath)
        .arg(qHash(sortedIndexes))
        .arg(QCryptographicHash::hash(attrStr.toUtf8(), QCryptographicHash::Md5).toHex());
}

// ============================================================================
// 主题相关函数
// ============================================================================

QString getIconColor(Theme::ThemeType theme, bool reverse) {
    QString lc = reverse ? "white" : "black";
    QString dc = reverse ? "black" : "white";

    if (theme == Theme::ThemeType::AUTO) {
        return sTheme->isDarkMode() ? dc : lc;
    } else {
        return (theme == Theme::ThemeType::DARK) ? dc : lc;
    }
}

// ============================================================================
// 辅助函数实现
// ============================================================================

bool drawSvgIcon(const QByteArray& svgData, QPainter* painter, const QRect& rect) {
    if (!painter || !painter->isActive()) {
        qWarning() << "Invalid painter in drawSvgIcon";
        return false;
    }

    QSvgRenderer renderer(svgData);
    if (!renderer.isValid()) {
        qWarning() << "Invalid SVG data";
        return false;
    }

    renderer.render(painter, QRectF(rect));
    return true;
}

QString writeSvg(const QString& iconPath, const QList<int>& indexes,
                 const QMap<QString, QString>& attributes) {
    if (!iconPath.toLower().endsWith(".svg")) {
        return QString();
    }

    // 使用缓存管理器
    QString cacheKey = SvgCache::generateCacheKey(iconPath, indexes, attributes);
    QString cached = SvgCache::instance().get(cacheKey);
    if (!cached.isEmpty()) {
        return cached;
    }

    QFile file(iconPath);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open SVG file:" << iconPath;
        return QString();
    }

    QByteArray data = file.readAll();
    file.close();

    // 统一使用 DOM 解析以确保正确处理 indexes（移除字符串替换优化以避免 bug）
    QDomDocument dom;
    if (!dom.setContent(data)) {
        qWarning() << "Failed to parse SVG:" << iconPath;
        return QString();
    }

    QDomNodeList pathNodes = dom.elementsByTagName("path");
    QList<int> targetIndexes = indexes.isEmpty() ?
                                   [&pathNodes]() {
                                       QList<int> list;
                                       for (int i = 0; i < pathNodes.length(); ++i) list.append(i);
                                       return list;
                                   }() : indexes;

    for (int i : targetIndexes) {
        if (i >= pathNodes.length()) continue;

        QDomElement element = pathNodes.at(i).toElement();
        for (auto it = attributes.constBegin(); it != attributes.constEnd(); ++it) {
            element.setAttribute(it.key(), it.value());
        }
    }

    QString modifiedSvg = dom.toString();

    // 存入缓存，使用字符串长度作为 cost 以更准确管理内存
    int cost = modifiedSvg.length() / 1024 + 1;  // 粗略估计，每 KB 一个 cost
    SvgCache::instance().insert(cacheKey, modifiedSvg, cost);
    return modifiedSvg;
}

void drawIcon(const QVariant& iconVariant, QPainter* painter, const QRect& rect,
              QIcon::State state, const QMap<QString, QString>& attributes) {
    if (!painter || !painter->isActive()) {
        return;
    }

    // 根据不同类型处理
    if (iconVariant.canConvert<FluentIconBase*>()) {
        FluentIconBase* icon = iconVariant.value<FluentIconBase*>();
        if (icon) {
            icon->render(painter, rect, Theme::ThemeType::AUTO, QList<int>(), attributes);
        }
    } else if (iconVariant.canConvert<QIcon>()) {
        QIcon icon = iconVariant.value<QIcon>();
        icon.paint(painter, rect, Qt::AlignCenter, QIcon::Normal, state);
    } else if (iconVariant.canConvert<QString>()) {
        QIcon icon(iconVariant.toString());
        icon.paint(painter, rect, Qt::AlignCenter, QIcon::Normal, state);
    }
}

QIcon toQIcon(const QVariant& iconVariant) {
    if (iconVariant.canConvert<QString>()) {
        return QIcon(iconVariant.toString());
    } else if (iconVariant.canConvert<FluentIconBase*>()) {
        FluentIconBase* icon = iconVariant.value<FluentIconBase*>();
        if (icon) {
            return icon->icon();
        }
    } else if (iconVariant.canConvert<QIcon>()) {
        return iconVariant.value<QIcon>();
    }
    return QIcon();
}

// ============================================================================
// FluentIconEngine 实现
// ============================================================================

FluentIconEngine::FluentIconEngine(QSharedPointer<FluentIconBase> icon, bool reverse)
    : m_icon(icon), m_isThemeReversed(reverse) {
    if (!m_icon) {
        qWarning() << "FluentIconEngine: Null icon pointer";
    }
}

void FluentIconEngine::paint(QPainter* painter, const QRect& rect,
                             QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive()) {
        return;
    }

    painter->save();

    if (mode == QIcon::Disabled) {
        painter->setOpacity(0.5);
    } else if (mode == QIcon::Selected) {
        painter->setOpacity(0.7);
    }

    Theme::ThemeType theme = m_isThemeReversed ?
                                 (sTheme->isDarkMode() ? Theme::ThemeType::LIGHT : Theme::ThemeType::DARK) :
                                 Theme::ThemeType::AUTO;

    QRect adjustedRect = rect;

    if (m_icon) {
        QIcon icon = m_icon->icon(theme);
        icon.paint(painter, adjustedRect, Qt::AlignCenter, mode, state);  // 传递 mode 和 state 以更好支持
    }

    painter->restore();
}

QIconEngine* FluentIconEngine::clone() const {
    return new FluentIconEngine(m_icon, m_isThemeReversed);
}

QPixmap FluentIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

    QPainter painter(&pixmap);
    if (!painter.isActive()) {
        return pixmap;
    }

    QRect rect(0, 0, size.width(), size.height());
    paint(&painter, rect, mode, state);
    return pixmap;
}

// ============================================================================
// SvgIconEngine 实现
// ============================================================================

SvgIconEngine::SvgIconEngine(const QString& svg) : m_svg(svg) {
}

void SvgIconEngine::paint(QPainter* painter, const QRect& rect,
                          QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive()) {
        return;
    }

    painter->save();

    if (mode == QIcon::Disabled) {
        painter->setOpacity(0.5);
    } else if (mode == QIcon::Selected) {
        painter->setOpacity(0.7);
    }

    drawSvgIcon(m_svg.toUtf8(), painter, rect);

    painter->restore();
}

QIconEngine* SvgIconEngine::clone() const {
    return new SvgIconEngine(m_svg);
}

QPixmap SvgIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

    QPainter painter(&pixmap);
    if (!painter.isActive()) {
        return pixmap;
    }

    QRect rect(0, 0, size.width(), size.height());
    paint(&painter, rect, mode, state);
    return pixmap;
}

// ============================================================================
// FontIconEngine 实现
// ============================================================================

FontIconEngine::FontIconEngine(const QString& fontFamily, const QString& charCode,
                               const QColor& color, bool isBold)
    : m_fontFamily(fontFamily), m_char(charCode), m_color(color), m_isBold(isBold) {
}

void FontIconEngine::paint(QPainter* painter, const QRect& rect,
                           QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive()) {
        return;
    }

    painter->save();

    if (mode == QIcon::Disabled) {
        painter->setOpacity(0.5);
    } else if (mode == QIcon::Selected) {
        painter->setOpacity(0.7);
    }

    constexpr double FONT_SCALE_FACTOR = 0.8;  // 常量化魔法数字

    QFont font(m_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(qRound(rect.height() * FONT_SCALE_FACTOR));

    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QFontMetrics fm(font);
    int x = rect.x() + (rect.width() - fm.horizontalAdvance(m_char)) / 2;
    int y = rect.y() + (rect.height() + fm.ascent() - fm.descent()) / 2;

    QPainterPath path;
    path.addText(x, y, font, m_char);
    painter->drawPath(path);

    painter->restore();
}

QIconEngine* FontIconEngine::clone() const {
    return new FontIconEngine(m_fontFamily, m_char, m_color, m_isBold);
}

QPixmap FontIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

    QPainter painter(&pixmap);
    if (!painter.isActive()) {
        return pixmap;
    }

    QRect rect(0, 0, size.width(), size.height());
    paint(&painter, rect, mode, state);
    return pixmap;
}

// ============================================================================
// FluentIconBase 实现
// ============================================================================

QIcon FluentIconBase::icon(Theme::ThemeType theme, const QColor& color) const {
    QString iconPath = path(theme);

    if (!(iconPath.endsWith(".svg") && color.isValid())) {
        return QIcon(iconPath);
    }

    QMap<QString, QString> attrs;
    attrs["fill"] = color.name();
    QString svg = writeSvg(iconPath, QList<int>(), attrs);
    return QIcon(new SvgIconEngine(svg));
}

FluentIconBase::Ptr FluentIconBase::colored(const QColor& lightColor, const QColor& darkColor) {
    return Ptr(new ColoredFluentIcon(this->clone(), lightColor, darkColor));
}

QIcon FluentIconBase::qicon(bool reverse) const {
    return QIcon(new FluentIconEngine(this->clone(), reverse));
}

void FluentIconBase::render(QPainter* painter, const QRect& rect, Theme::ThemeType theme,
                            const QList<int>& indexes,
                            const QMap<QString, QString>& attributes) const {
    if (!painter || !painter->isActive()) {
        return;
    }

    QString iconPath = path(theme);

    if (iconPath.endsWith(".svg")) {
        QByteArray svgData;
        if (!attributes.isEmpty()) {
            svgData = writeSvg(iconPath, indexes, attributes).toUtf8();
        } else {
            QFile file(iconPath);
            if (file.open(QFile::ReadOnly)) {
                svgData = file.readAll();
                file.close();
            } else {
                qWarning() << "Failed to read SVG file:" << iconPath;
                return;
            }
        }
        drawSvgIcon(svgData, painter, rect);
    } else {
        QIcon icon(iconPath);
        icon.paint(painter, rect, Qt::AlignCenter);
    }
}

// ============================================================================
// FluentFontIconBase 实现
// ============================================================================

bool FluentFontIconBase::s_isFontLoaded = false;
int FluentFontIconBase::s_fontId = -1;
QString FluentFontIconBase::s_fontFamily;

FluentFontIconBase::FluentFontIconBase(const QString& charCode)
    : m_char(charCode)
    , m_lightColor(0, 0, 0)
    , m_darkColor(255, 255, 255)
    , m_isBold(false) {
    loadFont();
}

QString FluentFontIconBase::path(Theme::ThemeType theme) const {
    Q_UNUSED(theme)
    return QString();
}

FluentFontIconBase::Ptr FluentFontIconBase::bold() {
    // 创建新的实例并设置粗体
    auto cloned = qSharedPointerDynamicCast<FluentFontIconBase>(this->clone());
    if (cloned) {
        cloned->m_isBold = true;
    }
    return cloned;
}

QIcon FluentFontIconBase::icon(Theme::ThemeType theme, const QColor& color) const {
    QColor iconColor = color.isValid() ? color : getIconColor(theme);
    return QIcon(new FontIconEngine(s_fontFamily, m_char, iconColor, m_isBold));
}

FluentFontIconBase::Ptr FluentFontIconBase::withColor(const QColor& lightColor, const QColor& darkColor) {
    // 创建新的实例并设置颜色
    auto cloned = qSharedPointerDynamicCast<FluentFontIconBase>(this->clone());
    if (cloned) {
        cloned->m_lightColor = lightColor;
        cloned->m_darkColor = darkColor;
    }
    return cloned;
}

void FluentFontIconBase::render(QPainter* painter, const QRect& rect, Theme::ThemeType theme,
                                const QList<int>& indexes,
                                const QMap<QString, QString>& attributes) const {
    Q_UNUSED(indexes)

    if (!painter || !painter->isActive()) {
        return;
    }

    QColor color = getIconColor(theme);
    if (attributes.contains("fill")) {
        color = QColor(attributes["fill"]);
    }

    constexpr double FONT_SCALE_FACTOR = 0.8;  // 常量化

    QFont font(s_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(qRound(rect.height() * FONT_SCALE_FACTOR));

    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QFontMetrics fm(font);
    int x = rect.x() + (rect.width() - fm.horizontalAdvance(m_char)) / 2;
    int y = rect.y() + (rect.height() + fm.ascent() - fm.descent()) / 2;

    QPainterPath path;
    path.addText(x, y, font, m_char);
    painter->drawPath(path);
}

bool FluentFontIconBase::loadFont() {
    if (s_isFontLoaded && s_fontId != -1 &&
        QFontDatabase::applicationFontFamilies(s_fontId).contains(s_fontFamily)) {
        return true;
    }

    if (!QApplication::instance()) {
        return false;
    }

    QString fontPath = path();
    if (fontPath.isEmpty()) {
        qWarning() << "Font path is empty";
        return false;
    }

    QFile file(fontPath);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Cannot open font file:" << fontPath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    // 清理旧字体
    if (s_fontId != -1) {
        QFontDatabase::removeApplicationFont(s_fontId);
    }

    s_fontId = QFontDatabase::addApplicationFontFromData(data);
    if (s_fontId == -1) {
        qWarning() << "Failed to load font from data";
        return false;
    }

    QStringList families = QFontDatabase::applicationFontFamilies(s_fontId);
    if (families.isEmpty()) {
        qWarning() << "No font families found";
        s_fontId = -1;
        return false;
    }
    s_fontFamily = families.first();

    // 移除未使用代码：loadIconNames() 和 s_iconNames（假设未用）

    s_isFontLoaded = true;
    return true;
}

void FluentFontIconBase::cleanup() {
    if (s_fontId != -1) {
        QFontDatabase::removeApplicationFont(s_fontId);
        s_fontId = -1;
        s_isFontLoaded = false;
        s_fontFamily.clear();
    }
}

QColor FluentFontIconBase::getIconColor(Theme::ThemeType theme) const {
    if (theme == Theme::ThemeType::AUTO) {
        return sTheme->isDarkMode() ? m_darkColor : m_lightColor;
    } else {
        return (theme == Theme::ThemeType::DARK) ? m_darkColor : m_lightColor;
    }
}

// ============================================================================
// ColoredFluentIcon 实现
// ============================================================================

ColoredFluentIcon::ColoredFluentIcon(FluentIconBase::Ptr icon,
                                     const QColor& lightColor,
                                     const QColor& darkColor)
    : m_fluentIcon(icon), m_lightColor(lightColor), m_darkColor(darkColor) {
    if (!icon) {
        qWarning() << "ColoredFluentIcon: Null icon pointer";
    }
}

FluentIconBase::Ptr ColoredFluentIcon::clone() const {
    if (!m_fluentIcon) return nullptr;
    return Ptr(new ColoredFluentIcon(m_fluentIcon->clone(), m_lightColor, m_darkColor));
}

QString ColoredFluentIcon::path(Theme::ThemeType theme) const {
    return m_fluentIcon ? m_fluentIcon->path(theme) : QString();
}

void ColoredFluentIcon::render(QPainter* painter, const QRect& rect, Theme::ThemeType theme,
                               const QList<int>& indexes,
                               const QMap<QString, QString>& attributes) const {
    if (!m_fluentIcon) return;

    QString iconPath = path(theme);

    if (!iconPath.endsWith(".svg")) {
        return m_fluentIcon->render(painter, rect, theme, indexes, attributes);
    }

    QColor color = (theme == Theme::ThemeType::AUTO) ?
                       (sTheme->isDarkMode() ? m_darkColor : m_lightColor) :
                       ((theme == Theme::ThemeType::DARK) ? m_darkColor : m_lightColor);

    QMap<QString, QString> newAttributes = attributes;
    newAttributes["fill"] = color.name();

    QByteArray svgData = writeSvg(iconPath, indexes, newAttributes).toUtf8();
    drawSvgIcon(svgData, painter, rect);
}

// ============================================================================
// FluentIcon 实现
// ============================================================================

FluentIcon::FluentIcon(IconType type) : m_type(type) {
}

FluentIconBase::Ptr FluentIcon::clone() const {
    return Ptr(new FluentIcon(m_type));
}

QString FluentIcon::path(Theme::ThemeType theme) const {
    QString color = getIconColor(theme);
    return QString(":/res/images/icons/%1_%2.svg")
        .arg(iconName(m_type))
        .arg(color);
}

QString FluentIcon::iconName(IconType type) {
    switch (type) {
    case UP: return "Up";
    case ADD: return "Add";
    case BUS: return "Bus";
    case CAR: return "Car";
    case CUT: return "Cut";
    case DELETE: return "Delete";
    case EDIT: return "Edit";
    case SEARCH: return "Search";
    default:
        qWarning() << "Unknown IconType:" << type;
        return "";
    }
}

// ============================================================================
// Icon 实现
// ============================================================================

Icon::Icon(const FluentIcon& fluentIcon)
    : QIcon(fluentIcon.path())
    , m_fluentIcon(fluentIcon) {
}
