#include "RoundMenu.h"

#include <QColor>
#include <QPointF>
#include <QPainter>
#include <QHoverEvent>
#include <QTimer>
#include <QApplication>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

#include <QDebug>

#include "Theme.h"
#include "MenuAnimation.h"
#include "Private/menu/RoundMenuPrivate.h"
#include "MenuActionListWidget.h"

RoundMenu::RoundMenu(const QString& title, QWidget* parent)
    : QMenu(title, parent), d_ptr(new RoundMenuPrivate())
{
    Q_D(RoundMenu);
    d->q_ptr = this;

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setMouseTracking(true);

    d->_pIsSubMenu = false;
    d->_layout = new QHBoxLayout(this);
    d->_view = new MenuActionListWidget(this);
    d->_showTimer = new QTimer(this);
    d->_shadowEffect = new QGraphicsDropShadowEffect(d->_view);

    d->_layout->addWidget(d->_view);
    d->_layout->setContentsMargins(12, 8, 12, 20);
    setLayout(d->_layout);

    d->setShadowEffect();

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::MENU);

    connect(d->_view, &QListWidget::itemClicked, d, &RoundMenuPrivate::handleItemClicked);
    connect(d->_view, &QListWidget::itemEntered, d, &RoundMenuPrivate::handleItemEntered);

    d->_showTimer->setSingleShot(true);
    d->_showTimer->setInterval(400);
    connect(d->_showTimer, &QTimer::timeout, d, &RoundMenuPrivate::onShowMenuTimeout);
}

RoundMenu::~RoundMenu() {
    Q_D(RoundMenu);
    const auto items = d->_view->findItems("", Qt::MatchContains);
    for (const auto item : items) {
        if (auto menu = item->data(Qt::UserRole).value<RoundMenu*>()) {
            delete menu;
        }
    }
}


void RoundMenu::setItemHeight(int height) {
    Q_D(RoundMenu);
    d->_view->setItemHeight(height);
}

void RoundMenu::setMaxVisibleItems(int num) {
    Q_D(RoundMenu);
    d->_view->setMaxVisibleItems(num);
}

void RoundMenu::addAction(QAction* action) {
    Q_D(RoundMenu);
    QListWidgetItem *item = d->createActionItem(action);
    d->_view->addItem(item);;
    adjustMenuSize();
}

void RoundMenu::insertAction(QAction* before, QAction* action) {
    Q_D(RoundMenu);

    for (int i = 0; i < d->_view->count(); ++i) {
        QListWidgetItem* item = d->_view->item(i);
        if (item->data(Qt::UserRole).value<QAction*>() == before) {
            d->createActionItem(action);
            QListWidgetItem* newItem = d->_view->takeItem(d->_view->count() - 1);
            d->_view->insertItem(i, newItem);
            return;
        }
    }

    addAction(action);
}

void RoundMenu::removeAction(QAction* action) {
    Q_D(RoundMenu);

    for (int i = 0; i < d->_view->count(); ++i) {
        QListWidgetItem* item = d->_view->item(i);
        if (item->data(Qt::UserRole).value<QAction*>() == action) {
            delete d->_view->takeItem(i);
            return;
        }
    }
}

void RoundMenu::addMenu(RoundMenu* menu) {
    Q_D(RoundMenu);
    if (menu == nullptr)
        return;
    if (d->_parentMenu) {
        d->_parentMenu->removeMenu(menu);
    }
    d->createSubMenuItem(menu);
    adjustMenuSize();
}

void RoundMenu::insertMenu(QAction *before, RoundMenu *menu)
{
    Q_D(RoundMenu);

    if (!menu)
        return;

    if (!d->_actions.contains(before))
        return;

    d->createSubMenuItem(menu);
    auto beforeItem = before->property("item").value<QListWidgetItem*>();
    int index = d->_view->row(beforeItem);
    d->_view->insertItem(index, menu->d_ptr->_menuItem);
    // d->_view->setItemWidget(menu->d_ptr->_menuItem, menu->m_menuItemWidget);
    adjustMenuSize();
}

void RoundMenu::removeMenu(RoundMenu *menu)
{
    Q_D(RoundMenu);
    if (!d->_subMenus.contains(menu))
        return;

    auto item = menu->d_ptr->_menuItem;
    d->_subMenus.removeOne(menu);
    d->removeItem(item);
}

void RoundMenu::addSeparator() {
    Q_D(RoundMenu);

    QMargins m = d->_view->contentsMargins();
    int w = d->_view->width()-m.left()-m.right();

    QListWidgetItem* separator = new QListWidgetItem;
    separator->setFlags(Qt::NoItemFlags);
    separator->setSizeHint(QSize(w, 9));
    separator->setData(Qt::DecorationRole, "separator");
    d->_view->addItem(separator);
    adjustMenuSize();
    adjustSize();
}

void RoundMenu::clear() {
    Q_D(RoundMenu);

    d->_view->clear();
}


void RoundMenu::exec(const QPoint& pos, bool animate, MenuAnimationType::MenuAnimation aniType) {
    Q_D(RoundMenu);

    if (!animate) aniType = MenuAnimationType::MenuAnimation::NONE;

    d->_view->adjustSize(pos, aniType);
    adjustMenuSize();

    show();
    MenuAnimationManager::make(this, aniType)->exec(pos);
}



void RoundMenu::adjustMenuSize() {
    Q_D(RoundMenu);

    QSize viewSize = d->_view->size();
    setFixedSize(viewSize.width() + d->_layout->contentsMargins().left() +
                 d->_layout->contentsMargins().right(),
                 viewSize.height() + d->_layout->contentsMargins().top() +
                 d->_layout->contentsMargins().bottom());
}

MenuActionListWidget* RoundMenu::view()
{
    Q_D(RoundMenu);

    return d->_view;
}

void RoundMenu::closeEvent(QCloseEvent* e) {
    Q_UNUSED(e);
    Q_D(RoundMenu);

    emit closed();
    if (d->_parentMenu) {
        d->_parentMenu->close();
    }
    d->_view->clearSelection();
    d->_view->setCurrentItem(nullptr);
    d->_view->clearFocus();
}

QList<QAction*> RoundMenu::menuActions()
{
    Q_D(RoundMenu);

    return d->_actions;
}

void RoundMenu::setDefaultAction(QAction *action)
{
    Q_D(RoundMenu);

    if (!d->_actions.contains(action)) {
        return;
    }
    QListWidgetItem* item = qvariant_cast<QListWidgetItem*>(action->property("item"));
    if (item) {
        d->_view->setCurrentItem(item);
    }
}


void RoundMenu::paintEvent(QPaintEvent *)
{

}

void RoundMenu::hideMenu(bool isHideBySystem)
{
    Q_D(RoundMenu);

    d->_view->clearSelection();
    if (d->_pIsSubMenu) {
        hide();
    } else {
        close();
    }
}

void RoundMenu::mouseMoveEvent(QMouseEvent* e) {
    Q_D(RoundMenu);

    if (!d->_pIsSubMenu) {
        return;
    }

    QPoint pos = e->globalPosition().toPoint();

    // get the rect of menu item
    QMargins margin = d->_view->contentsMargins();
    QRect rect = d->_view->visualItemRect(nullptr)
                     .translated(d->_view->mapToGlobal(QPoint(0, 0)));
    rect.translate(margin.left(), margin.top() + 2);

    if (d->_view->geometry().contains(pos) &&
        !rect.contains(pos) &&
        !this->geometry().contains(pos)) {
        d->_view->clearSelection();
        hideMenu(false);
    }
}

void RoundMenu::mousePressEvent(QMouseEvent* e) {
    Q_D(RoundMenu);

    QWidget* w = this->childAt(e->pos());
    if (w != d->_view && !d->_view->isAncestorOf(w)) {
        hideMenu(true);
    }
}


void RoundMenu::addWidget(QWidget *widget, bool selectable)
{
    Q_D(RoundMenu);

    auto action = new QAction();
    action->setSeparator(selectable);

    auto item = d->createActionItem(action);
    item->setSizeHint(widget->size());

    d->_view->addItem(item);
    d->_view->setItemWidget(item, widget);
    d->_view->adjustSize();

    if (!selectable) {
        item->setFlags(Qt::NoItemFlags);
    }

    adjustMenuSize();
    adjustSize();
}
