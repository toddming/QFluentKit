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
#include <QPainter>
#include <cmath>
#include <memory>

#include "Theme.h"

// Initialize static cache
QCache<QString, QByteArray> FluentIconUtils::s_svgCache(500);
QMutex FluentIconUtils::s_svgCacheMutex;

QString FluentIconUtils::iconColor(Fluent::ThemeMode theme, bool reverse)
{
    const QString lightColor = reverse ? QStringLiteral("white") : QStringLiteral("black");
    const QString darkColor = reverse ? QStringLiteral("black") : QStringLiteral("white");

    if (theme == Fluent::ThemeMode::AUTO) {
        return Theme::instance()->isDarkTheme() ? darkColor : lightColor;
    }
    return (theme == Fluent::ThemeMode::DARK) ? darkColor : lightColor;
}

void FluentIconUtils::drawSvgIcon(const QByteArray& iconData, QPainter* painter, const QRectF& rect)
{
    if (!painter || iconData.isEmpty() || rect.isEmpty()) {
        return;
    }

    QSvgRenderer renderer(iconData);
    if (renderer.isValid()) {
        renderer.render(painter, rect);
    }
}

void FluentIconUtils::drawSvgIcon(const QString& iconPath, QPainter* painter, const QRectF& rect)
{
    if (!painter || iconPath.isEmpty() || rect.isEmpty()) {
        return;
    }

    QFile file(iconPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    const QByteArray data = file.readAll();
    file.close();

    drawSvgIcon(data, painter, rect);
}

QString FluentIconUtils::writeSvg(const QString& iconPath,
                                  const QList<int>& indexes,
                                  const QHash<QString, QString>& attributes)
{
    if (iconPath.isEmpty() || !QFile::exists(iconPath) ||
            !iconPath.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
        return QString();
    }

    // Generate cache key
    QString cacheKey = iconPath;
    // Estimate additional space needed
    cacheKey.reserve(cacheKey.size() + attributes.size() * 32 + indexes.size() * 8);

    // Append attributes
    for (auto it = attributes.constBegin(); it != attributes.constEnd(); ++it) {
        cacheKey += QLatin1Char('_');
        cacheKey += it.key();
        cacheKey += it.value();
    }

    // Append indexes
    for (int index : indexes) {
        cacheKey += QLatin1Char('_');
        cacheKey += QString::number(index);
    }

    // Check cache
    {
        QMutexLocker locker(&s_svgCacheMutex);
        if (s_svgCache.contains(cacheKey)) {
            QByteArray* cachedData = s_svgCache.object(cacheKey);
            if (cachedData) {
                return QString::fromUtf8(*cachedData);
            }
        }
    }

    // Load and parse SVG
    QFile file(iconPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QDomDocument dom;
    QString errorMsg;
    int errorLine, errorColumn;
    if (!dom.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        file.close();
        qWarning("Failed to parse SVG at line %d, column %d: %s",
                 errorLine, errorColumn, qPrintable(errorMsg));
        return QString();
    }
    file.close();

    // Modify SVG paths
    const QDomNodeList pathNodes = dom.elementsByTagName(QStringLiteral("path"));

    // Determine which indices to process
    if (indexes.isEmpty()) {
        // Process all indices
        for (int i = 0; i < pathNodes.length(); ++i) {
            QDomElement element = pathNodes.at(i).toElement();
            if (!element.isNull()) {
                QHashIterator<QString, QString> it(attributes);
                while (it.hasNext()) {
                    it.next();
                    element.setAttribute(it.key(), it.value());
                }
            }
        }
    } else {
        // Process specified indices
        for (int index : indexes) {
            if (index >= 0 && index < pathNodes.length()) {
                QDomElement element = pathNodes.at(index).toElement();
                if (!element.isNull()) {
                    QHashIterator<QString, QString> it(attributes);
                    while (it.hasNext()) {
                        it.next();
                        element.setAttribute(it.key(), it.value());
                    }
                }
            }
        }
    }

    // Cache the result
    const QString result = dom.toString(-1); // -1 = no indentation for compact output
    const QByteArray resultData = result.toUtf8();
    {
        QMutexLocker locker(&s_svgCacheMutex);
        s_svgCache.insert(cacheKey, new QByteArray(resultData), resultData.size());
    }

    return result;
}

const QMap<Fluent::IconType, QString>& FluentIconUtils::fluentIconsMap()
{
    static const QMap<Fluent::IconType, QString> iconMap = {
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
    return iconMap;
}

QString FluentIconUtils::enumToString(Fluent::IconType iconType)
{
    return fluentIconsMap().value(iconType, QString());
}

QString FluentIconUtils::buildIconPath(Fluent::IconType iconEnum,
                                       const QString& templatePath,
                                       Fluent::ThemeMode theme,
                                       bool reverse,
                                       QString& cachedLightPath,
                                       QString& cachedDarkPath)
{
    // Determine effective theme considering reverse
    Fluent::ThemeMode effectiveTheme = theme;
    if (reverse && theme == Fluent::ThemeMode::AUTO) {
        effectiveTheme = Theme::instance()->isDarkTheme()
                ? Fluent::ThemeMode::LIGHT
                : Fluent::ThemeMode::DARK;
    }

    const QString color = iconColor(effectiveTheme);
    const bool isLight = (color == QStringLiteral("black"));
    QString& cache = isLight ? cachedLightPath : cachedDarkPath;

    if (!cache.isEmpty()) {
        return cache;
    }

    if (iconEnum != Fluent::IconType::CUSTOM_PATH) {
        const QString iconName = enumToString(iconEnum);
        if (!iconName.isEmpty()) {
            cache = QStringLiteral(":/qfluent/images/icons/%1_%2.svg").arg(iconName, color);
        }
    } else if (!templatePath.isEmpty()) {
        cache = templatePath;
        cache.replace(QStringLiteral("{color}"), color);
    }

    return cache;
}

QIcon FluentIconUtils::toQIcon(const QVariant& icon)
{
    if (icon.canConvert<QString>()) {
        const QString path = icon.toString();
        if (!path.isEmpty() && QFile::exists(path)) {
            return QIcon(path);
        }
    }
    if (icon.canConvert<QIcon>()) {
        return icon.value<QIcon>();
    }
    return QIcon();
}

void FluentIconUtils::drawIcon(const FluentIconBase& icon, QPainter* painter,
                               const QRectF& rect, Fluent::ThemeMode theme,
                               bool reverse, const QHash<QString, QString>& attributes)
{
    if (!painter || rect.isEmpty()) {
        return;
    }

    icon.render(painter, rect, theme, reverse, QList<int>(), attributes);
}

// ====================== FluentIconEngine ======================

FluentIconEngine::FluentIconEngine(Fluent::IconType iconType, bool reverse)
    : m_iconType(iconType)
    , m_isThemeReversed(reverse)
{
    if (m_iconType != Fluent::IconType::CUSTOM_PATH) {
        m_iconBase = QSharedPointer<FluentIconBase>(new FluentIcon(m_iconType));
    }
}

FluentIconEngine::FluentIconEngine(const QString& templatePath, bool reverse)
    : m_iconType(Fluent::IconType::CUSTOM_PATH)
    , m_templatePath(templatePath)
    , m_isThemeReversed(reverse)
{
    if (!templatePath.isEmpty()) {
        m_iconBase = QSharedPointer<FluentIconBase>(new FluentIcon(m_templatePath));
    }
}

FluentIconEngine::~FluentIconEngine() = default;

void FluentIconEngine::paint(QPainter* painter, const QRect& rect,
                             QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(state)

    if (!painter || !painter->isActive() || rect.isEmpty() || !m_iconBase) {
        return;
    }

    painter->save();

    // Apply mode-based opacity
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

    m_iconBase->render(painter, rect, Fluent::ThemeMode::AUTO, m_isThemeReversed);
    painter->restore();
}

QIconEngine* FluentIconEngine::clone() const
{
    // Use std::unique_ptr for exception safety
    std::unique_ptr<FluentIconEngine> engine;

    if (m_iconType == Fluent::IconType::CUSTOM_PATH) {
        engine.reset(new FluentIconEngine(m_templatePath, m_isThemeReversed));
    } else {
        engine.reset(new FluentIconEngine(m_iconType, m_isThemeReversed));
    }

    if (m_iconBase && engine) {
        engine->m_iconBase = QSharedPointer<FluentIconBase>(m_iconBase->clone().release());
    }

    return engine.release();
}

QPixmap FluentIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (size.isEmpty()) {
        return QPixmap();
    }

    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPixmap pixmap = QPixmap::fromImage(image);
    QPainter painter(&pixmap);
    paint(&painter, QRect(QPoint(0, 0), size), mode, state);

    return pixmap;
}

// ====================== SvgIconEngine ======================

SvgIconEngine::SvgIconEngine(const QByteArray& svgData)
    : m_svgData(svgData)
{
}

SvgIconEngine::SvgIconEngine(SvgIconEngine&& other) noexcept
    : QIconEngine()  // QIconEngine is abstract base class, use default constructor
    , m_svgData(std::move(other.m_svgData))
{
}

SvgIconEngine& SvgIconEngine::operator=(SvgIconEngine&& other) noexcept
{
    if (this != &other) {
        // QIconEngine doesn't have assignment operator, skip base class assignment
        m_svgData = std::move(other.m_svgData);
    }
    return *this;
}

void SvgIconEngine::paint(QPainter* painter, const QRect& rect,
                          QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)

    FluentIconUtils::drawSvgIcon(m_svgData, painter, rect);
}

QIconEngine* SvgIconEngine::clone() const
{
    return new SvgIconEngine(m_svgData);
}

QPixmap SvgIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (size.isEmpty()) {
        return QPixmap();
    }

    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPixmap pixmap = QPixmap::fromImage(image);
    QPainter painter(&pixmap);
    paint(&painter, QRect(QPoint(0, 0), size), mode, state);

    return pixmap;
}

// ====================== FontIconEngine ======================

FontIconEngine::FontIconEngine(const QString& fontFamily, QChar character,
                               const QColor& color, bool isBold)
    : m_fontFamily(fontFamily)
    , m_character(character)
    , m_color(color)
    , m_isBold(isBold)
{
}

FontIconEngine::FontIconEngine(FontIconEngine&& other) noexcept
    : QIconEngine()  // QIconEngine is abstract base class, use default constructor
    , m_fontFamily(std::move(other.m_fontFamily))
    , m_character(other.m_character)
    , m_color(other.m_color)
    , m_isBold(other.m_isBold)
{
}

FontIconEngine& FontIconEngine::operator=(FontIconEngine&& other) noexcept
{
    if (this != &other) {
        // QIconEngine doesn't have assignment operator, skip base class assignment
        m_fontFamily = std::move(other.m_fontFamily);
        m_character = other.m_character;
        m_color = other.m_color;
        m_isBold = other.m_isBold;
    }
    return *this;
}

void FontIconEngine::paint(QPainter* painter, const QRect& rect,
                           QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)

    if (!painter || rect.isEmpty() || m_character.isNull()) {
        return;
    }

    QFont font(m_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(qRound(rect.height() * 0.8));

    painter->save();
    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    const QFontMetrics fontMetrics(font);
    QRectF textRect = rect;
    textRect.moveTop((rect.height() - fontMetrics.height()) / 2.0);

    painter->drawText(textRect, Qt::AlignCenter, QString(m_character));
    painter->restore();
}

QIconEngine* FontIconEngine::clone() const
{
    return new FontIconEngine(m_fontFamily, m_character, m_color, m_isBold);
}

QPixmap FontIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (size.isEmpty()) {
        return QPixmap();
    }

    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPixmap pixmap = QPixmap::fromImage(image);
    QPainter painter(&pixmap);
    paint(&painter, QRect(QPoint(0, 0), size), mode, state);

    return pixmap;
}

// ====================== FluentIconBase ======================

QIcon FluentIconBase::icon(Fluent::ThemeMode theme, const QColor& color) const
{
    Q_UNUSED(color)
    const QString iconPath = path(theme, false);
    return iconPath.isEmpty() ? QIcon() : QIcon(iconPath);
}

std::unique_ptr<FluentIconBase> FluentIconBase::colored(const QColor& lightColor,
                                                        const QColor& darkColor) const
{
    // Default implementation returns a ColoredFluentIcon with empty path
    // Subclasses should override to provide proper implementation
    return std::make_unique<ColoredFluentIcon>(Fluent::IconType::CUSTOM_PATH, lightColor, darkColor);
}

std::unique_ptr<FluentIconBase> FluentIconBase::colored(const QColor& color) const
{
    return colored(color, color);
}

void FluentIconBase::render(QPainter* painter, const QRectF& rect,
                            Fluent::ThemeMode theme,
                            bool reverse,
                            const QList<int>& indexes,
                            const QHash<QString, QString>& attributes) const
{
    if (!painter || rect.isEmpty()) {
        return;
    }

    const QString iconPath = path(theme, reverse);
    if (iconPath.isEmpty()) {
        return;
    }

    if (iconPath.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
        QByteArray svgData;

        if (!attributes.isEmpty()) {
            const QString modifiedSvg = FluentIconUtils::writeSvg(iconPath, indexes, attributes);
            svgData = modifiedSvg.toUtf8();
        } else {
            QFile file(iconPath);
            if (file.open(QIODevice::ReadOnly)) {
                svgData = file.readAll();
                file.close();
            } else {
                qWarning("FluentIconBase: Failed to open icon file: %s", qPrintable(iconPath));
            }
        }

        if (!svgData.isEmpty()) {
            FluentIconUtils::drawSvgIcon(svgData, painter, rect);
        }
    } else {
        QIcon(iconPath).paint(painter, rect.toRect(), Qt::AlignCenter);
    }
}

// ====================== FluentFontIconBase ======================

QMutex FluentFontIconBase::s_fontMutex;
bool FluentFontIconBase::s_isFontLoaded = false;
int FluentFontIconBase::s_fontId = -1;
QString FluentFontIconBase::s_fontFamily;
QHash<QString, QChar> FluentFontIconBase::s_iconNames;

FluentFontIconBase::FluentFontIconBase(QChar character)
    : m_character(character)
    , m_lightColor(Qt::black)
    , m_darkColor(Qt::white)
    , m_isBold(false)
{
    loadFont();
}

QString FluentFontIconBase::path(Fluent::ThemeMode theme, bool reverse) const
{
    Q_UNUSED(theme)
    Q_UNUSED(reverse)
    return fontPath();
}

FluentFontIconBase FluentFontIconBase::fromName(const QString& name)
{
    QMutexLocker locker(&s_fontMutex);
    return FluentFontIconBase(s_iconNames.value(name, QChar()));
}

FluentFontIconBase& FluentFontIconBase::bold()
{
    m_isBold = true;
    return *this;
}

QIcon FluentFontIconBase::icon(Fluent::ThemeMode theme, const QColor& color) const
{
    const QColor finalColor = color.isValid() ? color : iconColor(theme);
    return QIcon(new FontIconEngine(s_fontFamily, m_character, finalColor, m_isBold));
}

// FluentFontIconBase uses the base class default colored() implementation
// which will warn users that font icons don't support this operation.
// Users should use icon(theme, color) instead for font icons.

void FluentFontIconBase::render(QPainter* painter, const QRectF& rect,
                                Fluent::ThemeMode theme,
                                bool reverse,
                                const QList<int>& indexes,
                                const QHash<QString, QString>& attributes) const
{
    Q_UNUSED(indexes)

    if (!painter || rect.isEmpty() || m_character.isNull()) {
        return;
    }

    const QColor color = attributes.contains(QStringLiteral("fill"))
            ? QColor(attributes.value(QStringLiteral("fill")))
            : iconColor(theme, reverse);

    QFont font(s_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(qRound(rect.height() * 0.8));

    painter->save();
    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->drawText(rect, Qt::AlignCenter, QString(m_character));
    painter->restore();
}

std::unique_ptr<FluentIconBase> FluentFontIconBase::clone() const
{
    auto copy = std::make_unique<FluentFontIconBase>(m_character);
    copy->m_lightColor = m_lightColor;
    copy->m_darkColor = m_darkColor;
    copy->m_isBold = m_isBold;
    return copy;
}

void FluentFontIconBase::loadFont()
{
    QMutexLocker locker(&s_fontMutex);

    if (s_isFontLoaded || !QApplication::instance()) {
        return;
    }

    const QString fontFilePath = fontPath();
    if (fontFilePath.isEmpty() || !QFile::exists(fontFilePath)) {
        qWarning("FluentFontIconBase: Font file not found: %s", qPrintable(fontFilePath));
        s_isFontLoaded = true;  // 标记为已尝试，防止重试循环
        return;
    }

    QFile file(fontFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("FluentFontIconBase: Failed to open font file: %s", qPrintable(fontFilePath));
        s_isFontLoaded = true;  // 标记为已尝试，防止重试循环
        return;
    }

    const QByteArray fontData = file.readAll();
    file.close();

    s_fontId = QFontDatabase::addApplicationFontFromData(fontData);
    if (s_fontId != -1) {
        const QStringList families = QFontDatabase::applicationFontFamilies(s_fontId);
        if (!families.isEmpty()) {
            s_fontFamily = families.first();
            s_isFontLoaded = true;
        }
    } else {
        qWarning("FluentFontIconBase: Failed to load application font from data");
        s_isFontLoaded = true;  // 标记为已尝试，防止重试循环
    }

    if (!iconNameMapPath().isEmpty()) {
        loadIconNames();
    }
}

void FluentFontIconBase::loadIconNames()
{
    QMutexLocker locker(&s_fontMutex);
    s_iconNames.clear();

    const QString mapPath = iconNameMapPath();
    if (mapPath.isEmpty() || !QFile::exists(mapPath)) {
        return;
    }

    QFile file(mapPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    const QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning("Failed to parse icon name map: %s", qPrintable(parseError.errorString()));
        return;
    }

    if (doc.isObject()) {
        const QJsonObject obj = doc.object();
        for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
            const QString charStr = it.value().toString();
            if (!charStr.isEmpty()) {
                s_iconNames.insert(it.key(), charStr.at(0));
            }
        }
    }
}

QColor FluentFontIconBase::iconColor(Fluent::ThemeMode theme, bool reverse) const
{
    Fluent::ThemeMode effectiveTheme = theme;
    if (reverse && theme == Fluent::ThemeMode::AUTO) {
        effectiveTheme = Theme::instance()->isDarkTheme()
                ? Fluent::ThemeMode::LIGHT
                : Fluent::ThemeMode::DARK;
    }

    if (effectiveTheme == Fluent::ThemeMode::AUTO) {
        return Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
    }
    return (effectiveTheme == Fluent::ThemeMode::DARK) ? m_darkColor : m_lightColor;
}

// ====================== ColoredFluentIcon ======================

ColoredFluentIcon::ColoredFluentIcon(Fluent::IconType iconEnum,
                                     const QColor& lightColor,
                                     const QColor& darkColor)
    : m_iconEnum(iconEnum)
    , m_lightColor(lightColor)
    , m_darkColor(darkColor)
{
}

ColoredFluentIcon::ColoredFluentIcon(const QString& templatePath,
                                     const QColor& lightColor,
                                     const QColor& darkColor)
    : m_iconEnum(Fluent::IconType::CUSTOM_PATH)
    , m_templatePath(templatePath)
    , m_lightColor(lightColor)
    , m_darkColor(darkColor)
{
}

QString ColoredFluentIcon::path(Fluent::ThemeMode theme, bool reverse) const
{
    return FluentIconUtils::buildIconPath(m_iconEnum, m_templatePath, theme, reverse,
                                          m_cachedLightPath, m_cachedDarkPath);
}

void ColoredFluentIcon::render(QPainter* painter, const QRectF& rect,
                               Fluent::ThemeMode theme,
                               bool reverse,
                               const QList<int>& indexes,
                               const QHash<QString, QString>& attributes) const
{
    if (!painter || rect.isEmpty()) {
        return;
    }

    const QString iconPath = path(theme, reverse);
    if (iconPath.isEmpty()) {
        return;
    }

    if (!iconPath.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
        QIcon(iconPath).paint(painter, rect.toRect(), Qt::AlignCenter);
        return;
    }

    // Determine effective theme for color selection
    Fluent::ThemeMode effectiveTheme = theme;
    if (reverse && theme == Fluent::ThemeMode::AUTO) {
        effectiveTheme = Theme::instance()->isDarkTheme()
                ? Fluent::ThemeMode::LIGHT
                : Fluent::ThemeMode::DARK;
    }

    const QColor color = (effectiveTheme == Fluent::ThemeMode::AUTO)
            ? (Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor)
            : (effectiveTheme == Fluent::ThemeMode::DARK ? m_darkColor : m_lightColor);

    QHash<QString, QString> modifiedAttributes = attributes;
    modifiedAttributes.insert(QStringLiteral("fill"), color.name());

    const QString svgContent = FluentIconUtils::writeSvg(iconPath, indexes, modifiedAttributes);
    const QByteArray svgData = svgContent.toUtf8();

    FluentIconUtils::drawSvgIcon(svgData, painter, rect);
}

std::unique_ptr<FluentIconBase> ColoredFluentIcon::colored(const QColor& lightColor,
                                                           const QColor& darkColor) const
{
    if (m_iconEnum == Fluent::IconType::CUSTOM_PATH) {
        return std::make_unique<ColoredFluentIcon>(m_templatePath, lightColor, darkColor);
    }
    return std::make_unique<ColoredFluentIcon>(m_iconEnum, lightColor, darkColor);
}

std::unique_ptr<FluentIconBase> ColoredFluentIcon::clone() const
{
    if (m_iconEnum == Fluent::IconType::CUSTOM_PATH) {
        return std::make_unique<ColoredFluentIcon>(m_templatePath, m_lightColor, m_darkColor);
    }
    return std::make_unique<ColoredFluentIcon>(m_iconEnum, m_lightColor, m_darkColor);
}

// ====================== FluentIcon ======================

FluentIcon::FluentIcon(Fluent::IconType iconEnum)
    : m_iconEnum(iconEnum)
{
}

FluentIcon::FluentIcon(const QString& templatePath)
    : m_templatePath(templatePath)
    , m_iconEnum(Fluent::IconType::CUSTOM_PATH)
{
}

QString FluentIcon::path(Fluent::ThemeMode theme, bool reverse) const
{
    return FluentIconUtils::buildIconPath(m_iconEnum, m_templatePath, theme, reverse,
                                          m_cachedLightPath, m_cachedDarkPath);
}

QIcon FluentIcon::qicon(bool reverse) const
{
    if (m_iconEnum == Fluent::IconType::CUSTOM_PATH) {
        return QIcon(new FluentIconEngine(m_templatePath, reverse));
    }
    return QIcon(new FluentIconEngine(m_iconEnum, reverse));
}

std::unique_ptr<FluentIconBase> FluentIcon::colored(const QColor& lightColor,
                                                    const QColor& darkColor) const
{
    if (m_iconEnum == Fluent::IconType::CUSTOM_PATH) {
        return std::make_unique<ColoredFluentIcon>(m_templatePath, lightColor, darkColor);
    }
    return std::make_unique<ColoredFluentIcon>(m_iconEnum, lightColor, darkColor);
}

std::unique_ptr<FluentIconBase> FluentIcon::clone() const
{
    if (m_iconEnum == Fluent::IconType::CUSTOM_PATH) {
        return std::make_unique<FluentIcon>(m_templatePath);
    }
    return std::make_unique<FluentIcon>(m_iconEnum);
}

// ====================== Global Function ======================

QIcon toQIcon(const QVariant& icon)
{
    return FluentIconUtils::toQIcon(icon);
}

// ====================== Action ======================

Action::Action(QObject* parent)
    : QAction(parent)
{
}

Action::Action(const QString& text, QObject* parent)
    : QAction(text, parent)
{
}

Action::Action(const QIcon& icon, const QString& text, QObject* parent)
    : QAction(icon, text, parent)
{
}

Action::Action(const FluentIconBase& icon, const QString& text, QObject* parent)
    : QAction(icon.icon(), text, parent)
    , m_fluentIcon(icon.clone().release())
{
}

QIcon Action::icon() const
{
    if (m_fluentIcon) {
        return m_fluentIcon->icon();
    }
    return QAction::icon();
}

void Action::setIcon(const QIcon& icon)
{
    m_fluentIcon.clear();
    QAction::setIcon(icon);
}

void Action::setFluentIcon(const FluentIconBase& icon)
{
    m_fluentIcon = QSharedPointer<FluentIconBase>(icon.clone().release());
    QAction::setIcon(m_fluentIcon->icon());
}

// ====================== 析构函数实现 ======================

FluentIconBase::~FluentIconBase() = default;

SvgIconEngine::~SvgIconEngine() = default;

FontIconEngine::~FontIconEngine() = default;

FluentFontIconBase::~FluentFontIconBase() = default;

ColoredFluentIcon::~ColoredFluentIcon() = default;

FluentIcon::~FluentIcon() = default;

Action::~Action() = default;
