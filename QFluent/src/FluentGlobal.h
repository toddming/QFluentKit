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
Q_ENUM_NS(IconType)

// ========== 复选框相关 ==========
enum class CheckBoxState {
    NORMAL,
    HOVER,
    PRESSED,
    CHECKED,
    CHECKED_HOVER,
    CHECKED_PRESSED,
    DISABLED,
    CHECKED_DISABLED
};
Q_ENUM_NS(CheckBoxState)

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

// ========== 窗口相关 ==========
enum class WindowEffect {
    Normal = 0x0000,
    DWMBlur = 0x0001,
    Acrylic,
    Mica,
    MicaAlt
};
Q_ENUM_NS(WindowEffect)

// ========== AppBar 按钮类型 ==========
enum WindowButtonHint {
    None               = 0x0000,
    RouteBack          = 0x0001,
    Icon               = 0x0002,
    IconLabel          = 0x0004,
    StayOnTop          = 0x0008,
    ThemeToggle        = 0x0010,
    Minimize           = 0x0020,
    Maximize           = 0x0040,
    Close              = 0x0080,
    Title              = 0x0100
};
Q_ENUM_NS(WindowButtonHint)
Q_DECLARE_FLAGS(WindowButtonHints, WindowButtonHint)

// ========== 信息栏相关 ==========
enum class MessageType {
    INFORMATION,
    SUCCESS,
    WARNING,
    ERROR
};
Q_ENUM_NS(MessageType)

enum class MessagePosition {
    TOP = 0,
    BOTTOM = 1,
    TOP_LEFT = 2,
    TOP_RIGHT = 3,
    BOTTOM_LEFT = 4,
    BOTTOM_RIGHT = 5,
    NONE = 6
};
Q_ENUM_NS(MessagePosition)

// ========== 分页相关 ==========
enum class Alignment {
    Align_Left,
    Align_Right,
    Align_Center
};
Q_ENUM_NS(Alignment)

// ========== 导航相关 ==========
enum class NavigationDisplayMode {
    MINIMAL = 0,
    COMPACT = 1,
    EXPAND = 2,
    MENU = 3
};
Q_ENUM_NS(NavigationDisplayMode)

enum class NavigationItemPosition {
    TOP = 0,
    SCROLL = 1,
    BOTTOM = 2
};
Q_ENUM_NS(NavigationItemPosition)

enum class SmoothMode {
    NO_SMOOTH = 0,
    CONSTANT = 1,
    LINEAR = 2,
    QUADRATI = 3,
    COSINE = 4
};
Q_ENUM_NS(SmoothMode)

enum class ScrollBarHandleDisplayMode {
    ALWAYS = 0,
    ON_HOVER = 1
};
Q_ENUM_NS(ScrollBarHandleDisplayMode)

} // namespace Fluent

Q_DECLARE_OPERATORS_FOR_FLAGS(Fluent::WindowButtonHints)
