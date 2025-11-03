#include "FluentIcon.h"
#include <QApplication>
#include <QPalette>
#include <QPixmap>
#include <QImage>
#include <QFile>
#include <QSvgRenderer>
#include <QDomDocument>
#include <QFontDatabase>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "Theme.h"

// 初始化静态缓存
QCache<QString, QByteArray> FluentIconUtils::s_svgCache(500); // 缓存 500 个着色 SVG

QString FluentIconUtils::getIconColor(ThemeType::ThemeMode theme, bool reverse) {
    QString lc = reverse ? "white" : "black";
    QString dc = reverse ? "black" : "white";

    if (theme == ThemeType::AUTO) {
        return Theme::instance()->isDarkTheme() ? dc : lc;
    }
    return (theme == ThemeType::DARK) ? dc : lc;
}

void FluentIconUtils::drawSvgIcon(const QByteArray& icon, QPainter* painter, const QRectF& rect) {
    if (icon.isEmpty()) return;
    QSvgRenderer renderer(icon);
    renderer.render(painter, rect);
}

void FluentIconUtils::drawSvgIcon(const QString& iconPath, QPainter* painter, const QRectF& rect) {
    QFile file(iconPath);
    if (!file.open(QFile::ReadOnly)) return;
    drawSvgIcon(file.readAll(), painter, rect);
}

QString FluentIconUtils::writeSvg(const QString& iconPath, const QList<int>& indexes, const QMap<QString, QString>& attributes) {
    if (!QFile::exists(iconPath) || !iconPath.endsWith(".svg", Qt::CaseInsensitive)) {
        return QString();
    }

    // 生成缓存键
    QString key = iconPath;
    for (auto it = attributes.begin(); it != attributes.end(); ++it)
        key += "_" + it.key() + it.value();
    for (int i : indexes) key += "_" + QString::number(i);

    if (s_svgCache.contains(key)) {
        return QString(*s_svgCache[key]);
    }

    QFile file(iconPath);
    if (!file.open(QFile::ReadOnly)) return QString();

    QDomDocument dom;
    if (!dom.setContent(&file)) {
        file.close();
        return QString();
    }
    file.close();

    QDomNodeList pathNodes = dom.elementsByTagName("path");
    QList<int> targetIndexes = indexes.isEmpty() ?
        [] (const QDomNodeList& nodes) {
            QList<int> list;
            for (int i = 0; i < nodes.length(); ++i) list.append(i);
            return list;
        }(pathNodes) : indexes;

    for (int i : targetIndexes) {
        if (i >= pathNodes.length()) continue;
        QDomElement element = pathNodes.at(i).toElement();
        for (auto it = attributes.begin(); it != attributes.end(); ++it) {
            element.setAttribute(it.key(), it.value());
        }
    }

    QString result = dom.toString();
    s_svgCache.insert(key, new QByteArray(result.toUtf8()), result.size());
    return result;
}

QIcon FluentIconUtils::toQIcon(const QVariant& icon) {
    if (icon.canConvert<QString>()) return QIcon(icon.toString());
    if (icon.canConvert<FluentIconBase*>()) {
        auto* fi = icon.value<FluentIconBase*>();
        return fi ? fi->icon() : QIcon();
    }
    if (icon.canConvert<QIcon>()) return icon.value<QIcon>();
    return QIcon();
}

void FluentIconUtils::drawIcon(const FluentIconBase& icon, QPainter* painter, const QRectF& rect, QIcon::State state, const QMap<QString, QString>& attributes) {
    Q_UNUSED(state)
    icon.render(painter, rect, ThemeType::AUTO, {}, attributes);
}

// ====================== FluentIconEngine ======================

FluentIconEngine::FluentIconEngine(FluentIconType::IconType iconType, bool reverse)
    : m_iconType(iconType), m_isThemeReversed(reverse) {
    if (m_iconType != FluentIconType::CUSTOM_PATH) {
        m_iconBase = std::make_unique<FluentIcon>(m_iconType);
    }
}

FluentIconEngine::FluentIconEngine(const QString& templatePath, bool reverse)
    : m_iconType(FluentIconType::CUSTOM_PATH), m_templatePath(templatePath), m_isThemeReversed(reverse) {
    m_iconBase = std::make_unique<FluentIcon>(m_templatePath);
}

void FluentIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive() || rect.isEmpty()) return;
    painter->save();

    if (mode == QIcon::Disabled) painter->setOpacity(0.5);
    else if (mode == QIcon::Selected) painter->setOpacity(0.7);

    ThemeType::ThemeMode theme = m_isThemeReversed
        ? (Theme::instance()->isDarkTheme() ? ThemeType::LIGHT : ThemeType::DARK)
        : ThemeType::AUTO;

    if (m_iconBase) {
        m_iconBase->render(painter, rect, theme);
    }

    painter->restore();
}

QIconEngine* FluentIconEngine::clone() const {
    auto* engine = (m_iconType == FluentIconType::CUSTOM_PATH)
        ? new FluentIconEngine(m_templatePath, m_isThemeReversed)
        : new FluentIconEngine(m_iconType, m_isThemeReversed);
    if (m_iconBase) {
        engine->m_iconBase = std::unique_ptr<FluentIconBase>(m_iconBase->clone());
    }
    return engine;
}

QPixmap FluentIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage img(size, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPixmap pix = QPixmap::fromImage(img);
    QPainter p(&pix);
    paint(&p, QRect(0, 0, size.width(), size.height()), mode, state);
    return pix;
}

// ====================== SvgIconEngine ======================

SvgIconEngine::SvgIconEngine(const QByteArray& svgData) : m_svgData(svgData) {}

void SvgIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode); Q_UNUSED(state);
    FluentIconUtils::drawSvgIcon(m_svgData, painter, rect);
}

QIconEngine* SvgIconEngine::clone() const { return new SvgIconEngine(m_svgData); }

QPixmap SvgIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage img(size, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPixmap pix = QPixmap::fromImage(img);
    QPainter p(&pix);
    paint(&p, QRect(0, 0, size.width(), size.height()), mode, state);
    return pix;
}

// ====================== FontIconEngine ======================

FontIconEngine::FontIconEngine(const QString& fontFamily, const QChar& ch, const QColor& color, bool isBold)
    : m_fontFamily(fontFamily), m_char(ch), m_color(color), m_isBold(isBold) {}

void FontIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode); Q_UNUSED(state);
    QFont font(m_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(rect.height() * 0.8);
    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QRectF textRect = rect;
    textRect.moveTop((rect.height() - painter->fontMetrics().height()) / 2.0);
    painter->drawText(textRect, Qt::AlignCenter, QString(m_char));
}

QIconEngine* FontIconEngine::clone() const { return new FontIconEngine(m_fontFamily, m_char, m_color, m_isBold); }

QPixmap FontIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage img(size, QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPixmap pix = QPixmap::fromImage(img);
    QPainter p(&pix);
    paint(&p, QRect(0, 0, size.width(), size.height()), mode, state);
    return pix;
}

// ====================== FluentIconBase ======================

QIcon FluentIconBase::icon(ThemeType::ThemeMode theme, const QColor& color) const {
    QString p = path(theme);
    if (!p.endsWith(".svg") || !color.isValid()) return QIcon(p);

    QMap<QString, QString> attrs{{"fill", color.name()}};
    QString svg = FluentIconUtils::writeSvg(p, {}, attrs);
    return QIcon(new SvgIconEngine(svg.toUtf8()));
}

ColoredFluentIcon FluentIconBase::colored(const QColor& lightColor, const QColor& darkColor) const {
    return ColoredFluentIcon(*this, lightColor, darkColor);
}

void FluentIconBase::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                            const QList<int>& indexes, const QMap<QString, QString>& attributes) const {
    QString p = path(theme);
    if (p.endsWith(".svg")) {
        QByteArray data;
        if (!attributes.isEmpty()) {
            data = FluentIconUtils::writeSvg(p, indexes, attributes).toUtf8();
        } else {
            QFile f(p);
            if (f.open(QFile::ReadOnly)) { data = f.readAll(); f.close(); }
        }
        if (!data.isEmpty()) FluentIconUtils::drawSvgIcon(data, painter, rect);
    } else {
        QIcon(p).paint(painter, rect.toRect(), Qt::AlignCenter);
    }
}

// ====================== FluentFontIconBase ======================

bool FluentFontIconBase::s_isFontLoaded = false;
int FluentFontIconBase::s_fontId = -1;
QString FluentFontIconBase::s_fontFamily;
QMap<QString, QChar> FluentFontIconBase::s_iconNames;

FluentFontIconBase::FluentFontIconBase(const QChar& ch)
    : m_char(ch), m_lightColor(Qt::black), m_darkColor(Qt::white), m_isBold(false) {
    loadFont();
}

QString FluentFontIconBase::path(ThemeType::ThemeMode theme) const { Q_UNUSED(theme); return fontPath(); }

FluentFontIconBase FluentFontIconBase::fromName(const QString& name) {
    return FluentFontIconBase(s_iconNames.value(name, QChar()));
}

FluentFontIconBase& FluentFontIconBase::bold() { m_isBold = true; return *this; }

QIcon FluentFontIconBase::icon(ThemeType::ThemeMode theme, const QColor& color) const {
    QColor c = color.isValid() ? color : getIconColor(theme);
    return QIcon(new FontIconEngine(s_fontFamily, m_char, c, m_isBold));
}

ColoredFluentIcon FluentFontIconBase::colored(const QColor& lightColor, const QColor& darkColor) const {
    auto* self = const_cast<FluentFontIconBase*>(this);
    self->m_lightColor = lightColor;
    self->m_darkColor = darkColor;
    return ColoredFluentIcon(*this, lightColor, darkColor);
}

void FluentFontIconBase::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                                const QList<int>& indexes, const QMap<QString, QString>& attributes) const {
    Q_UNUSED(indexes);
    QColor color = attributes.contains("fill") ? QColor(attributes["fill"]) : getIconColor(theme);
    QFont font(s_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(qRound(rect.height() * 0.8));
    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->drawText(rect, Qt::AlignCenter, QString(m_char));
}

void FluentFontIconBase::loadFont() {
    if (s_isFontLoaded || !QApplication::instance()) return;
    QString fp = fontPath();
    if (fp.isEmpty() || !QFile::exists(fp)) return;

    QFile file(fp);
    if (!file.open(QFile::ReadOnly)) return;
    QByteArray data = file.readAll(); file.close();

    s_fontId = QFontDatabase::addApplicationFontFromData(data);
    if (s_fontId != -1) {
        auto families = QFontDatabase::applicationFontFamilies(s_fontId);
        if (!families.isEmpty()) {
            s_fontFamily = families.first();
            s_isFontLoaded = true;
        }
    }
    if (!iconNameMapPath().isEmpty()) loadIconNames();
}

void FluentFontIconBase::loadIconNames() {
    s_iconNames.clear();
    QString path = iconNameMapPath();
    if (!QFile::exists(path)) return;
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (doc.isObject()) {
        for (auto it = doc.object().begin(); it != doc.object().end(); ++it) {
            s_iconNames[it.key()] = QChar(it.value().toString()[0]);
        }
    }
}

QColor FluentFontIconBase::getIconColor(ThemeType::ThemeMode theme) const {
    return (theme == ThemeType::AUTO)
        ? (Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor)
        : (theme == ThemeType::DARK ? m_darkColor : m_lightColor);
}

// ====================== ColoredFluentIcon ======================

ColoredFluentIcon::ColoredFluentIcon(const FluentIconBase& icon, const QColor& lightColor, const QColor& darkColor)
    : m_fluentIcon(icon.clone()), m_lightColor(lightColor), m_darkColor(darkColor) {}

QString ColoredFluentIcon::path(ThemeType::ThemeMode theme) const {
    return m_fluentIcon ? m_fluentIcon->path(theme) : QString();
}

void ColoredFluentIcon::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                               const QList<int>& indexes, const QMap<QString, QString>& attributes) const {
    if (!m_fluentIcon) return;
    QString p = path(theme);
    if (!p.endsWith(".svg")) { m_fluentIcon->render(painter, rect, theme, indexes, attributes); return; }

    QColor color = (theme == ThemeType::AUTO)
        ? (Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor)
        : (theme == ThemeType::DARK ? m_darkColor : m_lightColor);

    QMap<QString, QString> attrs = attributes;
    attrs["fill"] = color.name();
    QByteArray data = FluentIconUtils::writeSvg(p, indexes, attrs).toUtf8();
    FluentIconUtils::drawSvgIcon(data, painter, rect);
}

// ====================== FluentIcon ======================

FluentIcon::FluentIcon(FluentIconType::IconType iconEnum)
    : m_iconEnum(iconEnum), m_cachedLightPath(), m_cachedDarkPath() {}

FluentIcon::FluentIcon(const QString& templatePath)
    : m_iconEnum(FluentIconType::CUSTOM_PATH), m_templatePath(templatePath) {}

QString FluentIcon::path(ThemeType::ThemeMode theme) const {
    QString color = FluentIconUtils::getIconColor(theme);
    QString cache = (color == "black") ? m_cachedLightPath : m_cachedDarkPath;
    if (!cache.isEmpty()) {
        return cache;
    }
    if (m_iconEnum != FluentIconType::CUSTOM_PATH) {
        cache = QStringLiteral(":/res/images/icons/%1_%2.svg")
                    .arg(enumToString(m_iconEnum), color);
    } else {
        // 修复：在const成员函数中，不能直接调用replace，所以用临时字符串
        QString temp = m_templatePath;
        cache = temp.replace("{color}", color);
    }
    return cache;
}

QIcon FluentIcon::qicon(bool reverse) const {
    return (m_iconEnum == FluentIconType::CUSTOM_PATH)
        ? QIcon(new FluentIconEngine(m_templatePath, reverse))
        : QIcon(new FluentIconEngine(m_iconEnum, reverse));
}

QString FluentIcon::enumToString(FluentIconType::IconType e) const {
    return fluentIcons().value(e, "");
}

const QMap<FluentIconType::IconType, QString>& FluentIcon::fluentIcons() {
    static const QMap<FluentIconType::IconType, QString> map = {
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

// ====================== Icon & Action ======================

Icon::Icon(const FluentIcon& fluentIcon)
    : QIcon(fluentIcon.path()), m_fluentIcon(fluentIcon) {}

QIcon toQIcon(const QVariant& icon) { return FluentIconUtils::toQIcon(icon); }

Action::Action(QObject* parent) : QAction(parent), m_fluentIcon(nullptr) {}
Action::Action(const QString& text, QObject* parent) : QAction(text, parent), m_fluentIcon(nullptr) {}
Action::Action(const QIcon& icon, const QString& text, QObject* parent) : QAction(icon, text, parent), m_fluentIcon(nullptr) {}
Action::Action(const FluentIconBase& icon, const QString& text, QObject* parent)
    : QAction(icon.icon(), text, parent), m_fluentIcon(icon.clone()) {}

QIcon Action::icon() const {
    return m_fluentIcon ? m_fluentIcon->icon() : QAction::icon();
}

void Action::setFluentIcon(const FluentIconBase& icon) {
    m_fluentIcon.reset(icon.clone());
    QAction::setIcon(m_fluentIcon->icon());
}
