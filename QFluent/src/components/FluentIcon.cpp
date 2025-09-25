#include "FluentIcon.h"
#include <QDebug>

#include "../common/Theme.h"

IconEngine::IconEngine(const QString& fillPath, const QString& baseName, const QString& lightSuffix, const QString& darkSuffix)
{
    m_fillPath = fillPath;
    m_baseName = baseName;
    m_lightSuffix = lightSuffix;
    m_darkSuffix = darkSuffix;
}

QIconEngine* IconEngine::clone() const {
    return new IconEngine(*this);
}

QPixmap IconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode);
    Q_UNUSED(state);

    const QString &path = m_fillPath.arg(m_baseName, sTheme->isDarkMode() ? m_darkSuffix : m_lightSuffix);

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    QSvgRenderer renderer(path);
    if (renderer.isValid()) {
        renderer.render(&painter, QRect(QPoint(0, 0), size));
    }

    painter.end();

    return pixmap;
}

void IconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode);
    Q_UNUSED(state);

    const QString &path = m_fillPath.arg(m_baseName, sTheme->isDarkMode() ? m_darkSuffix : m_lightSuffix);

    QSvgRenderer renderer(path);
    if (renderer.isValid()) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        renderer.render(painter, rect);
        painter->restore();
    }
}

QIcon Icon::SvgIcon(const QString &fillPath, const QString &baseName, const QString &lightSuffix, const QString &darkSuffix)
{
    return QIcon(new IconEngine(fillPath, baseName, lightSuffix, darkSuffix));
}

QIcon Icon::FluentIcon(IconType icon)
{
    return QIcon(new IconEngine(":/res/images/icons/%1_%2.svg", toString(icon), "black", "white"));
}


QString Icon::toString(IconType icon)
{
    static const QMap<IconType, QString> map = {
        {IconType::UP, "Up"},
        {IconType::ADD, "Add"},
        {IconType::BUS, "Bus"},
        {IconType::CAR, "Car"},
        {IconType::CUT, "Cut"},
        {IconType::IOT, "IOT"},
        {IconType::PIN, "Pin"},
        {IconType::TAG, "Tag"},
        {IconType::VPN, "VPN"},
        {IconType::CAFE, "Cafe"},
        {IconType::CHAT, "Chat"},
        {IconType::COPY, "Copy"},
        {IconType::CODE, "Code"},
        {IconType::DOWN, "Down"},
        {IconType::EDIT, "Edit"},
        {IconType::FLAG, "Flag"},
        {IconType::FONT, "Font"},
        {IconType::GAME, "Game"},
        {IconType::HELP, "Help"},
        {IconType::HIDE, "Hide"},
        {IconType::HOME, "Home"},
        {IconType::INFO, "Info"},
        {IconType::LEAF, "Leaf"},
        {IconType::LINK, "Link"},
        {IconType::MAIL, "Mail"},
        {IconType::MENU, "Menu"},
        {IconType::MUTE, "Mute"},
        {IconType::MORE, "More"},
        {IconType::MOVE, "Move"},
        {IconType::PLAY, "Play"},
        {IconType::SAVE, "Save"},
        {IconType::SEND, "Send"},
        {IconType::SYNC, "Sync"},
        {IconType::UNIT, "Unit"},
        {IconType::VIEW, "View"},
        {IconType::WIFI, "Wifi"},
        {IconType::ZOOM, "Zoom"},
        {IconType::ALBUM, "Album"},
        {IconType::BRUSH, "Brush"},
        {IconType::BROOM, "Broom"},
        {IconType::CLOSE, "Close"},
        {IconType::CLOUD, "Cloud"},
        {IconType::EMBED, "Embed"},
        {IconType::GLOBE, "Globe"},
        {IconType::HEART, "Heart"},
        {IconType::LABEL, "Label"},
        {IconType::MEDIA, "Media"},
        {IconType::MOVIE, "Movie"},
        {IconType::MUSIC, "Music"},
        {IconType::ROBOT, "Robot"},
        {IconType::PAUSE, "Pause"},
        {IconType::PASTE, "Paste"},
        {IconType::PHOTO, "Photo"},
        {IconType::PHONE, "Phone"},
        {IconType::PRINT, "Print"},
        {IconType::SHARE, "Share"},
        {IconType::TILES, "Tiles"},
        {IconType::UNPIN, "Unpin"},
        {IconType::VIDEO, "Video"},
        {IconType::TRAIN, "Train"},
        {IconType::ADD_TO, "AddTo"},
        {IconType::ACCEPT, "Accept"},
        {IconType::CAMERA, "Camera"},
        {IconType::CANCEL, "Cancel"},
        {IconType::DELETE, "Delete"},
        {IconType::FOLDER, "Folder"},
        {IconType::FILTER, "Filter"},
        {IconType::MARKET, "Market"},
        {IconType::SCROLL, "Scroll"},
        {IconType::LAYOUT, "Layout"},
        {IconType::GITHUB, "GitHub"},
        {IconType::UPDATE, "Update"},
        {IconType::REMOVE, "Remove"},
        {IconType::RETURN, "Return"},
        {IconType::PEOPLE, "People"},
        {IconType::QRCODE, "QRCode"},
        {IconType::RINGER, "Ringer"},
        {IconType::ROTATE, "Rotate"},
        {IconType::SEARCH, "Search"},
        {IconType::VOLUME, "Volume"},
        {IconType::FRIGID , "Frigid"},
        {IconType::SAVE_AS, "SaveAs"},
        {IconType::ZOOM_IN, "ZoomIn"},
        {IconType::CONNECT, "Connect"},
        {IconType::HISTORY, "History"},
        {IconType::SETTING, "Setting"},
        {IconType::PALETTE, "Palette"},
        {IconType::MESSAGE, "Message"},
        {IconType::FIT_PAGE, "FitPage"},
        {IconType::ZOOM_OUT, "ZoomOut"},
        {IconType::AIRPLANE, "Airplane"},
        {IconType::ASTERISK, "Asterisk"},
        {IconType::CALORIES, "Calories"},
        {IconType::CALENDAR, "Calendar"},
        {IconType::FEEDBACK, "Feedback"},
        {IconType::LIBRARY, "BookShelf"},
        {IconType::MINIMIZE, "Minimize"},
        {IconType::CHECKBOX, "CheckBox"},
        {IconType::DOCUMENT, "Document"},
        {IconType::LANGUAGE, "Language"},
        {IconType::DOWNLOAD, "Download"},
        {IconType::QUESTION, "Question"},
        {IconType::SPEAKERS, "Speakers"},
        {IconType::DATE_TIME, "DateTime"},
        {IconType::FONT_SIZE, "FontSize"},
        {IconType::HOME_FILL, "HomeFill"},
        {IconType::PAGE_LEFT, "PageLeft"},
        {IconType::SAVE_COPY, "SaveCopy"},
        {IconType::SEND_FILL, "SendFill"},
        {IconType::SKIP_BACK, "SkipBack"},
        {IconType::SPEED_OFF, "SpeedOff"},
        {IconType::ALIGNMENT, "Alignment"},
        {IconType::BLUETOOTH, "Bluetooth"},
        {IconType::COMPLETED, "Completed"},
        {IconType::CONSTRACT, "Constract"},
        {IconType::HEADPHONE, "Headphone"},
        {IconType::MEGAPHONE, "Megaphone"},
        {IconType::PROJECTOR, "Projector"},
        {IconType::EDUCATION, "Education"},
        {IconType::LEFT_ARROW, "LeftArrow"},
        {IconType::ERASE_TOOL, "EraseTool"},
        {IconType::PAGE_RIGHT, "PageRight"},
        {IconType::PLAY_SOLID, "PlaySolid"},
        {IconType::BOOK_SHELF, "BookShelf"},
        {IconType::HIGHTLIGHT, "Highlight"},
        {IconType::FOLDER_ADD, "FolderAdd"},
        {IconType::PAUSE_BOLD, "PauseBold"},
        {IconType::PENCIL_INK, "PencilInk"},
        {IconType::PIE_SINGLE, "PieSingle"},
        {IconType::QUICK_NOTE, "QuickNote"},
        {IconType::SPEED_HIGH, "SpeedHigh"},
        {IconType::STOP_WATCH, "StopWatch"},
        {IconType::ZIP_FOLDER, "ZipFolder"},
        {IconType::BASKETBALL, "Basketball"},
        {IconType::BRIGHTNESS, "Brightness"},
        {IconType::DICTIONARY, "Dictionary"},
        {IconType::MICROPHONE, "Microphone"},
        {IconType::ARROW_DOWN, "ChevronDown"},
        {IconType::FULL_SCREEN, "FullScreen"},
        {IconType::MIX_VOLUMES, "MixVolumes"},
        {IconType::REMOVE_FROM, "RemoveFrom"},
        {IconType::RIGHT_ARROW, "RightArrow"},
        {IconType::QUIET_HOURS, "QuietHours"},
        {IconType::FINGERPRINT, "Fingerprint"},
        {IconType::APPLICATION, "Application"},
        {IconType::CERTIFICATE, "Certificate"},
        {IconType::TRANSPARENT, "Transparent"},
        {IconType::IMAGE_EXPORT, "ImageExport"},
        {IconType::SPEED_MEDIUM, "SpeedMedium"},
        {IconType::LIBRARY_FILL, "LibraryFill"},
        {IconType::MUSIC_FOLDER, "MusicFolder"},
        {IconType::POWER_BUTTON, "PowerButton"},
        {IconType::SKIP_FORWARD, "SkipForward"},
        {IconType::CARE_UP_SOLID, "CareUpSolid"},
        {IconType::ACCEPT_MEDIUM, "AcceptMedium"},
        {IconType::CANCEL_MEDIUM, "CancelMedium"},
        {IconType::CHEVRON_RIGHT, "ChevronRight"},
        {IconType::CLIPPING_TOOL, "ClippingTool"},
        {IconType::SEARCH_MIRROR, "SearchMirror"},
        {IconType::SHOPPING_CART, "ShoppingCart"},
        {IconType::FONT_INCREASE, "FontIncrease"},
        {IconType::BACK_TO_WINDOW, "BackToWindow"},
        {IconType::COMMAND_PROMPT, "CommandPrompt"},
        {IconType::CLOUD_DOWNLOAD, "CloudDownload"},
        {IconType::DICTIONARY_ADD, "DictionaryAdd"},
        {IconType::CARE_DOWN_SOLID, "CareDownSolid"},
        {IconType::CARE_LEFT_SOLID, "CareLeftSolid"},
        {IconType::CLEAR_SELECTION, "ClearSelection"},
        {IconType::DEVELOPER_TOOLS, "DeveloperTools"},
        {IconType::BACKGROUND_FILL, "BackgroundColor"},
        {IconType::CARE_RIGHT_SOLID, "CareRightSolid"},
        {IconType::CHEVRON_DOWN_MED, "ChevronDownMed"},
        {IconType::CHEVRON_RIGHT_MED, "ChevronRightMed"},
        {IconType::EMOJI_TAB_SYMBOLS, "EmojiTabSymbols"},
        {IconType::EXPRESSIVE_INPUT_ENTRY, "ExpressiveInputEntry"}
    };
    return map.value(icon, "Unknown");
}



void Icon::drawSvgIcon(QPainter *painter, Icon::IconType icon,
                       const QRectF &rect, const QMap<QString, QString> &attributes)
{
    const QString &path = QString(":/res/images/icons/%1_%2.svg")
            .arg(Icon::toString(icon), sTheme->isDarkMode() ? "white" : "black");

    if (attributes.isEmpty()) {
        QSvgRenderer renderer(path);
        renderer.render(painter, rect);
    } else {
        QString svgData = writeSvg(path, attributes);
        if (!svgData.isEmpty()) {
            QSvgRenderer renderer(svgData.toUtf8());
            renderer.render(painter, rect);
        }
    }
}

void Icon::drawSvgIcon(QPainter *painter, const QString& fillPath, const QString& baseName,
                       const QString& lightSuffix, const QString& darkSuffix,
                       const QRectF &rect, const QMap<QString, QString> &attributes)
{
    const QString &path = fillPath.arg(baseName, sTheme->isDarkMode() ? lightSuffix : darkSuffix);

    if (attributes.isEmpty()) {
        QSvgRenderer renderer(path);
        renderer.render(painter, rect);
    } else {
        QString svgData = writeSvg(path, attributes);
        if (!svgData.isEmpty()) {
            QSvgRenderer renderer(svgData.toUtf8());
            renderer.render(painter, rect);
        }
    }
}


QString Icon::writeSvg(const QString &iconPath, const QMap<QString, QString> &attributes, const QList<int> &indexes)
{
    QFile file(iconPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << iconPath;
        return QString();
    }

    QByteArray data = file.readAll();
    file.close();

    QDomDocument doc;
    QDomDocument::ParseResult result = doc.setContent(data);
    if (!result) {
        qWarning() << "Failed to parse SVG XML:" << result.errorMessage
                   << "at line" << result.errorLine << "column" << result.errorColumn;
        return QString();
    }

    setAttributesOnPath(doc, attributes, indexes);

    return doc.toString();
}

void Icon::setAttributesOnPath(QDomDocument &doc, const QMap<QString, QString> &attrs, const QList<int> &idxs)
{
    QDomNodeList pathNodes = doc.elementsByTagName("path");
    QList<int> targetIndexes = idxs.isEmpty() ? QList<int>() : idxs;

    // 如果 indexes 为空，则应用到所有 path
    bool applyToAll = targetIndexes.isEmpty();
    for (int i = 0; i < pathNodes.size(); ++i) {
        if (!applyToAll && !targetIndexes.contains(i)) {
            continue;
        }

        QDomElement elem = pathNodes.at(i).toElement();
        for (auto it = attrs.constBegin(); it != attrs.constEnd(); ++it) {
            elem.setAttribute(it.key(), it.value());
        }
    }
}








Action::Action(QObject* parent) : QAction(parent)
{
}

Action::Action(const QString& text, QObject* parent) : QAction(text, parent)
{
}

Action::Action(const QIcon& icon, const QString& text, QObject* parent) : QAction(icon, text, parent)
{
}

Action::Action(const Icon::IconType icon, const QString& text, QObject* parent)
    : QAction(text, parent)
{
    setIcon(icon);
}

Action::~Action() = default;

QIcon Action::icon() const
{
    return m_Icon;
}

void Action::setIcon(const Icon::IconType icon)
{
    m_Icon = QIcon(Icon::FluentIcon(icon));
    QAction::setIcon(m_Icon);
}

