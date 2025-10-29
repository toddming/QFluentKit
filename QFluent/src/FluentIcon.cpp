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
#include <QBuffer>
#include <algorithm>
#include "Theme.h"

// ============================================================================
// 常量定义
// ============================================================================
namespace {
    constexpr double FONT_SCALE_FACTOR = 0.8;
    constexpr int DEFAULT_CACHE_SIZE = 2048;  // KB
    constexpr int COST_PER_KB = 1;
}

// ============================================================================
// SvgCache 实现（单线程优化版）
// ============================================================================

SvgCache::SvgCache() : m_cache(DEFAULT_CACHE_SIZE) {
}

SvgCache& SvgCache::instance() {
    static SvgCache instance;
    return instance;
}

QByteArray SvgCache::get(const QString& key) const {
    QByteArray* cached = m_cache.object(key);
    return cached ? *cached : QByteArray();
}

void SvgCache::insert(const QString& key, const QByteArray& value, int cost) {
    m_cache.insert(key, new QByteArray(value), cost);
}

void SvgCache::setMaxCost(int cost) {
    m_cache.setMaxCost(cost);
}

void SvgCache::clear() {
    m_cache.clear();
}

QString SvgCache::generateCacheKey(const QString& iconPath,
                                   const QList<int>& indexes,
                                   const QMap<QString, QString>& attributes) {
    // 预分配字符串空间以减少重新分配
    QString result;
    result.reserve(iconPath.length() + 64);

    result = iconPath;
    result += '|';

    // 只有在 indexes 非空时才处理
    if (!indexes.isEmpty()) {
        QList<int> sortedIndexes = indexes;
        std::sort(sortedIndexes.begin(), sortedIndexes.end());
        result += QString::number(qHash(sortedIndexes));
    }
    result += '|';

    // 优化属性字符串生成
    if (!attributes.isEmpty()) {
        QByteArray attrBytes;
        attrBytes.reserve(attributes.size() * 32);  // 预估大小

        for (auto it = attributes.constBegin(); it != attributes.constEnd(); ++it) {
            attrBytes.append(it.key().toUtf8());
            attrBytes.append('=');
            attrBytes.append(it.value().toUtf8());
            attrBytes.append(';');
        }

        result += QCryptographicHash::hash(attrBytes, QCryptographicHash::Md5).toHex();
    }

    return result;
}

// ============================================================================
// 主题相关函数
// ============================================================================

QString getIconColor(ThemeType::ThemeMode theme, bool reverse) {
    const QString lightColor = reverse ? QStringLiteral("white") : QStringLiteral("black");
    const QString darkColor = reverse ? QStringLiteral("black") : QStringLiteral("white");

    if (theme == ThemeType::ThemeMode::AUTO) {
        return Theme::instance()->isDarkTheme() ? darkColor : lightColor;
    }
    return (theme == ThemeType::ThemeMode::DARK) ? "black" : "white";
}

// ============================================================================
// 辅助函数实现（优化版）
// ============================================================================

bool drawSvgIcon(const QByteArray& svgData, QPainter* painter, const QRectF& rect) {
    if (!painter || !painter->isActive() || svgData.isEmpty()) {
        return false;
    }

    QSvgRenderer renderer(svgData);
    if (!renderer.isValid()) {
        return false;
    }

    renderer.render(painter, rect);
    return true;
}

QByteArray writeSvg(const QString& iconPath, const QList<int>& indexes,
                    const QMap<QString, QString>& attributes) {
    if (!iconPath.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
        return QByteArray();
    }

    // 检查缓存
    const QString cacheKey = SvgCache::generateCacheKey(iconPath, indexes, attributes);
    QByteArray cached = SvgCache::instance().get(cacheKey);
    if (!cached.isEmpty()) {
        return cached;
    }

    // 读取文件
    QFile file(iconPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open SVG file:" << iconPath;
        return QByteArray();
    }

    QByteArray data = file.readAll();
    file.close();

    // 如果没有属性修改，直接缓存原始数据
    if (attributes.isEmpty()) {
        const int cost = data.size() / 1024 + COST_PER_KB;
        SvgCache::instance().insert(cacheKey, data, cost);
        return data;
    }

    // DOM 解析和修改
    QDomDocument dom;
    if (!dom.setContent(data)) {
        qWarning() << "Failed to parse SVG:" << iconPath;
        return QByteArray();
    }

    QDomNodeList pathNodes = dom.elementsByTagName(QStringLiteral("path"));
    const int pathCount = pathNodes.length();

    // 确定要修改的索引
    QList<int> targetIndexes = indexes;
    if (targetIndexes.isEmpty()) {
        targetIndexes.reserve(pathCount);
        for (int i = 0; i < pathCount; ++i) {
            targetIndexes.append(i);
        }
    }

    // 应用属性
    for (int i : targetIndexes) {
        if (i >= pathCount) continue;

        QDomElement element = pathNodes.at(i).toElement();
        for (auto it = attributes.constBegin(); it != attributes.constEnd(); ++it) {
            element.setAttribute(it.key(), it.value());
        }
    }

    // 转换为字节数组
    QByteArray modifiedSvg = dom.toByteArray();

    // 存入缓存
    const int cost = modifiedSvg.size() / 1024 + COST_PER_KB;
    SvgCache::instance().insert(cacheKey, modifiedSvg, cost);

    return modifiedSvg;
}

void drawIcon(const QVariant& iconVariant, QPainter* painter, const QRectF& rect,
              QIcon::State state, const QMap<QString, QString>& attributes) {
    if (!painter || !painter->isActive()) {
        return;
    }

    // 优化类型检查顺序（最常用的放前面）
    if (iconVariant.canConvert<QIcon>()) {
        QIcon icon = iconVariant.value<QIcon>();
        icon.paint(painter, rect.toRect(), Qt::AlignCenter, QIcon::Normal, state);
    } else if (iconVariant.canConvert<QString>()) {
        QIcon icon(iconVariant.toString());
        icon.paint(painter, rect.toRect(), Qt::AlignCenter, QIcon::Normal, state);
    } else if (iconVariant.canConvert<FluentIconBase*>()) {
        FluentIconBase* icon = iconVariant.value<FluentIconBase*>();
        if (icon) {
            icon->render(painter, rect, ThemeType::ThemeMode::AUTO, QList<int>(), attributes);
        }
    }
}

QIcon toQIcon(const QVariant& iconVariant) {
    if (iconVariant.canConvert<QIcon>()) {
        return iconVariant.value<QIcon>();
    } else if (iconVariant.canConvert<QString>()) {
        return QIcon(iconVariant.toString());
    } else if (iconVariant.canConvert<FluentIconBase*>()) {
        FluentIconBase* icon = iconVariant.value<FluentIconBase*>();
        if (icon) {
            return icon->icon();
        }
    }
    return QIcon();
}

// ============================================================================
// FluentIconEngine 实现
// ============================================================================

FluentIconEngine::FluentIconEngine(QSharedPointer<FluentIconBase> icon, bool reverse)
    : m_icon(std::move(icon)), m_isThemeReversed(reverse) {
    if (!m_icon) {
        qWarning() << "FluentIconEngine: Null icon pointer";
    }
}

void FluentIconEngine::paint(QPainter* painter, const QRect& rect,
                             QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive() || !m_icon) {
        return;
    }

    painter->save();

    // 根据模式设置透明度
    switch (mode) {
        case QIcon::Disabled:
            painter->setOpacity(0.5);
            break;
        case QIcon::Selected:
            painter->setOpacity(0.7);
            break;
        default:
            break;
    }

    const ThemeType::ThemeMode theme = m_isThemeReversed ?
                (Theme::instance()->isDarkTheme() ? ThemeType::ThemeMode::LIGHT : ThemeType::ThemeMode::DARK) :
                ThemeType::ThemeMode::AUTO;

    QIcon icon = m_icon->icon(theme);
    icon.paint(painter, rect, Qt::AlignCenter, mode, state);

    painter->restore();
}

QIconEngine* FluentIconEngine::clone() const {
    return new FluentIconEngine(m_icon, m_isThemeReversed);
}

QPixmap FluentIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    if (painter.isActive()) {
        paint(&painter, QRect(QPoint(0, 0), size), mode, state);
    }

    return pixmap;
}

// ============================================================================
// SvgIconEngine 实现（优化版）
// ============================================================================

SvgIconEngine::SvgIconEngine(const QByteArray& svg) : m_svg(svg) {
}

void SvgIconEngine::paint(QPainter* painter, const QRect& rect,
                          QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive()) {
        return;
    }

    painter->save();

    switch (mode) {
        case QIcon::Disabled:
            painter->setOpacity(0.5);
            break;
        case QIcon::Selected:
            painter->setOpacity(0.7);
            break;
        default:
            break;
    }

    drawSvgIcon(m_svg, painter, rect);

    painter->restore();
}

QIconEngine* SvgIconEngine::clone() const {
    return new SvgIconEngine(m_svg);
}

QPixmap SvgIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    if (painter.isActive()) {
        paint(&painter, QRect(QPoint(0, 0), size), mode, state);
    }

    return pixmap;
}

// ============================================================================
// FontIconEngine 实现（优化版）
// ============================================================================

FontIconEngine::FontIconEngine(const QString& fontFamily, QChar charCode,
                               const QColor& color, bool isBold)
    : m_fontFamily(fontFamily), m_char(charCode), m_color(color), m_isBold(isBold) {
}

void FontIconEngine::paint(QPainter* painter, const QRect& rect,
                           QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive()) {
        return;
    }

    painter->save();

    switch (mode) {
        case QIcon::Disabled:
            painter->setOpacity(0.5);
            break;
        case QIcon::Selected:
            painter->setOpacity(0.7);
            break;
        default:
            break;
    }

    QFont font(m_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(qRound(rect.height() * FONT_SCALE_FACTOR));

    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    const QFontMetrics fm(font);
    const int x = rect.x() + (rect.width() - fm.horizontalAdvance(m_char)) / 2;
    const int y = rect.y() + (rect.height() + fm.ascent() - fm.descent()) / 2;

    QPainterPath path;
    path.addText(x, y, font, QString(m_char));
    painter->drawPath(path);

    painter->restore();
}

QIconEngine* FontIconEngine::clone() const {
    return new FontIconEngine(m_fontFamily, m_char, m_color, m_isBold);
}

QPixmap FontIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    if (painter.isActive()) {
        paint(&painter, QRect(QPoint(0, 0), size), mode, state);
    }

    return pixmap;
}

// ============================================================================
// FluentIconBase 实现
// ============================================================================

QIcon FluentIconBase::icon(ThemeType::ThemeMode theme, const QColor& color) const {
    const QString iconPath = path(theme);

    if (!(iconPath.endsWith(QStringLiteral(".svg")) && color.isValid())) {
        return QIcon(iconPath);
    }

    QMap<QString, QString> attrs;
    attrs[QStringLiteral("fill")] = color.name();
    const QByteArray svg = writeSvg(iconPath, QList<int>(), attrs);
    return QIcon(new SvgIconEngine(svg));
}

FluentIconBase::Ptr FluentIconBase::colored(const QColor& lightColor, const QColor& darkColor) {
    return Ptr(new ColoredFluentIcon(this->clone(), lightColor, darkColor));
}

QIcon FluentIconBase::qicon(bool reverse) const {
    return QIcon(new FluentIconEngine(this->clone(), reverse));
}

void FluentIconBase::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                            const QList<int>& indexes,
                            const QMap<QString, QString>& attributes) const {
    if (!painter || !painter->isActive()) {
        return;
    }

    const QString iconPath = path(theme);

    if (iconPath.endsWith(QStringLiteral(".svg"))) {
        const QByteArray svgData = attributes.isEmpty() ?
                    [&iconPath]() {
                        QFile file(iconPath);
                        if (file.open(QIODevice::ReadOnly)) {
                            return file.readAll();
                        }
                        return QByteArray();
                    }() :
                    writeSvg(iconPath, indexes, attributes);

        if (!svgData.isEmpty()) {
            drawSvgIcon(svgData, painter, rect);
        }
    } else {
        QIcon icon(iconPath);
        icon.paint(painter, rect.toRect(), Qt::AlignCenter);
    }
}

// ============================================================================
// FluentFontIconBase 实现（优化版）
// ============================================================================

bool FluentFontIconBase::s_isFontLoaded = false;
int FluentFontIconBase::s_fontId = -1;
QString FluentFontIconBase::s_fontFamily;

FluentFontIconBase::FluentFontIconBase(QChar charCode)
    : m_char(charCode)
    , m_lightColor(0, 0, 0)
    , m_darkColor(255, 255, 255)
    , m_isBold(false) {
    loadFont();
}

QString FluentFontIconBase::path(ThemeType::ThemeMode theme) const {
    Q_UNUSED(theme)
    return QString();
}

FluentFontIconBase::Ptr FluentFontIconBase::bold() {
    auto cloned = qSharedPointerDynamicCast<FluentFontIconBase>(this->clone());
    if (cloned) {
        cloned->m_isBold = true;
    }
    return cloned;
}

QIcon FluentFontIconBase::icon(ThemeType::ThemeMode theme, const QColor& color) const {
    const QColor iconColor = color.isValid() ? color : getIconColor(theme);
    return QIcon(new FontIconEngine(s_fontFamily, m_char, iconColor, m_isBold));
}

FluentFontIconBase::Ptr FluentFontIconBase::withColor(const QColor& lightColor, const QColor& darkColor) {
    auto cloned = qSharedPointerDynamicCast<FluentFontIconBase>(this->clone());
    if (cloned) {
        cloned->m_lightColor = lightColor;
        cloned->m_darkColor = darkColor;
    }
    return cloned;
}

void FluentFontIconBase::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                                const QList<int>& indexes,
                                const QMap<QString, QString>& attributes) const {
    Q_UNUSED(indexes)

    if (!painter || !painter->isActive()) {
        return;
    }

    QColor color = attributes.contains(QStringLiteral("fill")) ?
                   QColor(attributes[QStringLiteral("fill")]) :
                   getIconColor(theme);

    QFont font(s_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(qRound(rect.height() * FONT_SCALE_FACTOR));

    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    const QFontMetrics fm(font);
    const int x = rect.x() + (rect.width() - fm.horizontalAdvance(m_char)) / 2;
    const int y = rect.y() + (rect.height() + fm.ascent() - fm.descent()) / 2;

    QPainterPath path;
    path.addText(x, y, font, QString(m_char));
    painter->drawPath(path);
}

bool FluentFontIconBase::loadFont() {
    // 快速路径：字体已加载
    if (s_isFontLoaded && s_fontId != -1 &&
            QFontDatabase::applicationFontFamilies(s_fontId).contains(s_fontFamily)) {
        return true;
    }

    if (!QApplication::instance()) {
        return false;
    }

    const QString fontPath = path();
    if (fontPath.isEmpty()) {
        qWarning() << "Font path is empty";
        return false;
    }

    QFile file(fontPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open font file:" << fontPath;
        return false;
    }

    const QByteArray data = file.readAll();
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

    const QStringList families = QFontDatabase::applicationFontFamilies(s_fontId);
    if (families.isEmpty()) {
        qWarning() << "No font families found";
        s_fontId = -1;
        return false;
    }
    s_fontFamily = families.first();

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

QColor FluentFontIconBase::getIconColor(ThemeType::ThemeMode theme) const {
    if (theme == ThemeType::ThemeMode::AUTO) {
        return Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
    }
    return (theme == ThemeType::ThemeMode::DARK) ? m_darkColor : m_lightColor;
}

// ============================================================================
// ColoredFluentIcon 实现
// ============================================================================

ColoredFluentIcon::ColoredFluentIcon(FluentIconBase::Ptr icon,
                                     const QColor& lightColor,
                                     const QColor& darkColor)
    : m_fluentIcon(std::move(icon)), m_lightColor(lightColor), m_darkColor(darkColor) {
    if (!m_fluentIcon) {
        qWarning() << "ColoredFluentIcon: Null icon pointer";
    }
}

FluentIconBase::Ptr ColoredFluentIcon::clone() const {
    if (!m_fluentIcon) return nullptr;
    // 优化：如果内部图标是不可变的（如 FluentIcon），直接共享以节省内存
    // 如果是可变的（如 FluentFontIconBase 调用过 bold()），则需要深拷贝
    // 这里采用深拷贝策略以确保安全性，避免共享状态导致的问题
    return Ptr(new ColoredFluentIcon(m_fluentIcon->clone(), m_lightColor, m_darkColor));
}

QString ColoredFluentIcon::path(ThemeType::ThemeMode theme) const {
    return m_fluentIcon ? m_fluentIcon->path(theme) : QString();
}

void ColoredFluentIcon::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                               const QList<int>& indexes,
                               const QMap<QString, QString>& attributes) const {
    if (!m_fluentIcon) return;

    const QString iconPath = path(theme);

    if (!iconPath.endsWith(QStringLiteral(".svg"))) {
        return m_fluentIcon->render(painter, rect, theme, indexes, attributes);
    }

    const QColor color = (theme == ThemeType::ThemeMode::AUTO) ?
                (Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor) :
                ((theme == ThemeType::ThemeMode::DARK) ? m_darkColor : m_lightColor);

    QMap<QString, QString> newAttributes = attributes;
    newAttributes[QStringLiteral("fill")] = color.name();

    const QByteArray svgData = writeSvg(iconPath, indexes, newAttributes);
    drawSvgIcon(svgData, painter, rect);
}

// ============================================================================
// FluentIcon 实现（优化版 - 使用静态哈希表）
// ============================================================================

FluentIcon::FluentIcon(FluentIconType::IconType type) : m_type(type) {
}

FluentIcon::FluentIcon(const QString& templatePath)
    : m_type(FluentIconType::CUSTOM_PATH)
    , m_templatePath(templatePath) {}

FluentIconBase::Ptr FluentIcon::clone() const {
    auto icon = new FluentIcon(m_type);
    icon->m_templatePath = this->m_templatePath;
    return Ptr(icon);
}

QString FluentIcon::path(ThemeType::ThemeMode theme) const {
    const QString color = getIconColor(theme);
    if (m_type != FluentIconType::CUSTOM_PATH) {
        return QStringLiteral(":/res/images/icons/%1_%2.svg")
                .arg(iconName(m_type), color);
    } else {
        QString result = m_templatePath;
        result.replace(QStringLiteral("{color}"), color);
        return result;
    }
}

const QHash<FluentIconType::IconType, QString>& FluentIcon::iconNameMap() {
    // 使用静态局部变量确保线程安全的单次初始化
    static const QHash<FluentIconType::IconType, QString> map = {
        {FluentIconType::UP, QStringLiteral("Up")},
        {FluentIconType::ADD, QStringLiteral("Add")},
        {FluentIconType::BUS, QStringLiteral("Bus")},
        {FluentIconType::CAR, QStringLiteral("Car")},
        {FluentIconType::CUT, QStringLiteral("Cut")},
        {FluentIconType::IOT, QStringLiteral("IOT")},
        {FluentIconType::PIN, QStringLiteral("Pin")},
        {FluentIconType::TAG, QStringLiteral("Tag")},
        {FluentIconType::VPN, QStringLiteral("VPN")},
        {FluentIconType::CAFE, QStringLiteral("Cafe")},
        {FluentIconType::CHAT, QStringLiteral("Chat")},
        {FluentIconType::COPY, QStringLiteral("Copy")},
        {FluentIconType::CODE, QStringLiteral("Code")},
        {FluentIconType::DOWN, QStringLiteral("Down")},
        {FluentIconType::EDIT, QStringLiteral("Edit")},
        {FluentIconType::FLAG, QStringLiteral("Flag")},
        {FluentIconType::FONT, QStringLiteral("Font")},
        {FluentIconType::GAME, QStringLiteral("Game")},
        {FluentIconType::HELP, QStringLiteral("Help")},
        {FluentIconType::HIDE, QStringLiteral("Hide")},
        {FluentIconType::HOME, QStringLiteral("Home")},
        {FluentIconType::INFO, QStringLiteral("Info")},
        {FluentIconType::LEAF, QStringLiteral("Leaf")},
        {FluentIconType::LINK, QStringLiteral("Link")},
        {FluentIconType::MAIL, QStringLiteral("Mail")},
        {FluentIconType::MENU, QStringLiteral("Menu")},
        {FluentIconType::MUTE, QStringLiteral("Mute")},
        {FluentIconType::MORE, QStringLiteral("More")},
        {FluentIconType::MOVE, QStringLiteral("Move")},
        {FluentIconType::PLAY, QStringLiteral("Play")},
        {FluentIconType::SAVE, QStringLiteral("Save")},
        {FluentIconType::SEND, QStringLiteral("Send")},
        {FluentIconType::SYNC, QStringLiteral("Sync")},
        {FluentIconType::UNIT, QStringLiteral("Unit")},
        {FluentIconType::VIEW, QStringLiteral("View")},
        {FluentIconType::WIFI, QStringLiteral("Wifi")},
        {FluentIconType::ZOOM, QStringLiteral("Zoom")},
        {FluentIconType::ALBUM, QStringLiteral("Album")},
        {FluentIconType::BRUSH, QStringLiteral("Brush")},
        {FluentIconType::BROOM, QStringLiteral("Broom")},
        {FluentIconType::CLOSE, QStringLiteral("Close")},
        {FluentIconType::CLOUD, QStringLiteral("Cloud")},
        {FluentIconType::EMBED, QStringLiteral("Embed")},
        {FluentIconType::GLOBE, QStringLiteral("Globe")},
        {FluentIconType::HEART, QStringLiteral("Heart")},
        {FluentIconType::LABEL, QStringLiteral("Label")},
        {FluentIconType::MEDIA, QStringLiteral("Media")},
        {FluentIconType::MOVIE, QStringLiteral("Movie")},
        {FluentIconType::MUSIC, QStringLiteral("Music")},
        {FluentIconType::ROBOT, QStringLiteral("Robot")},
        {FluentIconType::PAUSE, QStringLiteral("Pause")},
        {FluentIconType::PASTE, QStringLiteral("Paste")},
        {FluentIconType::PHOTO, QStringLiteral("Photo")},
        {FluentIconType::PHONE, QStringLiteral("Phone")},
        {FluentIconType::PRINT, QStringLiteral("Print")},
        {FluentIconType::SHARE, QStringLiteral("Share")},
        {FluentIconType::TILES, QStringLiteral("Tiles")},
        {FluentIconType::UNPIN, QStringLiteral("Unpin")},
        {FluentIconType::VIDEO, QStringLiteral("Video")},
        {FluentIconType::TRAIN, QStringLiteral("Train")},
        {FluentIconType::ADD_TO, QStringLiteral("AddTo")},
        {FluentIconType::ACCEPT, QStringLiteral("Accept")},
        {FluentIconType::CAMERA, QStringLiteral("Camera")},
        {FluentIconType::CANCEL, QStringLiteral("Cancel")},
        {FluentIconType::DELETE, QStringLiteral("Delete")},
        {FluentIconType::FOLDER, QStringLiteral("Folder")},
        {FluentIconType::FILTER, QStringLiteral("Filter")},
        {FluentIconType::MARKET, QStringLiteral("Market")},
        {FluentIconType::SCROLL, QStringLiteral("Scroll")},
        {FluentIconType::LAYOUT, QStringLiteral("Layout")},
        {FluentIconType::GITHUB, QStringLiteral("GitHub")},
        {FluentIconType::UPDATE, QStringLiteral("Update")},
        {FluentIconType::REMOVE, QStringLiteral("Remove")},
        {FluentIconType::RETURN, QStringLiteral("Return")},
        {FluentIconType::PEOPLE, QStringLiteral("People")},
        {FluentIconType::QRCODE, QStringLiteral("QRCode")},
        {FluentIconType::RINGER, QStringLiteral("Ringer")},
        {FluentIconType::ROTATE, QStringLiteral("Rotate")},
        {FluentIconType::SEARCH, QStringLiteral("Search")},
        {FluentIconType::VOLUME, QStringLiteral("Volume")},
        {FluentIconType::FRIGID, QStringLiteral("Frigid")},
        {FluentIconType::SAVE_AS, QStringLiteral("SaveAs")},
        {FluentIconType::ZOOM_IN, QStringLiteral("ZoomIn")},
        {FluentIconType::CONNECT, QStringLiteral("Connect")},
        {FluentIconType::HISTORY, QStringLiteral("History")},
        {FluentIconType::SETTING, QStringLiteral("Setting")},
        {FluentIconType::PALETTE, QStringLiteral("Palette")},
        {FluentIconType::MESSAGE, QStringLiteral("Message")},
        {FluentIconType::FIT_PAGE, QStringLiteral("FitPage")},
        {FluentIconType::ZOOM_OUT, QStringLiteral("ZoomOut")},
        {FluentIconType::AIRPLANE, QStringLiteral("Airplane")},
        {FluentIconType::ASTERISK, QStringLiteral("Asterisk")},
        {FluentIconType::CALORIES, QStringLiteral("Calories")},
        {FluentIconType::CALENDAR, QStringLiteral("Calendar")},
        {FluentIconType::FEEDBACK, QStringLiteral("Feedback")},
        {FluentIconType::LIBRARY, QStringLiteral("BookShelf")},
        {FluentIconType::MINIMIZE, QStringLiteral("Minimize")},
        {FluentIconType::CHECKBOX, QStringLiteral("CheckBox")},
        {FluentIconType::DOCUMENT, QStringLiteral("Document")},
        {FluentIconType::LANGUAGE, QStringLiteral("Language")},
        {FluentIconType::DOWNLOAD, QStringLiteral("Download")},
        {FluentIconType::QUESTION, QStringLiteral("Question")},
        {FluentIconType::SPEAKERS, QStringLiteral("Speakers")},
        {FluentIconType::DATE_TIME, QStringLiteral("DateTime")},
        {FluentIconType::FONT_SIZE, QStringLiteral("FontSize")},
        {FluentIconType::HOME_FILL, QStringLiteral("HomeFill")},
        {FluentIconType::PAGE_LEFT, QStringLiteral("PageLeft")},
        {FluentIconType::SAVE_COPY, QStringLiteral("SaveCopy")},
        {FluentIconType::SEND_FILL, QStringLiteral("SendFill")},
        {FluentIconType::SKIP_BACK, QStringLiteral("SkipBack")},
        {FluentIconType::SPEED_OFF, QStringLiteral("SpeedOff")},
        {FluentIconType::ALIGNMENT, QStringLiteral("Alignment")},
        {FluentIconType::BLUETOOTH, QStringLiteral("Bluetooth")},
        {FluentIconType::COMPLETED, QStringLiteral("Completed")},
        {FluentIconType::CONSTRACT, QStringLiteral("Constract")},
        {FluentIconType::HEADPHONE, QStringLiteral("Headphone")},
        {FluentIconType::MEGAPHONE, QStringLiteral("Megaphone")},
        {FluentIconType::PROJECTOR, QStringLiteral("Projector")},
        {FluentIconType::EDUCATION, QStringLiteral("Education")},
        {FluentIconType::LEFT_ARROW, QStringLiteral("LeftArrow")},
        {FluentIconType::ERASE_TOOL, QStringLiteral("EraseTool")},
        {FluentIconType::PAGE_RIGHT, QStringLiteral("PageRight")},
        {FluentIconType::PLAY_SOLID, QStringLiteral("PlaySolid")},
        {FluentIconType::BOOK_SHELF, QStringLiteral("BookShelf")},
        {FluentIconType::HIGHTLIGHT, QStringLiteral("Highlight")},
        {FluentIconType::FOLDER_ADD, QStringLiteral("FolderAdd")},
        {FluentIconType::PAUSE_BOLD, QStringLiteral("PauseBold")},
        {FluentIconType::PENCIL_INK, QStringLiteral("PencilInk")},
        {FluentIconType::PIE_SINGLE, QStringLiteral("PieSingle")},
        {FluentIconType::QUICK_NOTE, QStringLiteral("QuickNote")},
        {FluentIconType::SPEED_HIGH, QStringLiteral("SpeedHigh")},
        {FluentIconType::STOP_WATCH, QStringLiteral("StopWatch")},
        {FluentIconType::ZIP_FOLDER, QStringLiteral("ZipFolder")},
        {FluentIconType::BASKETBALL, QStringLiteral("Basketball")},
        {FluentIconType::BRIGHTNESS, QStringLiteral("Brightness")},
        {FluentIconType::DICTIONARY, QStringLiteral("Dictionary")},
        {FluentIconType::MICROPHONE, QStringLiteral("Microphone")},
        {FluentIconType::ARROW_DOWN, QStringLiteral("ChevronDown")},
        {FluentIconType::FULL_SCREEN, QStringLiteral("FullScreen")},
        {FluentIconType::MIX_VOLUMES, QStringLiteral("MixVolumes")},
        {FluentIconType::REMOVE_FROM, QStringLiteral("RemoveFrom")},
        {FluentIconType::RIGHT_ARROW, QStringLiteral("RightArrow")},
        {FluentIconType::QUIET_HOURS, QStringLiteral("QuietHours")},
        {FluentIconType::FINGERPRINT, QStringLiteral("Fingerprint")},
        {FluentIconType::APPLICATION, QStringLiteral("Application")},
        {FluentIconType::CERTIFICATE, QStringLiteral("Certificate")},
        {FluentIconType::TRANSPARENT, QStringLiteral("Transparent")},
        {FluentIconType::IMAGE_EXPORT, QStringLiteral("ImageExport")},
        {FluentIconType::SPEED_MEDIUM, QStringLiteral("SpeedMedium")},
        {FluentIconType::LIBRARY_FILL, QStringLiteral("LibraryFill")},
        {FluentIconType::MUSIC_FOLDER, QStringLiteral("MusicFolder")},
        {FluentIconType::POWER_BUTTON, QStringLiteral("PowerButton")},
        {FluentIconType::SKIP_FORWARD, QStringLiteral("SkipForward")},
        {FluentIconType::CARE_UP_SOLID, QStringLiteral("CareUpSolid")},
        {FluentIconType::ACCEPT_MEDIUM, QStringLiteral("AcceptMedium")},
        {FluentIconType::CANCEL_MEDIUM, QStringLiteral("CancelMedium")},
        {FluentIconType::CHEVRON_RIGHT, QStringLiteral("ChevronRight")},
        {FluentIconType::CLIPPING_TOOL, QStringLiteral("ClippingTool")},
        {FluentIconType::SEARCH_MIRROR, QStringLiteral("SearchMirror")},
        {FluentIconType::SHOPPING_CART, QStringLiteral("ShoppingCart")},
        {FluentIconType::FONT_INCREASE, QStringLiteral("FontIncrease")},
        {FluentIconType::BACK_TO_WINDOW, QStringLiteral("BackToWindow")},
        {FluentIconType::COMMAND_PROMPT, QStringLiteral("CommandPrompt")},
        {FluentIconType::CLOUD_DOWNLOAD, QStringLiteral("CloudDownload")},
        {FluentIconType::DICTIONARY_ADD, QStringLiteral("DictionaryAdd")},
        {FluentIconType::CARE_DOWN_SOLID, QStringLiteral("CareDownSolid")},
        {FluentIconType::CARE_LEFT_SOLID, QStringLiteral("CareLeftSolid")},
        {FluentIconType::CLEAR_SELECTION, QStringLiteral("ClearSelection")},
        {FluentIconType::DEVELOPER_TOOLS, QStringLiteral("DeveloperTools")},
        {FluentIconType::BACKGROUND_FILL, QStringLiteral("BackgroundColor")},
        {FluentIconType::CARE_RIGHT_SOLID, QStringLiteral("CareRightSolid")},
        {FluentIconType::CHEVRON_DOWN_MED, QStringLiteral("ChevronDownMed")},
        {FluentIconType::CHEVRON_RIGHT_MED, QStringLiteral("ChevronRightMed")},
        {FluentIconType::EMOJI_TAB_SYMBOLS, QStringLiteral("EmojiTabSymbols")},
        {FluentIconType::EXPRESSIVE_INPUT_ENTRY, QStringLiteral("ExpressiveInputEntry")}
    };
    return map;
}

QString FluentIcon::iconName(FluentIconType::IconType type) {
    const auto& map = iconNameMap();
    auto it = map.constFind(type);
    if (it != map.constEnd()) {
        return it.value();
    }
    qWarning() << "Unknown FluentIconType::IconType:" << type;
    return QString();
}

QHash<FluentIconType::IconType, QString> FluentIcon::fluentIcons()
{
    return iconNameMap();
}

// ============================================================================
// Icon 实现
// ============================================================================

Icon::Icon(const FluentIcon& fluentIcon)
    : QIcon(fluentIcon.path())
    , m_fluentIcon(fluentIcon) {
}

// ============================================================================
// Action 实现
// ============================================================================

Action::Action(QObject* parent) : QAction(parent) {
}

Action::Action(const QString& text, QObject* parent) : QAction(text, parent) {
}

Action::Action(const QIcon& icon, const QString& text, QObject* parent)
    : QAction(icon, text, parent) {
}

Action::Action(const FluentIconType::IconType icon, const QString& text, QObject* parent)
    : QAction(text, parent) {
    setIcon(icon);
}

Action::~Action() = default;

QIcon Action::icon() const {
    return QAction::icon();
}

void Action::setIcon(const FluentIconType::IconType icon) {
    QAction::setIcon(FluentIcon(icon).qicon());
}
