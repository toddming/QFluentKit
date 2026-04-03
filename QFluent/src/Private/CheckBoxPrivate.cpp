#include "CheckBoxPrivate.h"

#include <QColor>

#include "Theme.h"
#include "QFluent/CheckBox.h"

QColor CheckBoxPrivate::borderColor() const {
    if (Theme::instance()->isDarkTheme()) {
        switch (state()) {
        case CheckBox::State::NORMAL: return QColor(255, 255, 255, 141);
        case CheckBox::State::HOVER: return QColor(255, 255, 255, 141);
        case CheckBox::State::PRESSED: return QColor(255, 255, 255, 40);
        case CheckBox::State::CHECKED: return Theme::instance()->themeColor();
        case CheckBox::State::CHECKED_HOVER: return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_1);
        case CheckBox::State::CHECKED_PRESSED: return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_2);
        case CheckBox::State::DISABLED: return QColor(255, 255, 255, 41);
        case CheckBox::State::CHECKED_DISABLED: return QColor(0, 0, 0, 0);
        }
    } else {
        switch (state()) {
        case CheckBox::State::NORMAL: return QColor(0, 0, 0, 122);
        case CheckBox::State::HOVER: return QColor(0, 0, 0, 143);
        case CheckBox::State::PRESSED: return QColor(0, 0, 0, 69);
        case CheckBox::State::CHECKED: Theme::instance()->themeColor();
        case CheckBox::State::CHECKED_HOVER: return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_1);
        case CheckBox::State::CHECKED_PRESSED: return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_2);
        case CheckBox::State::DISABLED: return QColor(0, 0, 0, 56);
        case CheckBox::State::CHECKED_DISABLED: return QColor(0, 0, 0, 0);
        }
    }
    return QColor();
}

QColor CheckBoxPrivate::backgroundColor() const {
    if (Theme::instance()->isDarkTheme()) {
        switch (state()) {
        case CheckBox::State::NORMAL: return QColor(0, 0, 0, 26);
        case CheckBox::State::HOVER: return QColor(255, 255, 255, 11);
        case CheckBox::State::PRESSED: return QColor(255, 255, 255, 18);
        case CheckBox::State::CHECKED: Theme::instance()->themeColor();
        case CheckBox::State::CHECKED_HOVER: return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_1);
        case CheckBox::State::CHECKED_PRESSED: return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_2);
        case CheckBox::State::DISABLED: return QColor(0, 0, 0, 0);
        case CheckBox::State::CHECKED_DISABLED: return QColor(255, 255, 255, 41);
        }
    } else {
        switch (state()) {
        case CheckBox::State::NORMAL: return QColor(0, 0, 0, 6);
        case CheckBox::State::HOVER: return QColor(0, 0, 0, 13);
        case CheckBox::State::PRESSED: return QColor(0, 0, 0, 31);
        case CheckBox::State::CHECKED: Theme::instance()->themeColor();
        case CheckBox::State::CHECKED_HOVER: return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_1);
        case CheckBox::State::CHECKED_PRESSED: return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_2);
        case CheckBox::State::DISABLED: return QColor(0, 0, 0, 0);
        case CheckBox::State::CHECKED_DISABLED: return QColor(0, 0, 0, 56);
        }
    }
    return QColor();
}

CheckBox::State CheckBoxPrivate::state() const {
    Q_Q(const CheckBox);
    if (!q->isEnabled()) {
        return q->isChecked() ? CheckBox::State::CHECKED_DISABLED : CheckBox::State::DISABLED;
    }

    if (q->isChecked()) {
        if (m_isPressed) return CheckBox::State::CHECKED_PRESSED;
        if (m_isHover) return CheckBox::State::CHECKED_HOVER;
        return CheckBox::State::CHECKED;
    } else {
        if (m_isPressed) return CheckBox::State::PRESSED;
        if (m_isHover) return CheckBox::State::HOVER;
        return CheckBox::State::NORMAL;
    }
}
