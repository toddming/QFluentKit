#include "IconWidget.h"
#include <QPainter>
#include <QIcon>

#include "FluentIcon.h"
#include "Private/IconWidgetPrivate.h"

Q_PROPERTY_CREATE_Q_CPP(IconWidget, ThemeType::ThemeMode, IconTheme)
IconWidget::IconWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new IconWidgetPrivate())
{
    Q_D(IconWidget);
    d->q_ptr = this;
    d->m_fluentIcon = nullptr;
    d->_pIconTheme = ThemeType::ThemeMode::AUTO;

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
    Q_D(IconWidget);

    delete d->m_fluentIcon;
}

void IconWidget::setIcon(const QIcon &icon)
{
    Q_D(IconWidget);

    d->_pIcon = icon;
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

    d->_pIcon = QIcon();
    update();
}

QIcon IconWidget::getIcon() const
{
    Q_D_CONST(IconWidget);
    return d->_pIcon;
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
        FluentIconUtils::drawIcon(*d->m_fluentIcon, &painter, rect, d->_pIconTheme);
    } else if (!d->_pIcon.isNull()) {
        d->_pIcon.paint(&painter, rect, Qt::AlignCenter, QIcon::Normal);
    }
}


