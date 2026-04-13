#pragma once

#include <QObject>

#if defined(QFLUENT_LIBRARY)
#  define QFLUENT_EXPORT Q_DECL_EXPORT
#else
#  define QFLUENT_EXPORT Q_DECL_IMPORT
#endif

namespace Fluent {
Q_NAMESPACE_EXPORT(QFLUENT_EXPORT)

// ========== 主题相关 ==========
enum class ThemeMode {
    AUTO,
    LIGHT,
    DARK
};
Q_ENUM_NS(ThemeMode)

enum class ThemeColor {
    PRIMARY,
    DARK_1,
    DARK_2,
    DARK_3,
    LIGHT_1,
    LIGHT_2,
    LIGHT_3
};
Q_ENUM_NS(ThemeColor)

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
    LINE_EDIT,
    LIST_VIEW,
    TABLE_VIEW,
    CARD_WIDGET,
    TIME_PICKER,
    COLOR_DIALOG,
    SETTING_CARD,
    TEACHING_TIP,
    SWITCH_BUTTON,
    MESSAGE_DIALOG,
    CALENDAR_PICKER,
    SETTING_CARD_GROUP,
    EXPAND_SETTING_CARD,
    NAVIGATION_INTERFACE
};
Q_ENUM_NS(ThemeStyle)

// ========== 图标相关 ==========
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
    FRIGID,
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
Q_ENUM_NS(IconType)

// ========== 菜单相关 ==========
enum class MenuAnimation {
    NONE,
    DROP_DOWN,
    PULL_UP,
    FADE_IN_DROP_DOWN,
    FADE_IN_PULL_UP
};
Q_ENUM_NS(MenuAnimation)

enum class MenuIndicator {
    CHECK = 0,
    RADIO = 1
};
Q_ENUM_NS(MenuIndicator)



// ========== 分页相关 ==========
enum class Alignment {
    Align_Left,
    Align_Right,
    Align_Center
};
Q_ENUM_NS(Alignment)

} // namespace Fluent


#ifndef FLUENT_QHASH_FUNCTIONS_DEFINED
#define FLUENT_QHASH_FUNCTIONS_DEFINED

#include <QtCore/qhashfunctions.h>
#include <type_traits>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using FluentQHashReturnType = size_t;
#else
    using FluentQHashReturnType = uint;
#endif

inline FluentQHashReturnType qHash(const Fluent::IconType key, FluentQHashReturnType seed = 0) noexcept
{
    return ::qHash(static_cast<std::underlying_type_t<Fluent::IconType>>(key), seed);
}

inline FluentQHashReturnType qHash(const Fluent::ThemeMode key, FluentQHashReturnType seed = 0) noexcept
{
    return ::qHash(static_cast<std::underlying_type_t<Fluent::ThemeMode>>(key), seed);
}

inline FluentQHashReturnType qHash(const Fluent::ThemeColor key, FluentQHashReturnType seed = 0) noexcept
{
    return ::qHash(static_cast<std::underlying_type_t<Fluent::ThemeColor>>(key), seed);
}

inline FluentQHashReturnType qHash(const Fluent::ThemeStyle key, FluentQHashReturnType seed = 0) noexcept
{
    return ::qHash(static_cast<std::underlying_type_t<Fluent::ThemeStyle>>(key), seed);
}

#endif // FLUENT_QHASH_FUNCTIONS_DEFINED

