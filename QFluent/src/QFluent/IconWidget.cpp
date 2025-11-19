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
    d->_IconTheme = ThemeType::ThemeMode::AUTO;

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

    d->_Icon = icon;
    update();
}

void IconWidget::setIcon(const QString &iconPath)
{
    setIcon(QIcon(iconPath));
}


void IconWidget::setFluentIcon(const FluentIconBase &icon)
{
    Q_D(IconWidget);

    d->_fluentIcon.reset(icon.clone());

    d->_Icon = QIcon();
    update();
}

QIcon IconWidget::getIcon() const
{
    Q_D(const IconWidget);
    return d->_Icon;
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

    if (d->_fluentIcon) {
        FluentIconUtils::drawIcon(*d->_fluentIcon, &painter, rect, d->_IconTheme);
    } else if (!d->_Icon.isNull()) {
        d->_Icon.paint(&painter, rect, Qt::AlignCenter, QIcon::Normal);
    }
}

void IconWidget::setIconTheme(ThemeType::ThemeMode theme)
{
    Q_D(IconWidget);
    d->_IconTheme = theme;
}

ThemeType::ThemeMode IconWidget::getIconTheme()
{
    Q_D(IconWidget);
    return d->_IconTheme;
}
