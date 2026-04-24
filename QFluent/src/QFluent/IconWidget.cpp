#include "IconWidget.h"
#include "FluentIcon.h"
#include <QPainter>
#include <QIcon>

IconWidget::IconWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(16, 16);
}

IconWidget::IconWidget(const QIcon &icon, QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(16, 16);
    setIcon(icon);
}

IconWidget::IconWidget(Fluent::IconType type, QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(16, 16);
    setIcon(type);
}

void IconWidget::setIcon(const QIcon &icon)
{
    m_icon = icon;
    update();
}

void IconWidget::setIcon(Fluent::IconType type)
{
    m_icon = Fluent::icon(type);
    update();
}

QIcon IconWidget::icon() const
{
    return m_icon;
}

QSize IconWidget::sizeHint() const
{
    return QSize(16, 16);
}

void IconWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_icon.isNull()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    drawIcon(&painter, rect());
}

void IconWidget::drawIcon(QPainter *painter, const QRectF &rect)
{
    m_icon.paint(painter, rect.toRect());
}