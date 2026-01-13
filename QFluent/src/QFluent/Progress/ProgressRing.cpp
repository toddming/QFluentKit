#include "ProgressRing.h"
#include <QPen>
#include <QPainter>
#include "Theme.h"

ProgressRing::ProgressRing(QWidget *parent, bool useAni)
    : ProgressBar(parent, useAni), _strokeWidth(6)
{
    lightBackgroundColor = QColor(0, 0, 0, 34);
    darkBackgroundColor = QColor(255, 255, 255, 34);
    setCustomBackgroundColor(lightBackgroundColor, darkBackgroundColor);

    setFont(QFont("Segoe UI", 10));
    setTextVisible(false);
    setFixedSize(55, 55);
}

int ProgressRing::getStrokeWidth() const
{
    return _strokeWidth;
}

void ProgressRing::setStrokeWidth(int w)
{
    if (_strokeWidth == w) return;
    _strokeWidth = w;
    update();
}

void ProgressRing::_drawText(QPainter &painter, const QString &text)
{
    painter.setFont(font());
    painter.setPen(Theme::instance()->isDarkTheme() ? Qt::white : Qt::black);
    painter.drawText(rect(), Qt::AlignCenter, text);
}

void ProgressRing::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    const int cw = _strokeWidth; // 圆环厚度
    const int w = qMin(width(), height()) - cw;
    const QRectF rc(cw / 2, height() / 2 - w / 2, w, w);

    // 绘制背景
    const QColor bc = Theme::instance()->isDarkTheme() ? darkBackgroundColor : lightBackgroundColor;
    QPen pen(bc, cw, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.drawArc(rc, 0, 360 * 16);

    if (minimum() >= maximum()) return;

    // 绘制进度条
    pen.setColor(barColor());
    painter.setPen(pen);
    const int degree = static_cast<int>(getVal() / (maximum() - minimum()) * 360);
    painter.drawArc(rc, 90 * 16, -degree * 16);

    // 绘制文本
    if (isTextVisible()) {
        _drawText(painter, QString::number(static_cast<int>(getVal())));
    }
}
