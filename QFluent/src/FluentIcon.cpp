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
#include <QPainterPath>
#include <QJsonDocument>
#include <QJsonObject>

#include "Theme.h"

// 获取图标颜色
QString getIconColor(ThemeType::ThemeMode theme, bool reverse) {
    QString lc = reverse ? "white" : "black";
    QString dc = reverse ? "black" : "white";

    QString color;
    if (theme == ThemeType::AUTO) {
        color = Theme::instance()->isDarkTheme() ? dc : lc;
    } else {
        color = (theme == ThemeType::DARK) ? dc : lc;
    }

    return color;
}

// 绘制SVG图标
void drawSvgIcon(const QByteArray& icon, QPainter* painter, const QRectF& rect) {
    QSvgRenderer renderer(icon);
    renderer.render(painter, rect);
}

void drawSvgIcon(const QString& iconPath, QPainter* painter, const QRectF& rect) {
    QSvgRenderer renderer(iconPath);
    renderer.render(painter, rect);
}

// 写入SVG
QString writeSvg(const QString& iconPath, const QList<int>& indexes, const QMap<QString, QString>& attributes) {
    if (!iconPath.toLower().endsWith(".svg")) {
        return QString();
    }

    QFile file(iconPath);
    if (!file.open(QFile::ReadOnly)) {
        return QString();
    }

    QDomDocument dom;
    dom.setContent(&file);
    file.close();

    // 修改每个path的颜色
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

        for (auto it = attributes.begin(); it != attributes.end(); ++it) {
            element.setAttribute(it.key(), it.value());
        }
    }

    return dom.toString();
}

// 绘制图标
// void drawIcon(const QVariant& icon, QPainter* painter, const QRectF& rect, QIcon::State state, const QMap<QString, QString>& attributes) {
//     if (icon.canConvert<FluentIconBase*>()) {
//         FluentIconBase* fluentIcon = icon.value<FluentIconBase*>();
//         if (fluentIcon) {
//             fluentIcon->render(painter, rect, ThemeType::AUTO, QList<int>(), attributes);
//         }
//     } else if (icon.canConvert<Icon>()) {
//         Icon iconObj = icon.value<Icon>();
//         iconObj.fluentIcon().render(painter, rect, ThemeType::AUTO, QList<int>(), attributes);
//     } else if (icon.canConvert<QIcon>()) {
//         QIcon qicon = icon.value<QIcon>();
//         qicon.paint(painter, rect.toRect(), Qt::AlignCenter, QIcon::Normal, state);
//     } else if (icon.canConvert<QString>()) {
//         QIcon qicon(icon.toString());
//         qicon.paint(painter, rect.toRect(), Qt::AlignCenter, QIcon::Normal, state);
//     }
// }

// FluentIconEngine 实现
FluentIconEngine::FluentIconEngine(FluentIconType::IconType iconType, bool reverse)
    : m_iconType(iconType), m_templatePath(""), m_isThemeReversed(reverse) {
    if (m_iconType != FluentIconType::CUSTOM_PATH) {
        m_iconBase = std::make_unique<FluentIcon>(m_iconType);  // 预先创建并保存
    }
}

FluentIconEngine::FluentIconEngine(const QString& templatePath, bool reverse)
    : m_iconType(FluentIconType::CUSTOM_PATH), m_templatePath(templatePath), m_isThemeReversed(reverse) {
    m_iconBase = std::make_unique<FluentIcon>(m_templatePath);
}

void FluentIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    if (!painter || !painter->isActive() || rect.isEmpty()) {
        return;
    }

    painter->save();

    if (mode == QIcon::Disabled) {
        painter->setOpacity(0.5);
    } else if (mode == QIcon::Selected) {
        painter->setOpacity(0.7);
    }

    // 使用保存的 m_iconBase，避免重复创建
    ThemeType::ThemeMode theme = m_isThemeReversed ?
                (Theme::instance()->isDarkTheme() ? ThemeType::LIGHT : ThemeType::DARK) : ThemeType::AUTO;

    if (m_iconBase) {
        QIcon qicon = m_iconBase->icon(theme);
        qicon.paint(painter, rect, Qt::AlignCenter, QIcon::Normal, state);
    }

    painter->restore();
}

QIconEngine* FluentIconEngine::clone() const {
    if (m_iconType == FluentIconType::CUSTOM_PATH) {
        auto* engine = new FluentIconEngine(m_templatePath, m_isThemeReversed);
        engine->m_iconBase = std::make_unique<FluentIcon>(m_templatePath);  // 克隆时重新创建
        return engine;
    } else {
        auto* engine = new FluentIconEngine(m_iconType, m_isThemeReversed);
        engine->m_iconBase = std::make_unique<FluentIcon>(m_iconType);  // 克隆时重新创建
        return engine;
    }
}

QPixmap FluentIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

    QPainter painter(&pixmap);
    QRect rect(0, 0, size.width(), size.height());
    paint(&painter, rect, mode, state);
    return pixmap;
}

// SvgIconEngine 实现
SvgIconEngine::SvgIconEngine(const QString& svg) : m_svg(svg) {
}

void SvgIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode)
    Q_UNUSED(state)
    drawSvgIcon(m_svg.toUtf8(), painter, rect);
}

QIconEngine* SvgIconEngine::clone() const {
    return new SvgIconEngine(m_svg);
}

QPixmap SvgIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

    QPainter painter(&pixmap);
    QRect rect(0, 0, size.width(), size.height());
    paint(&painter, rect, mode, state);
    return pixmap;
}

// FontIconEngine 实现
FontIconEngine::FontIconEngine(const QString& fontFamily, const QString& ch, const QColor& color, bool isBold)
    : m_fontFamily(fontFamily), m_char(ch), m_color(color), m_isBold(isBold) {
}

void FontIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode)
    Q_UNUSED(state)

    QFont font(m_fontFamily);
    font.setBold(m_isBold);
    // font.setPixelSize(qRound(rect.height()));
    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QPainterPath path;
    path.addText(rect.x(), rect.y() + rect.height(), font, m_char);
    painter->drawPath(path);
}

QIconEngine* FontIconEngine::clone() const {
    return new FontIconEngine(m_fontFamily, m_char, m_color, m_isBold);
}

QPixmap FontIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

    QPainter painter(&pixmap);
    QRect rect(0, 0, size.width(), size.height());
    paint(&painter, rect, mode, state);
    return pixmap;
}

// FluentIconBase 实现
QIcon FluentIconBase::icon(ThemeType::ThemeMode theme, const QColor& color) const {
    QString iconPath = path(theme);

    if (!iconPath.endsWith(".svg") || !color.isValid()) {
        return QIcon(iconPath);
    }

    QMap<QString, QString> attrs;
    attrs["fill"] = color.name();
    QString svg = writeSvg(iconPath, QList<int>(), attrs);
    return QIcon(new SvgIconEngine(svg));
}

ColoredFluentIcon FluentIconBase::colored(const QColor& lightColor, const QColor& darkColor) const {
    return ColoredFluentIcon(*this, lightColor, darkColor);
}

QIcon FluentIconBase::qicon(bool reverse) const {
    return QIcon(new FluentIconEngine(FluentIconType::NONE, reverse));
}

void FluentIconBase::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                            const QList<int>& indexes, const QMap<QString, QString>& attributes) const {
    QString iconPath = path(theme);

    if (iconPath.endsWith(".svg")) {
        QByteArray iconData;
        if (!attributes.isEmpty()) {
            iconData = writeSvg(iconPath, indexes, attributes).toUtf8();
        } else {
            QFile file(iconPath);
            if (file.open(QFile::ReadOnly)) {
                iconData = file.readAll();
                file.close();
            }
        }

        if (!iconData.isEmpty()) {
            drawSvgIcon(iconData, painter, rect);
        }
    } else {
        QIcon icon(iconPath);
        QRect rectInt = rect.toRect();
        painter->drawPixmap(rectInt, icon.pixmap(rectInt.size()));
    }
}

// FluentFontIconBase 静态成员初始化
bool FluentFontIconBase::s_isFontLoaded = false;
int FluentFontIconBase::s_fontId = -1;
QString FluentFontIconBase::s_fontFamily;
QMap<QString, QString> FluentFontIconBase::s_iconNames;

FluentFontIconBase::FluentFontIconBase(const QString& ch)
    : m_char(ch), m_lightColor(0, 0, 0), m_darkColor(255, 255, 255), m_isBold(false) {
    loadFont();
}

QString FluentFontIconBase::path(ThemeType::ThemeMode theme) const {
    Q_UNUSED(theme)
    return fontPath();
}

FluentFontIconBase FluentFontIconBase::fromName(const QString& name) {
    FluentFontIconBase icon("");
    icon.m_char = s_iconNames.value(name, "");
    return icon;
}

FluentFontIconBase& FluentFontIconBase::bold() {
    m_isBold = true;
    return *this;
}

QIcon FluentFontIconBase::icon(ThemeType::ThemeMode theme, const QColor& color) const {
    QColor iconColor = color.isValid() ? color : getIconColor(theme);
    return QIcon(new FontIconEngine(s_fontFamily, m_char, iconColor, m_isBold));
}

ColoredFluentIcon FluentFontIconBase::colored(const QColor& lightColor, const QColor& darkColor) const {
    FluentFontIconBase* self = const_cast<FluentFontIconBase*>(this);
    self->m_lightColor = lightColor;
    self->m_darkColor = darkColor;
    return ColoredFluentIcon(*this, lightColor, darkColor);
}

void FluentFontIconBase::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                                const QList<int>& indexes, const QMap<QString, QString>& attributes) const {
    Q_UNUSED(indexes)

    QColor color = getIconColor(theme);

    if (attributes.contains("fill")) {
        color = QColor(attributes["fill"]);
    }

    QFont font(s_fontFamily);
    font.setBold(m_isBold);
    font.setPixelSize(qRound(rect.height()));
    painter->setFont(font);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QPainterPath path;
    path.addText(rect.x(), rect.y() + rect.height(), font, m_char);
    painter->drawPath(path);
}

void FluentFontIconBase::loadFont() {
    if (s_isFontLoaded || !QApplication::instance()) {
        return;
    }

    QString fontFilePath = fontPath();
    if (fontFilePath.isEmpty()) {
        return;
    }

    QFile file(fontFilePath);
    if (!file.open(QFile::ReadOnly)) {
        qWarning("Cannot open font file: %s", qPrintable(fontFilePath));
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    s_fontId = QFontDatabase::addApplicationFontFromData(data);
    if (s_fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(s_fontId);
        if (!fontFamilies.isEmpty()) {
            s_fontFamily = fontFamilies.first();
            s_isFontLoaded = true;
        }
    }

    if (!iconNameMapPath().isEmpty()) {
        loadIconNames();
    }
}

void FluentFontIconBase::loadIconNames() {
    s_iconNames.clear();

    QFile file(iconNameMapPath());
    if (!file.open(QFile::ReadOnly)) {
        qWarning("Cannot open icon name map file: %s", qPrintable(iconNameMapPath()));
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            s_iconNames[it.key()] = it.value().toString();
        }
    }
}

QColor FluentFontIconBase::getIconColor(ThemeType::ThemeMode theme) const {
    if (theme == ThemeType::AUTO) {
        return Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
    }
    return (theme == ThemeType::DARK) ? m_darkColor : m_lightColor;
}

// ColoredFluentIcon 实现
ColoredFluentIcon::ColoredFluentIcon(const FluentIconBase& icon, const QColor& lightColor, const QColor& darkColor)
    : m_fluentIcon(&icon), m_lightColor(lightColor), m_darkColor(darkColor) {
}

QString ColoredFluentIcon::path(ThemeType::ThemeMode theme) const {
    return m_fluentIcon ? m_fluentIcon->path(theme) : QString();
}

void ColoredFluentIcon::render(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme,
                               const QList<int>& indexes, const QMap<QString, QString>& attributes) const {
    if (!m_fluentIcon) return;

    QString iconPath = path(theme);

    if (!iconPath.endsWith(".svg")) {
        m_fluentIcon->render(painter, rect, theme, indexes, attributes);
        return;
    }

    QColor color;
    if (theme == ThemeType::AUTO) {
        color = Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
    } else {
        color = (theme == ThemeType::DARK) ? m_darkColor : m_lightColor;
    }

    QMap<QString, QString> newAttributes = attributes;
    newAttributes["fill"] = color.name();

    QByteArray iconData = writeSvg(iconPath, indexes, newAttributes).toUtf8();
    drawSvgIcon(iconData, painter, rect);
}

// FluentIcon 实现
FluentIcon::FluentIcon(FluentIconType::IconType iconEnum) : m_iconEnum(iconEnum) {
}

FluentIcon::FluentIcon(const QString& templatePath)
    : m_iconEnum(FluentIconType::CUSTOM_PATH)
    , m_templatePath(templatePath) {}

QString FluentIcon::path(ThemeType::ThemeMode theme) const {
    QString color = getIconColor(theme);

    if (m_iconEnum != FluentIconType::CUSTOM_PATH) {
        QString iconName = enumToString(m_iconEnum);
        return QString(":/res/images/icons/%1_%2.svg").arg(iconName, color);
    } else {
        QString result = m_templatePath;
        result.replace(QStringLiteral("{color}"), color);
        return result;
    }
}

QIcon FluentIcon::qicon(bool reverse) const {
    if (m_iconEnum == FluentIconType::CUSTOM_PATH) {
        return QIcon(new FluentIconEngine(m_templatePath, reverse));
    } else {
        return QIcon(new FluentIconEngine(m_iconEnum, reverse));
    }
}

QString FluentIcon::enumToString(FluentIconType::IconType e) const {
    return fluentIcons().value(e, "");
}

QMap<FluentIconType::IconType, QString> FluentIcon::fluentIcons(){
    static QMap<FluentIconType::IconType, QString> enumMap = {
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
    return enumMap;
}

// Icon 实现
Icon::Icon(const FluentIcon& fluentIcon)
    : QIcon(fluentIcon.path()), m_fluentIcon(fluentIcon) {
}

// toQIcon 实现
QIcon toQIcon(const QVariant& icon) {
    if (icon.canConvert<QString>()) {
        return QIcon(icon.toString());
    }

    if (icon.canConvert<FluentIconBase*>()) {
        FluentIconBase* fluentIcon = icon.value<FluentIconBase*>();
        if (fluentIcon) {
            return fluentIcon->icon();
        }
    }

    if (icon.canConvert<QIcon>()) {
        return icon.value<QIcon>();
    }

    return QIcon();
}

// Action 实现
Action::Action(QObject* parent) : QAction(parent), m_fluentIcon(nullptr) {
}

Action::Action(const QString& text, QObject* parent)
    : QAction(text, parent), m_fluentIcon(nullptr) {
}

Action::Action(const QIcon& icon, const QString& text, QObject* parent)
    : QAction(icon, text, parent), m_fluentIcon(nullptr) {
}

Action::Action(const FluentIconBase& icon, const QString& text, QObject* parent)
    : QAction(icon.icon(), text, parent), m_fluentIcon(&icon) {
}

QIcon Action::icon() const {
    if (m_fluentIcon) {
        return m_fluentIcon->icon();
    }
    return QAction::icon();
}

void Action::setIcon(const QIcon& icon) {
    m_fluentIcon = nullptr;
    QAction::setIcon(icon);
}

void Action::setFluentIcon(const FluentIconBase& icon) {
    m_fluentIcon = &icon;
    QAction::setIcon(icon.icon());
}
