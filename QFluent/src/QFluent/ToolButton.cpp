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
#include "Icon.h"


ToolButton::ToolButton(QWidget* parent)
    : QToolButton(parent)
    , m_isPressed(false)
    , m_isHover(false)
{
    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::BUTTON);

    setIcon(IconType::FLuentIcon::NONE);

    QTimer::singleShot(0, this, [this]() {
        postInit();
    });
}

ToolButton::ToolButton(IconType::FLuentIcon icon, QWidget* parent)
    : ToolButton(parent)
{
    setIcon(icon);
}

ToolButton::ToolButton(const QString& fillPath, const QString& baseName, const QString& lightSuffix, const QString& darkSuffix,
                       QWidget* parent)
    : ToolButton(parent)
{
    setIcon(fillPath, baseName, lightSuffix, darkSuffix);
}

void ToolButton::setIcon(IconType::FLuentIcon icon)
{
    m_iconType = icon;
    update();
}

void ToolButton::setIcon(const QString& fillPath, const QString& baseName, const QString& lightSuffix, const QString& darkSuffix)
{
    m_fillPath = fillPath;
    m_baseName = baseName;
    m_lightSuffix = lightSuffix;
    m_darkSuffix = darkSuffix;
    m_iconType = IconType::FLuentIcon::NONE;

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

    if (m_iconType == IconType::FLuentIcon::NONE && m_baseName.isEmpty()) {
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

    if (m_iconType != IconType::FLuentIcon::NONE) {
        Icon::drawSvgIcon(&painter, m_iconType, QRectF(x, y, w, h));
    } else {
        Icon::drawSvgIcon(&painter, m_fillPath, m_baseName, m_lightSuffix, m_darkSuffix, QRectF(x, y, w ,h));
    }
}



void ToolButton::postInit()
{

}


void ToggleToolButton::postInit()
{
    setCheckable(true);
    setChecked(false);
}
