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

    if (m_iconType != FluentIconType::IconType::NONE) {
        FluentIcon(m_iconType).render(&painter, QRectF(x, y, w, h));
    } else {
        FluentIcon(m_templatePath).render(&painter, QRectF(x, y, w, h));
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
