#pragma once

#include <QObject>

#include "Property.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define Q_BEGIN_ENUM_CREATE(CLASS) \
namespace CLASS                \
{                              \
        Q_NAMESPACE_EXPORT(QFLUENT_EXPORT)

#define Q_END_ENUM_CREATE(CLASS) }

#define Q_ENUM_CREATE(CLASS) Q_ENUM_NS(CLASS)
#else
#define Q_BEGIN_ENUM_CREATE(CLASS)          \
class QFLUENT_EXPORT CLASS : public QObject \
{                                       \
        Q_OBJECT                            \
    public:

#define Q_END_ENUM_CREATE(CLASS) \
             private:                         \
    Q_DISABLE_COPY(CLASS)        \
}                            \
    ;

#define Q_ENUM_CREATE(CLASS) Q_ENUM(CLASS)
#endif


    Q_BEGIN_ENUM_CREATE(ThemeType)
    enum ThemeMode
    {
        AUTO,
        LIGHT,
        DARK
    };
    Q_ENUM_CREATE(ThemeMode)


    enum ThemeColor
    {
        PRIMARY,
        DARK_1,
        DARK_2,
        DARK_3,
        LIGHT_1,
        LIGHT_2,
        LIGHT_3
    };
    Q_ENUM_CREATE(ThemeColor)


    enum class ThemeStyle {
        MENU,
        LABEL,
        PIVOT,
        BUTTON,
        DIALOG,
        SLIDER,
        INFO_BAR,
        SPIN_BOX,
        TAB_VIEW,
        TOOL_TIP,
        CHECK_BOX,
        COMBO_BOX,
        FLIP_VIEW,
        LINE_EDIT,
        LIST_VIEW,
        TREE_VIEW,
        INFO_BADGE,
        PIPS_PAGER,
        TABLE_VIEW,
        CARD_WIDGET,
        TIME_PICKER,
        COLOR_DIALOG,
        MEDIA_PLAYER,
        SETTING_CARD,
        TEACHING_TIP,
        FLUENT_WINDOW,
        SWITCH_BUTTON,
        MESSAGE_DIALOG,
        STATE_TOOL_TIP,
        CALENDAR_PICKER,
        FOLDER_LIST_DIALOG,
        SETTING_CARD_GROUP,
        EXPAND_SETTING_CARD,
        NAVIGATION_INTERFACE,
        TITLE_BAR,
        CUSTOM_STYLE_SHEET
    };
    Q_ENUM_CREATE(ThemeStyle)
    Q_END_ENUM_CREATE(ThemeType)


    Q_BEGIN_ENUM_CREATE(FluentIconType)
    enum IconType {
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
        NONE,
        CUSTOM_PATH = -1
    };
    Q_ENUM_CREATE(IconType)
    Q_END_ENUM_CREATE(FluentIconType)




    Q_BEGIN_ENUM_CREATE(CheckBoxType)
    enum CheckBoxState
    {
        NORMAL,
        HOVER,
        PRESSED,
        CHECKED,
        CHECKED_HOVER,
        CHECKED_PRESSED,
        DISABLED,
        CHECKED_DISABLED
    };
    Q_ENUM_CREATE(CheckBoxState)
    Q_END_ENUM_CREATE(CheckBoxType)




    Q_BEGIN_ENUM_CREATE(MenuAnimationType)
    enum MenuAnimation
    {
        NONE,
        DROP_DOWN,
        PULL_UP,
        FADE_IN_DROP_DOWN,
        FADE_IN_PULL_UP
    };
    Q_ENUM_CREATE(MenuAnimation)
    Q_END_ENUM_CREATE(MenuAnimationType)





    Q_BEGIN_ENUM_CREATE(MenuIndicatorType)
    enum MenuIndicator
    {
        CHECK = 0,
        RADIO = 1
    };
    Q_ENUM_CREATE(MenuIndicator)
    Q_END_ENUM_CREATE(MenuIndicatorType)


    Q_BEGIN_ENUM_CREATE(ApplicationType)
    enum WindowDisplayMode
    {
        Normal = 0x0000,
        DWMBlur = 0x0001,
        Acrylic,
        Mica,
        MicaAlt
    };
    Q_ENUM_CREATE(WindowDisplayMode)
    Q_END_ENUM_CREATE(ApplicationType)


    Q_BEGIN_ENUM_CREATE(AppBarType)
    enum ButtonType
    {
        NoneButtonHint = 0x0000,
        RouteBackButtonHint = 0x0001,
        IconButtonHint = 0x0002,
        IconLabelHint = 0x004,
        StayTopButtonHint = 0x0008,
        ThemeChangeButtonHint = 0x0010,
        MinimizeButtonHint = 0x00020,
        MaximizeButtonHint = 0x0040,
        CloseButtonHint = 0x0080,
        WindowTitleHint = 0x0100
    };
    Q_ENUM_CREATE(ButtonType)
    Q_DECLARE_FLAGS(ButtonFlags, ButtonType)
    Q_END_ENUM_CREATE(AppBarType)
    Q_DECLARE_OPERATORS_FOR_FLAGS(AppBarType::ButtonFlags)




    Q_BEGIN_ENUM_CREATE(InfoBarType)
    enum BarType
    {
        INFORMATION,
        SUCCESS,
        WARNING,
        ERROR
    };
    Q_ENUM_CREATE(BarType)


    enum BarPosition
    {
        TOP = 0,
        BOTTOM = 1,
        TOP_LEFT = 2,
        TOP_RIGHT = 3,
        BOTTOM_LEFT = 4,
        BOTTOM_RIGHT = 5,
        NONE = 6
    };
    Q_ENUM_CREATE(BarPosition)
    Q_END_ENUM_CREATE(InfoBarType)


    Q_BEGIN_ENUM_CREATE(PagiNationType)
    enum Alignment
    {
        Align_Left,
        Align_Right,
        Align_Center
    };
    Q_ENUM_CREATE(Alignment)
    Q_END_ENUM_CREATE(PagiNationType)




    Q_BEGIN_ENUM_CREATE(NavigationType)
    enum NavigationDisplayMode
    {
        MINIMAL = 0,
        COMPACT = 1,
        EXPAND = 2,
        MENU = 3
    };
    Q_ENUM_CREATE(NavigationDisplayMode)

    enum NavigationItemPosition
    {
        TOP = 0,
        SCROLL = 1,
        BOTTOM = 2
    };
    Q_ENUM_CREATE(NavigationItemPosition)
    Q_END_ENUM_CREATE(NavigationType)
