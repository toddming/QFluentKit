#include "IconWidget.h"
#include <QPainter>
#include <QIcon>

#include "FluentIcon.h"
#include "Private/IconWidgetPrivate.h"

IconWidget::IconWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new IconWidgetPrivate)
{
    Q_D(IconWidget);
    d->q_ptr = this;
    d->m_iconTheme = Fluent::ThemeMode::AUTO;

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

IconWidget::IconWidget(const FluentIconBase &icon, QWidget* parent)
    : IconWidget(parent)
{
    setFluentIcon(icon);
}

IconWidget::~IconWidget()
{
}

void IconWidget::setIcon(const QIcon &icon)
{
    Q_D(IconWidget);

    d->m_icon = icon;
    update();
}

void IconWidget::setIcon(const QString &iconPath)
{
    setIcon(QIcon(iconPath));
}


void IconWidget::setFluentIcon(const FluentIconBase &icon)
{
    Q_D(IconWidget);

    d->m_fluentIcon = icon.clone();

    d->m_icon = QIcon();
    update();
}

QIcon IconWidget::icon() const
{
    Q_D(const IconWidget);
    return d->m_icon;
}

QSize IconWidget::sizeHint() const
{
    return QSize(16, 16); // 默认建议大小，可被 setFixedSize 覆盖
}

void IconWidget::paintEvent(QPaintEvent *event)
{
    Q_D(IconWidget);

    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QRect rect = this->rect();

    if (d->m_fluentIcon) {
        FluentIconUtils::drawIcon(*d->m_fluentIcon, &painter, rect, d->m_iconTheme);
    } else if (!d->m_icon.isNull()) {
        d->m_icon.paint(&painter, rect, Qt::AlignCenter, QIcon::Normal);
    }
}

void IconWidget::setIconTheme(Fluent::ThemeMode theme)
{
    Q_D(IconWidget);
    d->m_iconTheme = theme;
}

Fluent::ThemeMode IconWidget::iconTheme()
{
    Q_D(IconWidget);
    return d->m_iconTheme;
}
