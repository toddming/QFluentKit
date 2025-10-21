#include "RadioButton.h"
#include <QPainterPath>
#include <QRectF>
#include <QColor>
#include <QPainter>
#include <QPoint>
#include <QEvent>
#include <QEnterEvent>

#include "Theme.h"
#include "StyleSheet.h"


RadioButton::RadioButton(QWidget *parent)
    : QRadioButton(parent), indicatorPos_(11, 12), isHover_(false)
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

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::BUTTON);
}

void RadioButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    drawIndicator(painter);
    drawText(painter);
}

void RadioButton::enterEvent(QEnterEvent *event)
{
    isHover_ = true;
    update();
    QRadioButton::enterEvent(event);
}

void RadioButton::leaveEvent(QEvent *event)
{
    isHover_ = false;
    update();
    QRadioButton::leaveEvent(event);
}

void RadioButton::drawText(QPainter &painter)
{
    if (!isEnabled()) {
        painter.setOpacity(0.36);
    }

    painter.setFont(font());
    painter.setPen(Theme::instance()->isDarkTheme() ? Qt::white : Qt::black);
    painter.drawText(QRect(29, 0, width(), height()), Qt::AlignVCenter, text());
}

void RadioButton::drawIndicator(QPainter &painter)
{
    bool isDark = Theme::instance()->isDarkTheme();

    QColor borderColor, filledColor;

    if (isChecked()) {
        if (isEnabled()) {
            borderColor = Theme::instance()->themeColor();
        } else {
            borderColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
        }

        filledColor = isDark ? Qt::black : Qt::white;

        if (isHover_ && !isDown()) {
            drawCircle(painter, indicatorPos_, 10, 4, borderColor, filledColor);
        } else {
            drawCircle(painter, indicatorPos_, 10, 5, borderColor, filledColor);
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
            } else if (isHover_) {
                filledColor = isDark ? QColor(255, 255, 255, 11) : QColor(0, 0, 0, 15);
            } else {
                filledColor = isDark ? QColor(0, 0, 0, 26) : QColor(0, 0, 0, 6);
            }
        } else {
            filledColor = Qt::transparent;
            borderColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
        }

        drawCircle(painter, indicatorPos_, 10, 1, borderColor, filledColor);

        if (isEnabled() && isDown()) {
            borderColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 24);
            drawCircle(painter, indicatorPos_, 9, 4, borderColor, Qt::transparent);
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


