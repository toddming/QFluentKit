#include "CheckBoxPrivate.h"

#include <QColor>

#include "Theme.h"
#include "QFluent/CheckBox.h"

QColor CheckBoxPrivate::borderColor()  {
    if (Theme::instance()->isDarkTheme()) {
        switch (state()) {
        case CheckBoxType::CheckBoxState::NORMAL: return QColor(255, 255, 255, 141);
        case CheckBoxType::CheckBoxState::HOVER: return QColor(255, 255, 255, 141);
        case CheckBoxType::CheckBoxState::PRESSED: return QColor(255, 255, 255, 40);
        case CheckBoxType::CheckBoxState::CHECKED: return Theme::instance()->themeColor();
        case CheckBoxType::CheckBoxState::CHECKED_HOVER: return Theme::instance()->themeColor(ThemeType::ThemeColor::DARK_1);
        case CheckBoxType::CheckBoxState::CHECKED_PRESSED: return Theme::instance()->themeColor(ThemeType::ThemeColor::DARK_2);
        case CheckBoxType::CheckBoxState::DISABLED: return QColor(255, 255, 255, 41);
        case CheckBoxType::CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 0);
        }
    } else {
        switch (state()) {
        case CheckBoxType::CheckBoxState::NORMAL: return QColor(0, 0, 0, 122);
        case CheckBoxType::CheckBoxState::HOVER: return QColor(0, 0, 0, 143);
        case CheckBoxType::CheckBoxState::PRESSED: return QColor(0, 0, 0, 69);
        case CheckBoxType::CheckBoxState::CHECKED: Theme::instance()->themeColor();
        case CheckBoxType::CheckBoxState::CHECKED_HOVER: return Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_1);
        case CheckBoxType::CheckBoxState::CHECKED_PRESSED: return Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_2);
        case CheckBoxType::CheckBoxState::DISABLED: return QColor(0, 0, 0, 56);
        case CheckBoxType::CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 0);
        }
    }
    return QColor();
}

QColor CheckBoxPrivate::backgroundColor() {
    if (Theme::instance()->isDarkTheme()) {
        switch (state()) {
        case CheckBoxType::CheckBoxState::NORMAL: return QColor(0, 0, 0, 26);
        case CheckBoxType::CheckBoxState::HOVER: return QColor(255, 255, 255, 11);
        case CheckBoxType::CheckBoxState::PRESSED: return QColor(255, 255, 255, 18);
        case CheckBoxType::CheckBoxState::CHECKED: Theme::instance()->themeColor();
        case CheckBoxType::CheckBoxState::CHECKED_HOVER: return Theme::instance()->themeColor(ThemeType::ThemeColor::DARK_1);
        case CheckBoxType::CheckBoxState::CHECKED_PRESSED: return Theme::instance()->themeColor(ThemeType::ThemeColor::DARK_2);
        case CheckBoxType::CheckBoxState::DISABLED: return QColor(0, 0, 0, 0);
        case CheckBoxType::CheckBoxState::CHECKED_DISABLED: return QColor(255, 255, 255, 41);
        }
    } else {
        switch (state()) {
        case CheckBoxType::CheckBoxState::NORMAL: return QColor(0, 0, 0, 6);
        case CheckBoxType::CheckBoxState::HOVER: return QColor(0, 0, 0, 13);
        case CheckBoxType::CheckBoxState::PRESSED: return QColor(0, 0, 0, 31);
        case CheckBoxType::CheckBoxState::CHECKED: Theme::instance()->themeColor();
        case CheckBoxType::CheckBoxState::CHECKED_HOVER: return Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_1);
        case CheckBoxType::CheckBoxState::CHECKED_PRESSED: return Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_2);
        case CheckBoxType::CheckBoxState::DISABLED: return QColor(0, 0, 0, 0);
        case CheckBoxType::CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 56);
        }
    }
    return QColor();
}

CheckBoxType::CheckBoxState CheckBoxPrivate::state() {
    Q_Q(CheckBox);
    if (!q->isEnabled()) {
        return q->isChecked() ? CheckBoxType::CheckBoxState::CHECKED_DISABLED : CheckBoxType::CheckBoxState::DISABLED;
    }

    if (q->isChecked()) {
        if (_isPressed) return CheckBoxType::CheckBoxState::CHECKED_PRESSED;
        if (_isHover) return CheckBoxType::CheckBoxState::CHECKED_HOVER;
        return CheckBoxType::CheckBoxState::CHECKED;
    } else {
        if (_isPressed) return CheckBoxType::CheckBoxState::PRESSED;
        if (_isHover) return CheckBoxType::CheckBoxState::HOVER;
        return CheckBoxType::CheckBoxState::NORMAL;
    }
}
