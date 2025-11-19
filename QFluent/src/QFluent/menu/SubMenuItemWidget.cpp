#include "SubMenuItemWidget.h"

#include <QPainter>
#include <QRectF>
#include <QListWidgetItem>
#include <QMenu>

#include "FluentIcon.h"
#include "FluentGlobal.h"

SubMenuItemWidget::SubMenuItemWidget(QMenu* menu, QListWidgetItem* item, QWidget* parent)
    : QWidget(parent)
    , menuAsQMenu(menu)
    , item(item)
{
}

SubMenuItemWidget::SubMenuItemWidget(RoundMenu* menu, QListWidgetItem* item, QWidget* parent)
    : QWidget(parent)
    , menuAsRoundMenu(menu)
    , item(item)
{
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void SubMenuItemWidget::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);
    emit showMenuSig(item);
}
#else
void SubMenuItemWidget::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    emit showMenuSig(item);
}
#endif

void SubMenuItemWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // draw right arrow
    QRectF arrowRect(width() - 10, height() / 2.0 - 9.0 / 2.0, 9, 9);
    FluentIcon(Fluent::IconType::CHEVRON_RIGHT).render(&painter, arrowRect);
}
