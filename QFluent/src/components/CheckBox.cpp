#include "CheckBox.h"
#include <QStyle>
#include <QRect>
#include <QPainterPath>


CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent) {
    sTheme->registerWidget(this, Theme::StyleSheetType::CHECK_BOX);

    isPressed = false;
    isHover = false;
}

CheckBox::CheckBox(const QString &text, QWidget *parent) : CheckBox(parent) {
    setText(text);

    sTheme->registerWidget(this, Theme::StyleSheetType::CHECK_BOX);
}

void CheckBox::mousePressEvent(QMouseEvent *event) {
    isPressed = true;
    QCheckBox::mousePressEvent(event);
}

void CheckBox::mouseReleaseEvent(QMouseEvent *event) {
    isPressed = false;
    QCheckBox::mouseReleaseEvent(event);
}

void CheckBox::enterEvent(QEnterEvent *event) {
    isHover = true;
    update();
    QCheckBox::enterEvent(event);
}

void CheckBox::leaveEvent(QEvent *event) {
    isHover = false;
    update();
    QCheckBox::leaveEvent(event);
}

QColor CheckBox::borderColor() const {
    if (sTheme->isDarkMode()) {
        switch (state()) {
        case CheckBoxState::NORMAL: return QColor(255, 255, 255, 141);
        case CheckBoxState::HOVER: return QColor(255, 255, 255, 141);
        case CheckBoxState::PRESSED: return QColor(255, 255, 255, 40);
        case CheckBoxState::CHECKED: return sTheme->themeColor();
        case CheckBoxState::CHECKED_HOVER: return sTheme->themeColor(Theme::ThemeColorType::DARK_1);
        case CheckBoxState::CHECKED_PRESSED: return sTheme->themeColor(Theme::ThemeColorType::DARK_2);
        case CheckBoxState::DISABLED: return QColor(255, 255, 255, 41);
        case CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 0);
        }
    } else {
        switch (state()) {
        case CheckBoxState::NORMAL: return QColor(0, 0, 0, 122);
        case CheckBoxState::HOVER: return QColor(0, 0, 0, 143);
        case CheckBoxState::PRESSED: return QColor(0, 0, 0, 69);
        case CheckBoxState::CHECKED: sTheme->themeColor();
        case CheckBoxState::CHECKED_HOVER: return sTheme->themeColor(Theme::ThemeColorType::LIGHT_1);
        case CheckBoxState::CHECKED_PRESSED: return sTheme->themeColor(Theme::ThemeColorType::LIGHT_2);
        case CheckBoxState::DISABLED: return QColor(0, 0, 0, 56);
        case CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 0);
        }
    }
    return QColor();
}

QColor CheckBox::backgroundColor() const {
    if (sTheme->isDarkMode()) {
        switch (state()) {
        case CheckBoxState::NORMAL: return QColor(0, 0, 0, 26);
        case CheckBoxState::HOVER: return QColor(255, 255, 255, 11);
        case CheckBoxState::PRESSED: return QColor(255, 255, 255, 18);
        case CheckBoxState::CHECKED: sTheme->themeColor();
        case CheckBoxState::CHECKED_HOVER: return sTheme->themeColor(Theme::ThemeColorType::DARK_1);
        case CheckBoxState::CHECKED_PRESSED: return sTheme->themeColor(Theme::ThemeColorType::DARK_2);
        case CheckBoxState::DISABLED: return QColor(0, 0, 0, 0);
        case CheckBoxState::CHECKED_DISABLED: return QColor(255, 255, 255, 41);
        }
    } else {
        switch (state()) {
        case CheckBoxState::NORMAL: return QColor(0, 0, 0, 6);
        case CheckBoxState::HOVER: return QColor(0, 0, 0, 13);
        case CheckBoxState::PRESSED: return QColor(0, 0, 0, 31);
        case CheckBoxState::CHECKED: sTheme->themeColor();
        case CheckBoxState::CHECKED_HOVER: return sTheme->themeColor(Theme::ThemeColorType::LIGHT_1);
        case CheckBoxState::CHECKED_PRESSED: return sTheme->themeColor(Theme::ThemeColorType::LIGHT_2);
        case CheckBoxState::DISABLED: return QColor(0, 0, 0, 0);
        case CheckBoxState::CHECKED_DISABLED: return QColor(0, 0, 0, 56);
        }
    }
    return QColor();
}

CheckBox::CheckBoxState CheckBox::state() const {
    if (!isEnabled()) {
        return isChecked() ? CheckBoxState::CHECKED_DISABLED : CheckBoxState::DISABLED;
    }

    if (isChecked()) {
        if (isPressed) return CheckBoxState::CHECKED_PRESSED;
        if (isHover) return CheckBoxState::CHECKED_HOVER;
        return CheckBoxState::CHECKED;
    } else {
        if (isPressed) return CheckBoxState::PRESSED;
        if (isHover) return CheckBoxState::HOVER;
        return CheckBoxState::NORMAL;
    }
}

void CheckBox::paintEvent(QPaintEvent *event) {
    QCheckBox::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 获取指示器矩形
    QStyleOptionButton opt;
    opt.initFrom(this);
    QRect rect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, this);

    // 绘制形状：圆角矩形
    painter.setPen(borderColor());
    painter.setBrush(backgroundColor());
    painter.drawRoundedRect(rect, 4.5, 4.5);

    if (!isEnabled()) {
        painter.setOpacity(0.8);
    }


    if (checkState() == Qt::Checked) {
        Icon::drawSvgIcon(&painter, Icon::IconType::ACCEPT, rect.adjusted(3, 3, -3, -3));
    } else if (checkState() == Qt::PartiallyChecked) {

    }
}
