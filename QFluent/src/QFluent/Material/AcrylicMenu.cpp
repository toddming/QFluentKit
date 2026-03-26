#include "AcrylicMenu.h"
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

#include "QFluent/Menu/MenuAnimation.h"
#include "Theme.h"

AcrylicMenuActionListWidget::AcrylicMenuActionListWidget(QWidget *parent)
    : MenuActionListWidget(parent)
    , m_acrylicBrush(viewport(), 35)
{
    setViewportMargins(0, 0, 0, 0);
    setProperty("transparent", true);
    MenuActionListWidget::addItem(createPlaceholderItem(topMargin()));
    MenuActionListWidget::addItem(createPlaceholderItem(bottomMargin()));
}

void AcrylicMenuActionListWidget::updateAcrylicColor()
{
    QColor tintColor, luminosityColor;
    if (Theme::instance()->isDarkTheme()) {
        tintColor = QColor(32, 32, 32, 200);
        luminosityColor = QColor(0, 0, 0, 0);
    } else {
        tintColor = QColor(255, 255, 255, 160);
        luminosityColor = QColor(255, 255, 255, 50);
    }
    m_acrylicBrush.setTintColor(tintColor);
    m_acrylicBrush.setLuminosityColor(luminosityColor);
}

int AcrylicMenuActionListWidget::topMargin() const
{
    return 6;
}

int AcrylicMenuActionListWidget::bottomMargin() const
{
    return 6;
}

void AcrylicMenuActionListWidget::setItemHeight(int height)
{
    if (height == property("_itemHeight").toInt()) {
        return;
    }
    setProperty("_itemHeight", height);

    for (int i = 1; i < count() - 1; ++i) {
        QListWidgetItem *item = this->item(i);
        if (!itemWidget(item)) {
            QSize size = item->sizeHint();
            size.setHeight(height);
            item->setSizeHint(size);
        }
    }
    adjustSize();
}

void AcrylicMenuActionListWidget::addItem(QListWidgetItem *item)
{
    insertItem(count() - 1, item);
}

QListWidgetItem* AcrylicMenuActionListWidget::createPlaceholderItem(int height)
{
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(1, height));
    item->setFlags(Qt::NoItemFlags);
    return item;
}

QPainterPath AcrylicMenuActionListWidget::clipPath() const
{
    QPainterPath path;
    path.addRoundedRect(QRectF(rect()).adjusted(0, 0, -2.5, -2.5), 8, 8);
    return path;
}

void AcrylicMenuActionListWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_acrylicBrush.setClipPath(clipPath());
    updateAcrylicColor();
    m_acrylicBrush.paint();
    MenuActionListWidget::paintEvent(event);
}

AcrylicMenu::AcrylicMenu(const QString &title, QWidget *parent)
    : RoundMenu(title, parent)
{
    m_listWidget = new AcrylicMenuActionListWidget(this);
    setUpMenu(m_listWidget);
}

void AcrylicMenu::setItemHeight(int height)
{
    m_listWidget->setItemHeight(height);
}

void AcrylicMenu::setUpMenu(AcrylicMenuActionListWidget *newView)
{
    if (view()) {
        hBoxLayout()->removeWidget(view());
        view()->deleteLater();
    }
    setView(newView);
    hBoxLayout()->addWidget(newView);

    connect(newView, &AcrylicMenuActionListWidget::itemClicked, this, &AcrylicMenu::onItemClicked);
    connect(newView, &AcrylicMenuActionListWidget::itemEntered, this, &AcrylicMenu::onItemEntered);
}

void AcrylicMenu::exec(const QPoint &pos, bool animate, Fluent::MenuAnimation aniType)
{
    QPoint p = MenuAnimationManager::make(this, aniType)->endPosition(pos);
    RoundMenu::exec(pos, animate, aniType);
    m_listWidget->m_acrylicBrush.grabImage(QRect(p, layout()->sizeHint()));
}
