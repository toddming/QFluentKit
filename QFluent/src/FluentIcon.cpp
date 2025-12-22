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

QString FluentIconUtils::getIconColor(Fluent::ThemeMode theme, bool reverse) {
    QString lc = reverse ? "white" : "black";
    QString dc = reverse ? "black" : "white";

    if (theme == Fluent::ThemeMode::AUTO) {
        return Theme::instance()->isDarkTheme() ? dc : lc;
    }
    return (theme == Fluent::ThemeMode::DARK) ? "black" : "white";
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

// 新增: 公共的枚举映射功能
const QMap<Fluent::IconType, QString>& FluentIconUtils::fluentIcons() {
    static const QMap<Fluent::IconType, QString> map = {
        {Fluent::IconType::UP, QStringLiteral("Up")},
        {Fluent::IconType::ADD, QStringLiteral("Add")},
        {Fluent::IconType::BUS, QStringLiteral("Bus")},
        {Fluent::IconType::CAR, QStringLiteral("Car")},
        {Fluent::IconType::CUT, QStringLiteral("Cut")},
        {Fluent::IconType::IOT, QStringLiteral("IOT")},
        {Fluent::IconType::PIN, QStringLiteral("Pin")},
        {Fluent::IconType::TAG, QStringLiteral("Tag")},
        {Fluent::IconType::VPN, QStringLiteral("VPN")},
        {Fluent::IconType::CAFE, QStringLiteral("Cafe")},
        {Fluent::IconType::CHAT, QStringLiteral("Chat")},
        {Fluent::IconType::COPY, QStringLiteral("Copy")},
        {Fluent::IconType::CODE, QStringLiteral("Code")},
        {Fluent::IconType::DOWN, QStringLiteral("Down")},
        {Fluent::IconType::EDIT, QStringLiteral("Edit")},
        {Fluent::IconType::FLAG, QStringLiteral("Flag")},
        {Fluent::IconType::FONT, QStringLiteral("Font")},
        {Fluent::IconType::GAME, QStringLiteral("Game")},
        {Fluent::IconType::HELP, QStringLiteral("Help")},
        {Fluent::IconType::HIDE, QStringLiteral("Hide")},
        {Fluent::IconType::HOME, QStringLiteral("Home")},
        {Fluent::IconType::INFO, QStringLiteral("Info")},
        {Fluent::IconType::LEAF, QStringLiteral("Leaf")},
        {Fluent::IconType::LINK, QStringLiteral("Link")},
        {Fluent::IconType::MAIL, QStringLiteral("Mail")},
        {Fluent::IconType::MENU, QStringLiteral("Menu")},
        {Fluent::IconType::MUTE, QStringLiteral("Mute")},
        {Fluent::IconType::MORE, QStringLiteral("More")},
        {Fluent::IconType::MOVE, QStringLiteral("Move")},
        {Fluent::IconType::PLAY, QStringLiteral("Play")},
        {Fluent::IconType::SAVE, QStringLiteral("Save")},
        {Fluent::IconType::SEND, QStringLiteral("Send")},
        {Fluent::IconType::SYNC, QStringLiteral("Sync")},
        {Fluent::IconType::UNIT, QStringLiteral("Unit")},
        {Fluent::IconType::VIEW, QStringLiteral("View")},
        {Fluent::IconType::WIFI, QStringLiteral("Wifi")},
        {Fluent::IconType::ZOOM, QStringLiteral("Zoom")},
        {Fluent::IconType::ALBUM, QStringLiteral("Album")},
        {Fluent::IconType::BRUSH, QStringLiteral("Brush")},
        {Fluent::IconType::BROOM, QStringLiteral("Broom")},
        {Fluent::IconType::CLOSE, QStringLiteral("Close")},
        {Fluent::IconType::CLOUD, QStringLiteral("Cloud")},
        {Fluent::IconType::EMBED, QStringLiteral("Embed")},
        {Fluent::IconType::GLOBE, QStringLiteral("Globe")},
        {Fluent::IconType::HEART, QStringLiteral("Heart")},
        {Fluent::IconType::LABEL, QStringLiteral("Label")},
        {Fluent::IconType::MEDIA, QStringLiteral("Media")},
        {Fluent::IconType::MOVIE, QStringLiteral("Movie")},
        {Fluent::IconType::MUSIC, QStringLiteral("Music")},
        {Fluent::IconType::ROBOT, QStringLiteral("Robot")},
        {Fluent::IconType::PAUSE, QStringLiteral("Pause")},
        {Fluent::IconType::PASTE, QStringLiteral("Paste")},
        {Fluent::IconType::PHOTO, QStringLiteral("Photo")},
        {Fluent::IconType::PHONE, QStringLiteral("Phone")},
        {Fluent::IconType::PRINT, QStringLiteral("Print")},
        {Fluent::IconType::SHARE, QStringLiteral("Share")},
        {Fluent::IconType::TILES, QStringLiteral("Tiles")},
        {Fluent::IconType::UNPIN, QStringLiteral("Unpin")},
        {Fluent::IconType::VIDEO, QStringLiteral("Video")},
        {Fluent::IconType::TRAIN, QStringLiteral("Train")},
        {Fluent::IconType::ADD_TO, QStringLiteral("AddTo")},
        {Fluent::IconType::ACCEPT, QStringLiteral("Accept")},
        {Fluent::IconType::CAMERA, QStringLiteral("Camera")},
        {Fluent::IconType::CANCEL, QStringLiteral("Cancel")},
        {Fluent::IconType::DELETE, QStringLiteral("Delete")},
        {Fluent::IconType::FOLDER, QStringLiteral("Folder")},
        {Fluent::IconType::FILTER, QStringLiteral("Filter")},
        {Fluent::IconType::MARKET, QStringLiteral("Market")},
        {Fluent::IconType::SCROLL, QStringLiteral("Scroll")},
        {Fluent::IconType::LAYOUT, QStringLiteral("Layout")},
        {Fluent::IconType::GITHUB, QStringLiteral("GitHub")},
        {Fluent::IconType::UPDATE, QStringLiteral("Update")},
        {Fluent::IconType::REMOVE, QStringLiteral("Remove")},
        {Fluent::IconType::RETURN, QStringLiteral("Return")},
        {Fluent::IconType::PEOPLE, QStringLiteral("People")},
        {Fluent::IconType::QRCODE, QStringLiteral("QRCode")},
        {Fluent::IconType::RINGER, QStringLiteral("Ringer")},
        {Fluent::IconType::ROTATE, QStringLiteral("Rotate")},
        {Fluent::IconType::SEARCH, QStringLiteral("Search")},
        {Fluent::IconType::VOLUME, QStringLiteral("Volume")},
        {Fluent::IconType::FRIGID, QStringLiteral("Frigid")},
        {Fluent::IconType::SAVE_AS, QStringLiteral("SaveAs")},
        {Fluent::IconType::ZOOM_IN, QStringLiteral("ZoomIn")},
        {Fluent::IconType::CONNECT, QStringLiteral("Connect")},
        {Fluent::IconType::HISTORY, QStringLiteral("History")},
        {Fluent::IconType::SETTING, QStringLiteral("Setting")},
        {Fluent::IconType::PALETTE, QStringLiteral("Palette")},
        {Fluent::IconType::MESSAGE, QStringLiteral("Message")},
        {Fluent::IconType::FIT_PAGE, QStringLiteral("FitPage")},
        {Fluent::IconType::ZOOM_OUT, QStringLiteral("ZoomOut")},
        {Fluent::IconType::AIRPLANE, QStringLiteral("Airplane")},
        {Fluent::IconType::ASTERISK, QStringLiteral("Asterisk")},
        {Fluent::IconType::CALORIES, QStringLiteral("Calories")},
        {Fluent::IconType::CALENDAR, QStringLiteral("Calendar")},
        {Fluent::IconType::FEEDBACK, QStringLiteral("Feedback")},
        {Fluent::IconType::LIBRARY, QStringLiteral("BookShelf")},
        {Fluent::IconType::MINIMIZE, QStringLiteral("Minimize")},
        {Fluent::IconType::CHECKBOX, QStringLiteral("CheckBox")},
        {Fluent::IconType::DOCUMENT, QStringLiteral("Document")},
        {Fluent::IconType::LANGUAGE, QStringLiteral("Language")},
        {Fluent::IconType::DOWNLOAD, QStringLiteral("Download")},
        {Fluent::IconType::QUESTION, QStringLiteral("Question")},
        {Fluent::IconType::SPEAKERS, QStringLiteral("Speakers")},
        {Fluent::IconType::DATE_TIME, QStringLiteral("DateTime")},
        {Fluent::IconType::FONT_SIZE, QStringLiteral("FontSize")},
        {Fluent::IconType::HOME_FILL, QStringLiteral("HomeFill")},
        {Fluent::IconType::PAGE_LEFT, QStringLiteral("PageLeft")},
        {Fluent::IconType::SAVE_COPY, QStringLiteral("SaveCopy")},
        {Fluent::IconType::SEND_FILL, QStringLiteral("SendFill")},
        {Fluent::IconType::SKIP_BACK, QStringLiteral("SkipBack")},
        {Fluent::IconType::SPEED_OFF, QStringLiteral("SpeedOff")},
        {Fluent::IconType::ALIGNMENT, QStringLiteral("Alignment")},
        {Fluent::IconType::BLUETOOTH, QStringLiteral("Bluetooth")},
        {Fluent::IconType::COMPLETED, QStringLiteral("Completed")},
        {Fluent::IconType::CONSTRACT, QStringLiteral("Constract")},
        {Fluent::IconType::HEADPHONE, QStringLiteral("Headphone")},
        {Fluent::IconType::MEGAPHONE, QStringLiteral("Megaphone")},
        {Fluent::IconType::PROJECTOR, QStringLiteral("Projector")},
        {Fluent::IconType::EDUCATION, QStringLiteral("Education")},
        {Fluent::IconType::LEFT_ARROW, QStringLiteral("LeftArrow")},
        {Fluent::IconType::ERASE_TOOL, QStringLiteral("EraseTool")},
        {Fluent::IconType::PAGE_RIGHT, QStringLiteral("PageRight")},
        {Fluent::IconType::PLAY_SOLID, QStringLiteral("PlaySolid")},
        {Fluent::IconType::BOOK_SHELF, QStringLiteral("BookShelf")},
        {Fluent::IconType::HIGHTLIGHT, QStringLiteral("Highlight")},
        {Fluent::IconType::FOLDER_ADD, QStringLiteral("FolderAdd")},
        {Fluent::IconType::PAUSE_BOLD, QStringLiteral("PauseBold")},
        {Fluent::IconType::PENCIL_INK, QStringLiteral("PencilInk")},
        {Fluent::IconType::PIE_SINGLE, QStringLiteral("PieSingle")},
        {Fluent::IconType::QUICK_NOTE, QStringLiteral("QuickNote")},
        {Fluent::IconType::SPEED_HIGH, QStringLiteral("SpeedHigh")},
        {Fluent::IconType::STOP_WATCH, QStringLiteral("StopWatch")},
        {Fluent::IconType::ZIP_FOLDER, QStringLiteral("ZipFolder")},
        {Fluent::IconType::BASKETBALL, QStringLiteral("Basketball")},
        {Fluent::IconType::BRIGHTNESS, QStringLiteral("Brightness")},
        {Fluent::IconType::DICTIONARY, QStringLiteral("Dictionary")},
        {Fluent::IconType::MICROPHONE, QStringLiteral("Microphone")},
        {Fluent::IconType::ARROW_DOWN, QStringLiteral("ChevronDown")},
        {Fluent::IconType::FULL_SCREEN, QStringLiteral("FullScreen")},
        {Fluent::IconType::MIX_VOLUMES, QStringLiteral("MixVolumes")},
        {Fluent::IconType::REMOVE_FROM, QStringLiteral("RemoveFrom")},
        {Fluent::IconType::RIGHT_ARROW, QStringLiteral("RightArrow")},
        {Fluent::IconType::QUIET_HOURS, QStringLiteral("QuietHours")},
        {Fluent::IconType::FINGERPRINT, QStringLiteral("Fingerprint")},
        {Fluent::IconType::APPLICATION, QStringLiteral("Application")},
        {Fluent::IconType::CERTIFICATE, QStringLiteral("Certificate")},
        {Fluent::IconType::TRANSPARENT, QStringLiteral("Transparent")},
        {Fluent::IconType::IMAGE_EXPORT, QStringLiteral("ImageExport")},
        {Fluent::IconType::SPEED_MEDIUM, QStringLiteral("SpeedMedium")},
        {Fluent::IconType::LIBRARY_FILL, QStringLiteral("LibraryFill")},
        {Fluent::IconType::MUSIC_FOLDER, QStringLiteral("MusicFolder")},
        {Fluent::IconType::POWER_BUTTON, QStringLiteral("PowerButton")},
        {Fluent::IconType::SKIP_FORWARD, QStringLiteral("SkipForward")},
        {Fluent::IconType::CARE_UP_SOLID, QStringLiteral("CareUpSolid")},
        {Fluent::IconType::ACCEPT_MEDIUM, QStringLiteral("AcceptMedium")},
        {Fluent::IconType::CANCEL_MEDIUM, QStringLiteral("CancelMedium")},
        {Fluent::IconType::CHEVRON_RIGHT, QStringLiteral("ChevronRight")},
        {Fluent::IconType::CLIPPING_TOOL, QStringLiteral("ClippingTool")},
        {Fluent::IconType::SEARCH_MIRROR, QStringLiteral("SearchMirror")},
        {Fluent::IconType::SHOPPING_CART, QStringLiteral("ShoppingCart")},
        {Fluent::IconType::FONT_INCREASE, QStringLiteral("FontIncrease")},
        {Fluent::IconType::BACK_TO_WINDOW, QStringLiteral("BackToWindow")},
        {Fluent::IconType::COMMAND_PROMPT, QStringLiteral("CommandPrompt")},
        {Fluent::IconType::CLOUD_DOWNLOAD, QStringLiteral("CloudDownload")},
        {Fluent::IconType::DICTIONARY_ADD, QStringLiteral("DictionaryAdd")},
        {Fluent::IconType::CARE_DOWN_SOLID, QStringLiteral("CareDownSolid")},
        {Fluent::IconType::CARE_LEFT_SOLID, QStringLiteral("CareLeftSolid")},
        {Fluent::IconType::CLEAR_SELECTION, QStringLiteral("ClearSelection")},
        {Fluent::IconType::DEVELOPER_TOOLS, QStringLiteral("DeveloperTools")},
        {Fluent::IconType::BACKGROUND_FILL, QStringLiteral("BackgroundColor")},
        {Fluent::IconType::CARE_RIGHT_SOLID, QStringLiteral("CareRightSolid")},
        {Fluent::IconType::CHEVRON_DOWN_MED, QStringLiteral("ChevronDownMed")},
        {Fluent::IconType::CHEVRON_RIGHT_MED, QStringLiteral("ChevronRightMed")},
        {Fluent::IconType::EMOJI_TAB_SYMBOLS, QStringLiteral("EmojiTabSymbols")},
        {Fluent::IconType::EXPRESSIVE_INPUT_ENTRY, QStringLiteral("ExpressiveInputEntry")}
    };
    return map;
}

QString FluentIconUtils::enumToString(Fluent::IconType e) {
    return fluentIcons().value(e, "");
}

QIcon FluentIconUtils::toQIcon(const QVariant& icon) {
    // if (icon.canConvert<QString>()) return QIcon(icon.toString());
    // if (icon.canConvert<FluentIconBase*>()) {
    //     auto* fi = icon.value<FluentIconBase*>();
    //     return fi ? fi->icon() : QIcon();
    // }
    // if (icon.canConvert<QIcon>()) return icon.value<QIcon>();
    return QIcon();
}

void FluentIconUtils::drawIcon(const FluentIconBase& icon, QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme,
                               QIcon::State state, const QMap<QString, QString>& attributes) {
    Q_UNUSED(state)
    icon.render(painter, rect, theme, {}, attributes);
}

// ====================== FluentIconEngine ======================

FluentIconEngine::FluentIconEngine(Fluent::IconType iconType, bool reverse)
    : m_iconType(iconType), m_isThemeReversed(reverse) {
    if (m_iconType != Fluent::IconType::CUSTOM_PATH) {
        m_iconBase = std::make_unique<FluentIcon>(m_iconType);
    }
}

FluentIconEngine::FluentIconEngine(const QString& templatePath, bool reverse)
    : m_iconType(Fluent::IconType::CUSTOM_PATH), m_templatePath(templatePath), m_isThemeReversed(reverse) {
    m_iconBase = std::make_unique<FluentIcon>(m_templatePath);
}

void FluentIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive() || rect.isEmpty()) return;
    painter->save();

    if (mode == QIcon::Disabled) painter->setOpacity(0.5);
    else if (mode == QIcon::Selected) painter->setOpacity(0.7);

    Fluent::ThemeMode theme = m_isThemeReversed
            ? (Theme::instance()->isDarkTheme() ? Fluent::ThemeMode::LIGHT : Fluent::ThemeMode::DARK)
            : Fluent::ThemeMode::AUTO;

    if (m_iconBase) {
        m_iconBase->render(painter, rect, theme);
    }

    painter->restore();
}

QIconEngine* FluentIconEngine::clone() const {
    auto* engine = (m_iconType == Fluent::IconType::CUSTOM_PATH)
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

QIcon FluentIconBase::icon(Fluent::ThemeMode theme, const QColor& color) const {
    QString p = path(theme);
    if (!p.endsWith(".svg") || !color.isValid()) return QIcon(p);

    QMap<QString, QString> attrs{{"fill", color.name()}};
    QString svg = FluentIconUtils::writeSvg(p, {}, attrs);
    return QIcon(new SvgIconEngine(svg.toUtf8()));
}

ColoredFluentIcon FluentIconBase::colored(const QColor& lightColor, const QColor& darkColor) const {
    return ColoredFluentIcon(*this, lightColor, darkColor);
}

void FluentIconBase::render(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme,
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

QString FluentFontIconBase::path(Fluent::ThemeMode theme) const { Q_UNUSED(theme); return fontPath(); }

FluentFontIconBase FluentFontIconBase::fromName(const QString& name) {
    return FluentFontIconBase(s_iconNames.value(name, QChar()));
}

FluentFontIconBase& FluentFontIconBase::bold() { m_isBold = true; return *this; }

QIcon FluentFontIconBase::icon(Fluent::ThemeMode theme, const QColor& color) const {
    QColor c = color.isValid() ? color : getIconColor(theme);
    return QIcon(new FontIconEngine(s_fontFamily, m_char, c, m_isBold));
}

ColoredFluentIcon FluentFontIconBase::colored(const QColor& lightColor, const QColor& darkColor) const {
    auto* self = const_cast<FluentFontIconBase*>(this);
    self->m_lightColor = lightColor;
    self->m_darkColor = darkColor;
    return ColoredFluentIcon(*this, lightColor, darkColor);
}

void FluentFontIconBase::render(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme,
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

QColor FluentFontIconBase::getIconColor(Fluent::ThemeMode theme) const {
    return (theme == Fluent::ThemeMode::AUTO)
            ? (Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor)
            : (theme == Fluent::ThemeMode::DARK ? m_darkColor : m_lightColor);
}

// ====================== ColoredFluentIcon ======================

ColoredFluentIcon::ColoredFluentIcon(const FluentIconBase& icon, const QColor& lightColor, const QColor& darkColor)
    : m_iconEnum(Fluent::IconType::CUSTOM_PATH), m_lightColor(lightColor), m_darkColor(darkColor) {
    // 由于无法直接获取icon的类型,这里简单存储为CUSTOM_PATH
    // 实际应用中可能需要更复杂的类型识别机制
}

ColoredFluentIcon::ColoredFluentIcon(Fluent::IconType iconEnum, const QColor& lightColor, const QColor& darkColor)
    : m_iconEnum(iconEnum), m_lightColor(lightColor), m_darkColor(darkColor) {}

ColoredFluentIcon::ColoredFluentIcon(const QString& templatePath, const QColor& lightColor, const QColor& darkColor)
    : m_iconEnum(Fluent::IconType::CUSTOM_PATH), m_templatePath(templatePath),
      m_lightColor(lightColor), m_darkColor(darkColor) {}

QString ColoredFluentIcon::path(Fluent::ThemeMode theme) const {
    QString color = FluentIconUtils::getIconColor(theme);
    QString& cache = (color == "black") ? m_cachedLightPath : m_cachedDarkPath;

    if (!cache.isEmpty()) {
        return cache;
    }

    if (m_iconEnum != Fluent::IconType::CUSTOM_PATH) {
        cache = QStringLiteral(":/res/images/icons/%1_%2.svg")
                .arg(FluentIconUtils::enumToString(m_iconEnum), color);
    } else {
        QString temp = m_templatePath;
        cache = temp.replace("{color}", color);
    }
    return cache;
}

void ColoredFluentIcon::render(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme,
                               const QList<int>& indexes, const QMap<QString, QString>& attributes) const {
    QString p = path(theme);
    if (!p.endsWith(".svg")) {
        QIcon(p).paint(painter, rect.toRect(), Qt::AlignCenter);
        return;
    }

    QColor color = (theme == Fluent::ThemeMode::AUTO)
            ? (Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor)
            : (theme == Fluent::ThemeMode::DARK ? m_darkColor : m_lightColor);

    QMap<QString, QString> attrs = attributes;
    attrs["fill"] = color.name();
    QByteArray data = FluentIconUtils::writeSvg(p, indexes, attrs).toUtf8();
    FluentIconUtils::drawSvgIcon(data, painter, rect);
}

// ====================== FluentIcon ======================

FluentIcon::FluentIcon(Fluent::IconType iconEnum)
    : m_iconEnum(iconEnum), m_cachedLightPath(), m_cachedDarkPath() {}

FluentIcon::FluentIcon(const QString& templatePath)
    : m_iconEnum(Fluent::IconType::CUSTOM_PATH), m_templatePath(templatePath) {}

QString FluentIcon::path(Fluent::ThemeMode theme) const {
    QString color = FluentIconUtils::getIconColor(theme);
    QString& cache = (color == "black") ? m_cachedLightPath : m_cachedDarkPath;

    if (!cache.isEmpty()) {
        return cache;
    }

    if (m_iconEnum != Fluent::IconType::CUSTOM_PATH) {
        cache = QStringLiteral(":/res/images/icons/%1_%2.svg")
                .arg(FluentIconUtils::enumToString(m_iconEnum), color);
    } else {
        QString temp = m_templatePath;
        cache = temp.replace("{color}", color);
    }
    return cache;
}

QIcon FluentIcon::qicon(bool reverse) const {
    return (m_iconEnum == Fluent::IconType::CUSTOM_PATH)
            ? QIcon(new FluentIconEngine(m_templatePath, reverse))
            : QIcon(new FluentIconEngine(m_iconEnum, reverse));
}

// ====================== Icon & Action ======================

Icon::Icon(const FluentIcon& fluentIcon)
    : QIcon(fluentIcon.path()), m_fluentIcon(fluentIcon) {}

QIcon toQIcon(const QVariant& icon) { return FluentIconUtils::toQIcon(icon); }

Action::Action(QObject* parent) : QAction(parent), m_fluentIcon(nullptr) {}

Action::Action(const QString& text, QObject* parent) : QAction(text, parent), m_fluentIcon(nullptr) {}

Action::Action(const QIcon& icon, const QString& text, QObject* parent)
    : QAction(icon, text, parent), m_fluentIcon(nullptr) {}

Action::Action(const FluentIconBase& icon, const QString& text, QObject* parent)
    : QAction(icon.icon(), text, parent), m_fluentIcon(icon.clone()) {}

QIcon Action::icon() const {
    return m_fluentIcon ? m_fluentIcon->icon() : QAction::icon();
}

void Action::setFluentIcon(const FluentIconBase& icon) {
    m_fluentIcon.reset(icon.clone());
    QAction::setIcon(m_fluentIcon->icon());
}
