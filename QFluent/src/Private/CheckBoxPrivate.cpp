#include "CheckBoxPrivate.h"

#include <QColor>

#include "Theme.h"
#include "QFluent/CheckBox.h"

QColor CheckBoxPrivate::borderColor()  {
    if (Theme::instance()->isDarkTheme()) {
        switch (state()) {
        case Fluent::CheckBoxState::NORMAL: return QColor(255, 255, 255, 141);
        case Fluent::CheckBoxState::HOVER: return QColor(255, 255, 255, 141);
        case Fluent::CheckBoxState::PRESSED: return QColor(255, 255, 255, 40);
        case Fluent::CheckBoxState::CHECKED: return Theme::instance()->themeColor();
        case Fluent::CheckBoxState::CHECKED_HOVER: return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_1);
        case Fluent::CheckBoxState::CHECKED_PRESSED: return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_2);
        case Fluent::CheckBoxState::DISABLED: return QColor(255, 255, 255, 41);
        case Fluent::CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 0);
        }
    } else {
        switch (state()) {
        case Fluent::CheckBoxState::NORMAL: return QColor(0, 0, 0, 122);
        case Fluent::CheckBoxState::HOVER: return QColor(0, 0, 0, 143);
        case Fluent::CheckBoxState::PRESSED: return QColor(0, 0, 0, 69);
        case Fluent::CheckBoxState::CHECKED: Theme::instance()->themeColor();
        case Fluent::CheckBoxState::CHECKED_HOVER: return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_1);
        case Fluent::CheckBoxState::CHECKED_PRESSED: return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_2);
        case Fluent::CheckBoxState::DISABLED: return QColor(0, 0, 0, 56);
        case Fluent::CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 0);
        }
    }
    return QColor();
}

QColor CheckBoxPrivate::backgroundColor() {
    if (Theme::instance()->isDarkTheme()) {
        switch (state()) {
        case Fluent::CheckBoxState::NORMAL: return QColor(0, 0, 0, 26);
        case Fluent::CheckBoxState::HOVER: return QColor(255, 255, 255, 11);
        case Fluent::CheckBoxState::PRESSED: return QColor(255, 255, 255, 18);
        case Fluent::CheckBoxState::CHECKED: Theme::instance()->themeColor();
        case Fluent::CheckBoxState::CHECKED_HOVER: return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_1);
        case Fluent::CheckBoxState::CHECKED_PRESSED: return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_2);
        case Fluent::CheckBoxState::DISABLED: return QColor(0, 0, 0, 0);
        case Fluent::CheckBoxState::CHECKED_DISABLED: return QColor(255, 255, 255, 41);
        }
    } else {
        switch (state()) {
        case Fluent::CheckBoxState::NORMAL: return QColor(0, 0, 0, 6);
        case Fluent::CheckBoxState::HOVER: return QColor(0, 0, 0, 13);
        case Fluent::CheckBoxState::PRESSED: return QColor(0, 0, 0, 31);
        case Fluent::CheckBoxState::CHECKED: Theme::instance()->themeColor();
        case Fluent::CheckBoxState::CHECKED_HOVER: return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_1);
        case Fluent::CheckBoxState::CHECKED_PRESSED: return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_2);
        case Fluent::CheckBoxState::DISABLED: return QColor(0, 0, 0, 0);
        case Fluent::CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 56);
        }
    }
    return QColor();
}

Fluent::CheckBoxState CheckBoxPrivate::state() {
    Q_Q(CheckBox);
    if (!q->isEnabled()) {
        return q->isChecked() ? Fluent::CheckBoxState::CHECKED_DISABLED : Fluent::CheckBoxState::DISABLED;
    }

    if (q->isChecked()) {
        if (isPressed) return Fluent::CheckBoxState::CHECKED_PRESSED;
        if (isHover) return Fluent::CheckBoxState::CHECKED_HOVER;
        return Fluent::CheckBoxState::CHECKED;
    } else {
        if (isPressed) return Fluent::CheckBoxState::PRESSED;
        if (isHover) return Fluent::CheckBoxState::HOVER;
        return Fluent::CheckBoxState::NORMAL;
    }
}
