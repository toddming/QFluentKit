#ifndef FLUENT_ICONS_H
#define FLUENT_ICONS_H

#include <QIcon>
#include <QIconEngine>
#include <QRectF>
#include <QFile>
#include <QObject>
#include <QColor>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QAction>
#include <QSvgRenderer>
#include <QDomDocument>
#include <QApplication>
#include <QPalette>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QByteArray>
#include <QObject>

#include "../Property.h"

class QFLUENT_EXPORT IconEngine : public QIconEngine {

public:
    IconEngine(const QString& fillPath, const QString& baseName, const QString& lightSuffix, const QString& darkSuffix);

    ~IconEngine() override = default;

    void paint(QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

private:
    QString m_fillPath;
    QString m_baseName;
    QString m_lightSuffix;
    QString m_darkSuffix;
};


class QFLUENT_EXPORT Icon : public QObject
{
    Q_OBJECT
public:
    enum class IconType {
        UP,
        ADD,
        BUS,
        CAR,
        CUT,
        IOT,
        PIN,
        TAG,
        VPN,
        CAFE,
        CHAT,
        COPY,
        CODE,
        DOWN,
        EDIT,
        FLAG,
        FONT,
        GAME,
        HELP,
        HIDE,
        HOME,
        INFO,
        LEAF,
        LINK,
        MAIL,
        MENU,
        MUTE,
        MORE,
        MOVE,
        PLAY,
        SAVE,
        SEND,
        SYNC,
        UNIT,
        VIEW,
        WIFI,
        ZOOM,
        ALBUM,
        BRUSH,
        BROOM,
        CLOSE,
        CLOUD,
        EMBED,
        GLOBE,
        HEART,
        LABEL,
        MEDIA,
        MOVIE,
        MUSIC,
        ROBOT,
        PAUSE,
        PASTE,
        PHOTO,
        PHONE,
        PRINT,
        SHARE,
        TILES,
        UNPIN,
        VIDEO,
        TRAIN,
        ADD_TO,
        ACCEPT,
        CAMERA,
        CANCEL,
        DELETE,
        FOLDER,
        FILTER,
        MARKET,
        SCROLL,
        LAYOUT,
        GITHUB,
        UPDATE,
        REMOVE,
        RETURN,
        PEOPLE,
        QRCODE,
        RINGER,
        ROTATE,
        SEARCH,
        VOLUME,
        FRIGID ,
        SAVE_AS,
        ZOOM_IN,
        CONNECT,
        HISTORY,
        SETTING,
        PALETTE,
        MESSAGE,
        FIT_PAGE,
        ZOOM_OUT,
        AIRPLANE,
        ASTERISK,
        CALORIES,
        CALENDAR,
        FEEDBACK,
        LIBRARY,
        MINIMIZE,
        CHECKBOX,
        DOCUMENT,
        LANGUAGE,
        DOWNLOAD,
        QUESTION,
        SPEAKERS,
        DATE_TIME,
        FONT_SIZE,
        HOME_FILL,
        PAGE_LEFT,
        SAVE_COPY,
        SEND_FILL,
        SKIP_BACK,
        SPEED_OFF,
        ALIGNMENT,
        BLUETOOTH,
        COMPLETED,
        CONSTRACT,
        HEADPHONE,
        MEGAPHONE,
        PROJECTOR,
        EDUCATION,
        LEFT_ARROW,
        ERASE_TOOL,
        PAGE_RIGHT,
        PLAY_SOLID,
        BOOK_SHELF,
        HIGHTLIGHT,
        FOLDER_ADD,
        PAUSE_BOLD,
        PENCIL_INK,
        PIE_SINGLE,
        QUICK_NOTE,
        SPEED_HIGH,
        STOP_WATCH,
        ZIP_FOLDER,
        BASKETBALL,
        BRIGHTNESS,
        DICTIONARY,
        MICROPHONE,
        ARROW_DOWN,
        FULL_SCREEN,
        MIX_VOLUMES,
        REMOVE_FROM,
        RIGHT_ARROW,
        QUIET_HOURS,
        FINGERPRINT,
        APPLICATION,
        CERTIFICATE,
        TRANSPARENT,
        IMAGE_EXPORT,
        SPEED_MEDIUM,
        LIBRARY_FILL,
        MUSIC_FOLDER,
        POWER_BUTTON,
        SKIP_FORWARD,
        CARE_UP_SOLID,
        ACCEPT_MEDIUM,
        CANCEL_MEDIUM,
        CHEVRON_RIGHT,
        CLIPPING_TOOL,
        SEARCH_MIRROR,
        SHOPPING_CART,
        FONT_INCREASE,
        BACK_TO_WINDOW,
        COMMAND_PROMPT,
        CLOUD_DOWNLOAD,
        DICTIONARY_ADD,
        CARE_DOWN_SOLID,
        CARE_LEFT_SOLID,
        CLEAR_SELECTION,
        DEVELOPER_TOOLS,
        BACKGROUND_FILL,
        CARE_RIGHT_SOLID,
        CHEVRON_DOWN_MED,
        CHEVRON_RIGHT_MED,
        EMOJI_TAB_SYMBOLS,
        EXPRESSIVE_INPUT_ENTRY,
        NONE
    };
    Q_ENUM(IconType)

    static QIcon SvgIcon(const QString& fillPath, const QString& baseName,
                         const QString& lightSuffix, const QString& darkSuffix);

    static QIcon FluentIcon(Icon::IconType icon);

    static void drawSvgIcon(QPainter *painter, Icon::IconType icon,
                            const QRectF &rect, const QMap<QString, QString> &attributes = {});

    static void drawSvgIcon(QPainter *painter, const QString& fillPath, const QString& baseName,
                            const QString& lightSuffix, const QString& darkSuffix,
                            const QRectF &rect, const QMap<QString, QString> &attributes = {});


private:
    static QString toString(Icon::IconType icon);

    static QString writeSvg(const QString &iconPath,
                            const QMap<QString, QString> &attributes = {},
                            const QList<int> &indexes = {});

    static void setAttributesOnPath(QDomDocument &doc, const QMap<QString, QString> &attrs, const QList<int> &idxs);

};




class QFLUENT_EXPORT Action : public QAction
{
    Q_OBJECT

public:
    explicit Action(QObject* parent = nullptr);
    explicit Action(const QString& text, QObject* parent = nullptr);
    Action(const QIcon& icon, const QString& text, QObject* parent = nullptr);
    Action(const Icon::IconType icon, const QString& text, QObject* parent = nullptr);
    ~Action() override;

    QIcon icon() const;

    void setIcon(const Icon::IconType icon);

private:
    QIcon m_Icon;

};

#endif // FLUENT_ICONS_H
