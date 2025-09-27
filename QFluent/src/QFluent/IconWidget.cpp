#include "IconWidget.h"
#include <QPainter>

IconWidget::IconWidget(QWidget *parent)
    : QWidget(parent)
{
    setIcon(QIcon());
}

IconWidget::IconWidget(const QIcon &icon, QWidget *parent)
    : IconWidget(parent)
{
    setIcon(icon);
}

IconWidget::IconWidget(const QString &iconPath, QWidget *parent)
    : IconWidget(parent)
{
    setIcon(iconPath);
}

IconWidget::IconWidget(IconType::FLuentIcon icon, QWidget *parent)
    : IconWidget(parent)
{
    setIcon(icon);
}

void IconWidget::setIcon(const QIcon &icon)
{
    m_icon = icon;
    m_isFluentIcon = false;
    m_fluentIcon = IconType::FLuentIcon::NONE;
    update();
}

void IconWidget::setIcon(const QString &iconPath)
{
    setIcon(QIcon(iconPath));
}

void IconWidget::setIcon(IconType::FLuentIcon icon)
{
    m_fluentIcon = icon;
    m_isFluentIcon = true;
    m_icon = QIcon();
    update();
}

QIcon IconWidget::getIcon() const
{
    return m_icon;
}

QSize IconWidget::sizeHint() const
{
    return QSize(16, 16); // 默认建议大小，可被 setFixedSize 覆盖
}

void IconWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QRect rect = this->rect();

    if (m_isFluentIcon && (m_fluentIcon != IconType::FLuentIcon::NONE)) {
        Icon::drawSvgIcon(&painter, m_fluentIcon, rect);
    } else {
        if (!m_icon.isNull()) {
            m_icon.paint(&painter, rect, Qt::AlignCenter, QIcon::Normal);
        }
    }
}
