#include "RadioButton.h"
#include <QPainterPath>
#include <QRectF>
#include <QColor>
#include <QPainter>
#include <QPoint>
#include <QEvent>

#include "Theme.h"
#include "StyleSheet.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif


RadioButton::RadioButton(QWidget *parent)
    : QRadioButton(parent), m_indicatorPos(11, 12), m_isHover(false)
{
    init();
}

RadioButton::RadioButton(const QString &text, QWidget *parent)
    : RadioButton(parent)
{
    setText(text);
}

void RadioButton::init()
{
    setAttribute(Qt::WA_MacShowFocusRect, false);

    StyleSheet::registerWidget(this, Fluent::ThemeStyle::BUTTON);
}

void RadioButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    drawIndicator(painter);
    drawText(painter);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RadioButton::enterEvent(QEnterEvent *event)
{
    m_isHover = true;
    update();
    QRadioButton::enterEvent(event);
}
#else
void RadioButton::enterEvent(QEvent *event)
{
    m_isHover = true;
    update();
    QRadioButton::enterEvent(event);
}
#endif

void RadioButton::leaveEvent(QEvent *event)
{
    m_isHover = false;
    update();
    QRadioButton::leaveEvent(event);
}

void RadioButton::drawText(QPainter &painter)
{
    if (!isEnabled()) {
        painter.setOpacity(0.36);
    }

    painter.setFont(font());
    painter.setPen(Theme::isDark() ? Qt::white : Qt::black);
    int textX = m_indicatorPos.x() + 10 + 8;
    painter.drawText(QRect(textX, 0, width(), height()), Qt::AlignVCenter, text());
}

void RadioButton::drawIndicator(QPainter &painter)
{
    bool isDark = Theme::isDark();

    QColor borderColor, filledColor;

    if (isChecked()) {
        if (isEnabled()) {
            borderColor = Theme::themeColor(Fluent::ThemeColor::PRIMARY);
        } else {
            borderColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
        }

        filledColor = isDark ? Qt::black : Qt::white;

        if (m_isHover && !isDown()) {
            drawCircle(painter, m_indicatorPos, 10, 4, borderColor, filledColor);
        } else {
            drawCircle(painter, m_indicatorPos, 10, 5, borderColor, filledColor);
        }
    } else {
        if (isEnabled()) {
            if (!isDown()) {
                borderColor = isDark ? QColor(255, 255, 255, 153) : QColor(0, 0, 0, 153);
            } else {
                borderColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
            }

            if (isDown()) {
                filledColor = isDark ? Qt::black : Qt::white;
            } else if (m_isHover) {
                filledColor = isDark ? QColor(255, 255, 255, 11) : QColor(0, 0, 0, 15);
            } else {
                filledColor = isDark ? QColor(0, 0, 0, 26) : QColor(0, 0, 0, 6);
            }
        } else {
            filledColor = Qt::transparent;
            borderColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
        }

        drawCircle(painter, m_indicatorPos, 10, 1, borderColor, filledColor);

        if (isEnabled() && isDown()) {
            borderColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 24);
            drawCircle(painter, m_indicatorPos, 9, 4, borderColor, Qt::transparent);
        }
    }
}

void RadioButton::drawCircle(QPainter &painter, const QPoint &center, int radius, int thickness, const QColor &borderColor, const QColor &filledColor)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    // Outer circle (border)
    QRectF outerRect(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius);
    path.addEllipse(outerRect);

    // Inner circle (filled)
    int ir = radius - thickness;
    QRectF innerRect(center.x() - ir, center.y() - ir, 2 * ir, 2 * ir);
    QPainterPath innerPath;
    innerPath.addEllipse(innerRect);

    path = path.subtracted(innerPath);

    // Draw outer ring
    painter.setPen(Qt::NoPen);
    painter.fillPath(path, borderColor);

    // Fill inner circle
    painter.fillPath(innerPath, filledColor);
}


