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
#include "Theme.h"
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

QString getIconColor(ThemeType::ThemeMode theme, bool reverse) {
    QString lc = reverse ? "white" : "black";
    QString dc = reverse ? "black" : "white";

    if (theme == ThemeType::ThemeMode::AUTO) {
        return Theme::instance()->isDarkTheme() ? dc : lc;
    } else {
        return (theme == ThemeType::ThemeMode::DARK) ? dc : lc;
    }
}

// ============================================================================
// 辅助函数实现
// ============================================================================

bool drawSvgIcon(const QByteArray& svgData, QPainter* painter, const QRectF& rect) {
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

void drawIcon(const QVariant& iconVariant, QPainter* painter, const QRectF& rect,
              QIcon::State state, const QMap<QString, QString>& attributes) {
    if (!painter || !painter->isActive()) {
        return;
    }

    // 根据不同类型处理
    if (iconVariant.canConvert<FluentIconBase*>()) {
        FluentIconBase* icon = iconVariant.value<FluentIconBase*>();
        if (icon) {
            icon->render(painter, rect, ThemeType::ThemeMode::AUTO, QList<int>(), attributes);
        }
    } else if (iconVariant.canConvert<QIcon>()) {
        QIcon icon = iconVariant.value<QIcon>();
        icon.paint(painter, rect.toRect(), Qt::AlignCenter, QIcon::Normal, state);
    } else if (iconVariant.canConvert<QString>()) {
        QIcon icon(iconVariant.toString());
        icon.paint(painter, rect.toRect(), Qt::AlignCenter, QIcon::Normal, state);
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

    ThemeType::ThemeMode theme = m_isThemeReversed ?
                (Theme::instance()->isDarkTheme() ? ThemeType::ThemeMode::LIGHT : ThemeType::ThemeMode::DARK) :
                ThemeType::ThemeMode::AUTO;

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

QIcon FluentIconBase::icon(ThemeType::ThemeMode theme, const QColor& color) const {
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

void FluentIconBase::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
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
        icon.paint(painter, rect.toRect(), Qt::AlignCenter);
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

QString FluentFontIconBase::path(ThemeType::ThemeMode theme) const {
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

QIcon FluentFontIconBase::icon(ThemeType::ThemeMode theme, const QColor& color) const {
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

void FluentFontIconBase::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
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

QColor FluentFontIconBase::getIconColor(ThemeType::ThemeMode theme) const {
    if (theme == ThemeType::ThemeMode::AUTO) {
        return Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
    } else {
        return (theme == ThemeType::ThemeMode::DARK) ? m_darkColor : m_lightColor;
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

QString ColoredFluentIcon::path(ThemeType::ThemeMode theme) const {
    return m_fluentIcon ? m_fluentIcon->path(theme) : QString();
}

void ColoredFluentIcon::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                               const QList<int>& indexes,
                               const QMap<QString, QString>& attributes) const {
    if (!m_fluentIcon) return;

    QString iconPath = path(theme);

    if (!iconPath.endsWith(".svg")) {
        return m_fluentIcon->render(painter, rect, theme, indexes, attributes);
    }

    QColor color = (theme == ThemeType::ThemeMode::AUTO) ?
                (Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor) :
                ((theme == ThemeType::ThemeMode::DARK) ? m_darkColor : m_lightColor);

    QMap<QString, QString> newAttributes = attributes;
    newAttributes["fill"] = color.name();

    QByteArray svgData = writeSvg(iconPath, indexes, newAttributes).toUtf8();
    drawSvgIcon(svgData, painter, rect);
}

// ============================================================================
// FluentIcon 实现
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
    QString color = getIconColor(theme);
    if (m_type != FluentIconType::CUSTOM_PATH) {
        return QString(":/res/images/icons/%1_%2.svg")
                .arg(iconName(m_type))
                .arg(color);
    } else {
        QString result = m_templatePath;
        result.replace("{color}", color);
        return result;
    }
}

QString FluentIcon::iconName(FluentIconType::IconType type) {
    switch (type) {
    case FluentIconType::UP: return "Up";
    case FluentIconType::ADD: return "Add";
    case FluentIconType::BUS: return "Bus";
    case FluentIconType::CAR: return "Car";
    case FluentIconType::CUT: return "Cut";
    case FluentIconType::IOT: return "IOT";
    case FluentIconType::PIN: return "Pin";
    case FluentIconType::TAG: return "Tag";
    case FluentIconType::VPN: return "VPN";
    case FluentIconType::CAFE: return "Cafe";
    case FluentIconType::CHAT: return "Chat";
    case FluentIconType::COPY: return "Copy";
    case FluentIconType::CODE: return "Code";
    case FluentIconType::DOWN: return "Down";
    case FluentIconType::EDIT: return "Edit";
    case FluentIconType::FLAG: return "Flag";
    case FluentIconType::FONT: return "Font";
    case FluentIconType::GAME: return "Game";
    case FluentIconType::HELP: return "Help";
    case FluentIconType::HIDE: return "Hide";
    case FluentIconType::HOME: return "Home";
    case FluentIconType::INFO: return "Info";
    case FluentIconType::LEAF: return "Leaf";
    case FluentIconType::LINK: return "Link";
    case FluentIconType::MAIL: return "Mail";
    case FluentIconType::MENU: return "Menu";
    case FluentIconType::MUTE: return "Mute";
    case FluentIconType::MORE: return "More";
    case FluentIconType::MOVE: return "Move";
    case FluentIconType::PLAY: return "Play";
    case FluentIconType::SAVE: return "Save";
    case FluentIconType::SEND: return "Send";
    case FluentIconType::SYNC: return "Sync";
    case FluentIconType::UNIT: return "Unit";
    case FluentIconType::VIEW: return "View";
    case FluentIconType::WIFI: return "Wifi";
    case FluentIconType::ZOOM: return "Zoom";
    case FluentIconType::ALBUM: return "Album";
    case FluentIconType::BRUSH: return "Brush";
    case FluentIconType::BROOM: return "Broom";
    case FluentIconType::CLOSE: return "Close";
    case FluentIconType::CLOUD: return "Cloud";
    case FluentIconType::EMBED: return "Embed";
    case FluentIconType::GLOBE: return "Globe";
    case FluentIconType::HEART: return "Heart";
    case FluentIconType::LABEL: return "Label";
    case FluentIconType::MEDIA: return "Media";
    case FluentIconType::MOVIE: return "Movie";
    case FluentIconType::MUSIC: return "Music";
    case FluentIconType::ROBOT: return "Robot";
    case FluentIconType::PAUSE: return "Pause";
    case FluentIconType::PASTE: return "Paste";
    case FluentIconType::PHOTO: return "Photo";
    case FluentIconType::PHONE: return "Phone";
    case FluentIconType::PRINT: return "Print";
    case FluentIconType::SHARE: return "Share";
    case FluentIconType::TILES: return "Tiles";
    case FluentIconType::UNPIN: return "Unpin";
    case FluentIconType::VIDEO: return "Video";
    case FluentIconType::TRAIN: return "Train";
    case FluentIconType::ADD_TO: return "AddTo";
    case FluentIconType::ACCEPT: return "Accept";
    case FluentIconType::CAMERA: return "Camera";
    case FluentIconType::CANCEL: return "Cancel";
    case FluentIconType::DELETE: return "Delete";
    case FluentIconType::FOLDER: return "Folder";
    case FluentIconType::FILTER: return "Filter";
    case FluentIconType::MARKET: return "Market";
    case FluentIconType::SCROLL: return "Scroll";
    case FluentIconType::LAYOUT: return "Layout";
    case FluentIconType::GITHUB: return "GitHub";
    case FluentIconType::UPDATE: return "Update";
    case FluentIconType::REMOVE: return "Remove";
    case FluentIconType::RETURN: return "Return";
    case FluentIconType::PEOPLE: return "People";
    case FluentIconType::QRCODE: return "QRCode";
    case FluentIconType::RINGER: return "Ringer";
    case FluentIconType::ROTATE: return "Rotate";
    case FluentIconType::SEARCH: return "Search";
    case FluentIconType::VOLUME: return "Volume";
    case FluentIconType::FRIGID : return "Frigid";
    case FluentIconType::SAVE_AS: return "SaveAs";
    case FluentIconType::ZOOM_IN: return "ZoomIn";
    case FluentIconType::CONNECT: return "Connect";
    case FluentIconType::HISTORY: return "History";
    case FluentIconType::SETTING: return "Setting";
    case FluentIconType::PALETTE: return "Palette";
    case FluentIconType::MESSAGE: return "Message";
    case FluentIconType::FIT_PAGE: return "FitPage";
    case FluentIconType::ZOOM_OUT: return "ZoomOut";
    case FluentIconType::AIRPLANE: return "Airplane";
    case FluentIconType::ASTERISK: return "Asterisk";
    case FluentIconType::CALORIES: return "Calories";
    case FluentIconType::CALENDAR: return "Calendar";
    case FluentIconType::FEEDBACK: return "Feedback";
    case FluentIconType::LIBRARY: return "BookShelf";
    case FluentIconType::MINIMIZE: return "Minimize";
    case FluentIconType::CHECKBOX: return "CheckBox";
    case FluentIconType::DOCUMENT: return "Document";
    case FluentIconType::LANGUAGE: return "Language";
    case FluentIconType::DOWNLOAD: return "Download";
    case FluentIconType::QUESTION: return "Question";
    case FluentIconType::SPEAKERS: return "Speakers";
    case FluentIconType::DATE_TIME: return "DateTime";
    case FluentIconType::FONT_SIZE: return "FontSize";
    case FluentIconType::HOME_FILL: return "HomeFill";
    case FluentIconType::PAGE_LEFT: return "PageLeft";
    case FluentIconType::SAVE_COPY: return "SaveCopy";
    case FluentIconType::SEND_FILL: return "SendFill";
    case FluentIconType::SKIP_BACK: return "SkipBack";
    case FluentIconType::SPEED_OFF: return "SpeedOff";
    case FluentIconType::ALIGNMENT: return "Alignment";
    case FluentIconType::BLUETOOTH: return "Bluetooth";
    case FluentIconType::COMPLETED: return "Completed";
    case FluentIconType::CONSTRACT: return "Constract";
    case FluentIconType::HEADPHONE: return "Headphone";
    case FluentIconType::MEGAPHONE: return "Megaphone";
    case FluentIconType::PROJECTOR: return "Projector";
    case FluentIconType::EDUCATION: return "Education";
    case FluentIconType::LEFT_ARROW: return "LeftArrow";
    case FluentIconType::ERASE_TOOL: return "EraseTool";
    case FluentIconType::PAGE_RIGHT: return "PageRight";
    case FluentIconType::PLAY_SOLID: return "PlaySolid";
    case FluentIconType::BOOK_SHELF: return "BookShelf";
    case FluentIconType::HIGHTLIGHT: return "Highlight";
    case FluentIconType::FOLDER_ADD: return "FolderAdd";
    case FluentIconType::PAUSE_BOLD: return "PauseBold";
    case FluentIconType::PENCIL_INK: return "PencilInk";
    case FluentIconType::PIE_SINGLE: return "PieSingle";
    case FluentIconType::QUICK_NOTE: return "QuickNote";
    case FluentIconType::SPEED_HIGH: return "SpeedHigh";
    case FluentIconType::STOP_WATCH: return "StopWatch";
    case FluentIconType::ZIP_FOLDER: return "ZipFolder";
    case FluentIconType::BASKETBALL: return "Basketball";
    case FluentIconType::BRIGHTNESS: return "Brightness";
    case FluentIconType::DICTIONARY: return "Dictionary";
    case FluentIconType::MICROPHONE: return "Microphone";
    case FluentIconType::ARROW_DOWN: return "ChevronDown";
    case FluentIconType::FULL_SCREEN: return "FullScreen";
    case FluentIconType::MIX_VOLUMES: return "MixVolumes";
    case FluentIconType::REMOVE_FROM: return "RemoveFrom";
    case FluentIconType::RIGHT_ARROW: return "RightArrow";
    case FluentIconType::QUIET_HOURS: return "QuietHours";
    case FluentIconType::FINGERPRINT: return "Fingerprint";
    case FluentIconType::APPLICATION: return "Application";
    case FluentIconType::CERTIFICATE: return "Certificate";
    case FluentIconType::TRANSPARENT: return "Transparent";
    case FluentIconType::IMAGE_EXPORT: return "ImageExport";
    case FluentIconType::SPEED_MEDIUM: return "SpeedMedium";
    case FluentIconType::LIBRARY_FILL: return "LibraryFill";
    case FluentIconType::MUSIC_FOLDER: return "MusicFolder";
    case FluentIconType::POWER_BUTTON: return "PowerButton";
    case FluentIconType::SKIP_FORWARD: return "SkipForward";
    case FluentIconType::CARE_UP_SOLID: return "CareUpSolid";
    case FluentIconType::ACCEPT_MEDIUM: return "AcceptMedium";
    case FluentIconType::CANCEL_MEDIUM: return "CancelMedium";
    case FluentIconType::CHEVRON_RIGHT: return "ChevronRight";
    case FluentIconType::CLIPPING_TOOL: return "ClippingTool";
    case FluentIconType::SEARCH_MIRROR: return "SearchMirror";
    case FluentIconType::SHOPPING_CART: return "ShoppingCart";
    case FluentIconType::FONT_INCREASE: return "FontIncrease";
    case FluentIconType::BACK_TO_WINDOW: return "BackToWindow";
    case FluentIconType::COMMAND_PROMPT: return "CommandPrompt";
    case FluentIconType::CLOUD_DOWNLOAD: return "CloudDownload";
    case FluentIconType::DICTIONARY_ADD: return "DictionaryAdd";
    case FluentIconType::CARE_DOWN_SOLID: return "CareDownSolid";
    case FluentIconType::CARE_LEFT_SOLID: return "CareLeftSolid";
    case FluentIconType::CLEAR_SELECTION: return "ClearSelection";
    case FluentIconType::DEVELOPER_TOOLS: return "DeveloperTools";
    case FluentIconType::BACKGROUND_FILL: return "BackgroundColor";
    case FluentIconType::CARE_RIGHT_SOLID: return "CareRightSolid";
    case FluentIconType::CHEVRON_DOWN_MED: return "ChevronDownMed";
    case FluentIconType::CHEVRON_RIGHT_MED: return "ChevronRightMed";
    case FluentIconType::EMOJI_TAB_SYMBOLS: return "EmojiTabSymbols";
    case FluentIconType::EXPRESSIVE_INPUT_ENTRY: return "ExpressiveInputEntry";
    default:
        qWarning() << "Unknown FluentIconType::IconType:" << type;
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

// ============================================================================
// Action 实现
// ============================================================================
Action::Action(QObject* parent) : QAction(parent)
{
}

Action::Action(const QString& text, QObject* parent) : QAction(text, parent)
{
}

Action::Action(const QIcon& icon, const QString& text, QObject* parent) : QAction(icon, text, parent)
{
}

Action::Action(const FluentIconType::IconType icon, const QString& text, QObject* parent)
    : QAction(text, parent)
{
    setIcon(icon);
}

Action::~Action() = default;

QIcon Action::icon() const
{
    return QAction::icon();
}

void Action::setIcon(const FluentIconType::IconType icon)
{
    QAction::setIcon(FluentIcon(icon).qicon());
}

