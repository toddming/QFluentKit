#include "CardWidget.h"
#include <QPainter>
#include <QPainterPath>

#include "Theme.h"
#include "StyleSheet.h"

CardWidget::CardWidget(QWidget *parent)
    : BackgroundAnimationWidget(parent)
    , m_isClickEnabled(false)
    , m_borderRadius(5)
{
    setBackgroundColor(normalBackgroundColor());
    Theme::onThemeModeChanged(this, [this](Fluent::ThemeMode) {
        updateBackgroundColor();
    });
}

void CardWidget::mouseReleaseEvent(QMouseEvent *e)
{
    BackgroundAnimationWidget::mouseReleaseEvent(e);
    if (m_isClickEnabled) {
        emit clicked();
    }
}

void CardWidget::setClickEnabled(bool isEnabled)
{
    m_isClickEnabled = isEnabled;
    update();
}

bool CardWidget::isClickEnabled() const
{
    return m_isClickEnabled;
}

QColor CardWidget::normalBackgroundColor() const
{
    return QColor(255, 255, 255, Theme::isDark() ? 13 : 170);
}

QColor CardWidget::hoverBackgroundColor() const
{
    return QColor(255, 255, 255, Theme::isDark() ? 21 : 64);
}

QColor CardWidget::pressedBackgroundColor() const
{
    return QColor(255, 255, 255, Theme::isDark() ? 8 : 64);
}

int CardWidget::borderRadius() const
{
    return m_borderRadius;
}

void CardWidget::setBorderRadius(int radius)
{
    m_borderRadius = radius;
    update();
}

void CardWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    int w = width();
    int h = height();
    int r = m_borderRadius;
    int d = 2 * r;

    bool isDark = Theme::isDark();

    // 绘制顶部边框
    QPainterPath path;
    path.arcMoveTo(1, h - d - 1, d, d, 240);
    path.arcTo(1, h - d - 1, d, d, 225, -60);
    path.lineTo(1, r);
    path.arcTo(1, 1, d, d, -180, -90);
    path.lineTo(w - r, 1);
    path.arcTo(w - d - 1, 1, d, d, 90, -90);
    path.lineTo(w - 1, h - r);
    path.arcTo(w - d - 1, h - d - 1, d, d, 0, -60);

    QColor topBorderColor(0, 0, 0, 20);
    if (isDark) {
        if (this->isPressed()) {
            topBorderColor = QColor(255, 255, 255, 18);
        } else if (isHover()) {
            topBorderColor = QColor(255, 255, 255, 13);
        }
    } else {
        topBorderColor = QColor(0, 0, 0, 15);
    }

    painter.strokePath(path, topBorderColor);

    // 绘制底部边框
    path = QPainterPath();
    path.arcMoveTo(1, h - d - 1, d, d, 240);
    path.arcTo(1, h - d - 1, d, d, 240, 30);
    path.lineTo(w - r - 1, h - 1);
    path.arcTo(w - d - 1, h - d - 1, d, d, 270, 30);

    QColor bottomBorderColor = topBorderColor;
    if (!isDark && isHover() && !isPressed()) {
        bottomBorderColor = QColor(0, 0, 0, 27);
    }

    painter.strokePath(path, bottomBorderColor);

    // 绘制背景
    painter.setPen(Qt::NoPen);
    QRect rect = this->rect().adjusted(1, 1, -1, -1);

    painter.setBrush(this->backgroundColor());
    painter.drawRoundedRect(rect, r, r);
}


void SimpleCardWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setBrush(this->backgroundColor());

    if (Theme::isDark())
        painter.setPen(QColor(0, 0, 0, 48));
    else
        painter.setPen(QColor(0, 0, 0, 12));

    int r = this->borderRadius();
    painter.drawRoundedRect(this->rect().adjusted(1, 1, -1, -1), r, r);
}

QColor SimpleCardWidget::normalBackgroundColor() const
{
    return QColor(255, 255, 255, Theme::isDark() ? 13 : 170);
}

QColor SimpleCardWidget::hoverBackgroundColor() const
{
    return this->normalBackgroundColor();
}

QColor SimpleCardWidget::pressedBackgroundColor() const
{
    return this->normalBackgroundColor();
}
