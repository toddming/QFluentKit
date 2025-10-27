#include "SubMenuItemWidget.h"

#include <QPainter>
#include <QRectF>
#include <QListWidgetItem>
#include <QMenu>

#include "FluentIcon.h"
#include "Define.h"

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

void SubMenuItemWidget::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);
    emit showMenuSig(item);
}

void SubMenuItemWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // draw right arrow
    QRectF arrowRect(width() - 10, height() / 2.0 - 9.0 / 2.0, 9, 9);
    FluentIcon(FluentIconType::CHEVRON_RIGHT).render(&painter, arrowRect);
}
