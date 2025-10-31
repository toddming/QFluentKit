#include "ToolButton.h"
#include <QPainter>
#include <QStyleOptionButton>
#include <QApplication>
#include <QTimer>
#include <QSize>
#include <QIcon>
#include <QMouseEvent>
#include <QPaintEvent>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"


ToolButton::ToolButton(QWidget* parent)
    : QToolButton(parent)
    , m_isPressed(false)
    , m_isHover(false)
{
    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::BUTTON);

    setIcon(FluentIconType::IconType::NONE);

    QTimer::singleShot(0, this, [this]() {
        postInit();
    });
}

ToolButton::ToolButton(FluentIconType::IconType icon, QWidget* parent)
    : ToolButton(parent)
{
    setIcon(icon);
}

ToolButton::ToolButton(const QString& templatePath, QWidget* parent)
    : ToolButton(parent)
{
    setIcon(templatePath);
}

ToolButton::ToolButton(QIcon icon, QWidget* parent)
    : ToolButton(parent)
{
    QToolButton::setIcon(icon);
}

void ToolButton::setIcon(FluentIconType::IconType icon)
{
    m_iconType = icon;
    update();
}

void ToolButton::setIcon(const QString& templatePath)
{
    m_templatePath = templatePath;
    m_iconType = FluentIconType::IconType::NONE;

    update();
}

void ToolButton::mousePressEvent(QMouseEvent* event)
{
    m_isPressed = true;
    QToolButton::mousePressEvent(event);
}

void ToolButton::mouseReleaseEvent(QMouseEvent* event)
{
    m_isPressed = false;
    QToolButton::mouseReleaseEvent(event);
}

void ToolButton::enterEvent(QEnterEvent* event)
{
    m_isHover = true;
    update();
    QToolButton::enterEvent(event);
}

void ToolButton::leaveEvent(QEvent* event)
{
    m_isHover = false;
    update();
    QToolButton::leaveEvent(event);
}

void ToolButton::paintEvent(QPaintEvent* event)
{
    QToolButton::paintEvent(event);

    if (m_iconType == FluentIconType::IconType::NONE && m_templatePath.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (!isEnabled()) {
        painter.setOpacity(0.43);
    } else if (m_isPressed) {
        painter.setOpacity(0.63);
    }

    int w = iconSize().width();
    int h = iconSize().height();
    int y = (height() - h) / 2;
    int x = (width() - w) / 2;

    drawIcon(&painter, QRectF(x, y, w, h));
}

void PrimaryToolButton::drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme)
{
    if (iconType() != FluentIconType::IconType::NONE) {
        FluentIcon(iconType()).render(painter, rect, Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
    } else {
        FluentIcon(templatePath()).render(painter, rect, Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
    }
}



void ToolButton::postInit()
{

}

void ToolButton::drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme)
{
    if (m_iconType != FluentIconType::IconType::NONE) {
        FluentIcon(m_iconType).render(painter, rect, theme);
    } else {
        FluentIcon(m_templatePath).render(painter, rect, theme);
    }
}


void ToggleToolButton::drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme)
{
    if (iconType() != FluentIconType::IconType::NONE) {
        if (Theme::instance()->isDarkTheme()) {
            FluentIcon(iconType()).render(painter, rect, isChecked() ? ThemeType::DARK : ThemeType::LIGHT);
        } else {
            FluentIcon(iconType()).render(painter, rect, isChecked() ? ThemeType::LIGHT : ThemeType::DARK);
        }
    } else {
        if (Theme::instance()->isDarkTheme()) {
            FluentIcon(templatePath()).render(painter, rect, isChecked() ? ThemeType::DARK : ThemeType::LIGHT);
        } else {
            FluentIcon(templatePath()).render(painter, rect, isChecked() ? ThemeType::LIGHT : ThemeType::DARK);
        }
    }
}


void ToggleToolButton::postInit()
{
    setCheckable(true);
    setChecked(false);
}


void PillToolButton::paintEvent(QPaintEvent* event)
{

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool isDark = Theme::instance()->isDarkTheme();

    QRect rect;
    QColor borderColor;
    QColor bgColor;

    if (!isChecked()) {
        rect = this->rect().adjusted(1, 1, -1, -1);
        borderColor = isDark ? QColor(255, 255, 255, 18) : QColor(0, 0, 0, 15);

        if (!isEnabled()) {
            bgColor = isDark ? QColor(255, 255, 255, 11) : QColor(249, 249, 249, 75);
        } else if (isPressed() || isHover()) {
            bgColor = isDark ? QColor(255, 255, 255, 21) : QColor(249, 249, 249, 128);
        } else {
            bgColor = isDark ? QColor(255, 255, 255, 15) : QColor(243, 243, 243, 194);
        }
    } else {
        if (!isEnabled()) {
            bgColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
        } else if (isPressed()) {
            bgColor =  Theme::instance()->themeColor(isDark ? ThemeType::DARK_2 : ThemeType::LIGHT_3);
        } else if (isHover()) {
            bgColor =  Theme::instance()->themeColor(isDark ? ThemeType::DARK_1 : ThemeType::LIGHT_1);
        } else {
            bgColor = Theme::instance()->themeColor();
        }

        borderColor = Qt::transparent;
        rect = this->rect();
    }

    painter.setPen(borderColor);
    painter.setBrush(bgColor);

    int r = rect.height() / 2;
    painter.drawRoundedRect(rect, r, r);

    ToggleToolButton::paintEvent(event);
}
