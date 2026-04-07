#include "AcrylicCheckableMenu.h"
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QPainterPath>
#include <QColor>
#include <QRectF>
#include <QPaintEvent>
#include <QSize>
#include <QMargins>
#include <QHBoxLayout>

#include "QFluent/Menu/MenuItemDelegate.h"
#include "QFluent/Menu/MenuAnimation.h"
#include "Theme.h"

AcrylicCheckableMenu::AcrylicCheckableMenu(const QString &title, QWidget *parent,
                                           Fluent::MenuIndicator indicatorType)
    : RoundMenu(title, parent)
{
    m_listWidget = new AcrylicMenuActionListWidget(this);
    m_listWidget->setObjectName("checkableListWidget");
    setUpMenu(m_listWidget);

    if (indicatorType == Fluent::MenuIndicator::CHECK) {
        m_listWidget->setItemDelegate(new CheckIndicatorMenuItemDelegate(this));
    } else {
        m_listWidget->setItemDelegate(new RadioIndicatorMenuItemDelegate(this));
    }
}

void AcrylicCheckableMenu::setItemHeight(int height)
{
    m_listWidget->setItemHeight(height);
}

void AcrylicCheckableMenu::setUpMenu(AcrylicMenuActionListWidget *newView)
{
    if (view()) {
        hBoxLayout()->removeWidget(view());
        view()->deleteLater();
    }
    setView(newView);
    hBoxLayout()->addWidget(newView);

    connect(newView, &AcrylicMenuActionListWidget::itemClicked, this, &AcrylicCheckableMenu::onItemClicked);
    connect(newView, &AcrylicMenuActionListWidget::itemEntered, this, &AcrylicCheckableMenu::onItemEntered);
}

void AcrylicCheckableMenu::exec(const QPoint &pos, bool animate, Fluent::MenuAnimation aniType)
{
    QPoint p = MenuAnimationManager::make(this, aniType)->endPosition(pos);
    RoundMenu::exec(pos, animate, aniType);
    m_listWidget->acrylicBrush().grabImage(QRect(p, layout()->sizeHint()));
}

int AcrylicCheckableMenu::adjustItemText(QListWidgetItem *item, QAction *action)
{
    int w = RoundMenu::adjustItemText(item, action);
    item->setSizeHint(QSize(w + 26, itemHeight()));
    return w + 26;
}
