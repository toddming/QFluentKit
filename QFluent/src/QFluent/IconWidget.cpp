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
    d->iconTheme = Fluent::ThemeMode::AUTO;

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

    d->icon = icon;
    update();
}

void IconWidget::setIcon(const QString &iconPath)
{
    setIcon(QIcon(iconPath));
}


void IconWidget::setFluentIcon(const FluentIconBase &icon)
{
    Q_D(IconWidget);

    d->fluentIcon = icon.clone();

    d->icon = QIcon();
    update();
}

QIcon IconWidget::icon() const
{
    Q_D(const IconWidget);
    return d->icon;
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

    if (d->fluentIcon) {
        FluentIconUtils::drawIcon(*d->fluentIcon, &painter, rect, d->iconTheme);
    } else if (!d->icon.isNull()) {
        d->icon.paint(&painter, rect, Qt::AlignCenter, QIcon::Normal);
    }
}

void IconWidget::setIconTheme(Fluent::ThemeMode theme)
{
    Q_D(IconWidget);
    d->iconTheme = theme;
}

Fluent::ThemeMode IconWidget::iconTheme()
{
    Q_D(IconWidget);
    return d->iconTheme;
}
