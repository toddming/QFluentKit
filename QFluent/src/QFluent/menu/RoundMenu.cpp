#include "RoundMenu.h"

#include <QColor>
#include <QPointF>
#include <QPainter>
#include <QHoverEvent>
#include <QTimer>
#include <QApplication>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

#include "Theme.h"
#include "StyleSheet.h"

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

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::MENU);

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

    d->_view->adjustSize(pos, aniType);
    if (!animate) aniType = MenuAnimationType::MenuAnimation::NONE;
    MenuAnimationManager::make(this, aniType)->exec(pos);
    adjustMenuSize();

    show();
    if (d->_pIsSubMenu)
        d->_menuItem->setSelected(true);
}



void RoundMenu::adjustMenuSize() {
    Q_D(RoundMenu);

    QSize viewSize = d->_view->size();
    setFixedSize(viewSize.width() + d->_layout->contentsMargins().left() +
                 d->_layout->contentsMargins().right(),
                 viewSize.height() + d->_layout->contentsMargins().top() +
                 d->_layout->contentsMargins().bottom() + 2);
}

int RoundMenu::itemHeight() const
{
    Q_D_CONST(RoundMenu);

    return d->_view->itemHeight();
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
    d->_isHideBySystem = true;
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
    d->_isHideBySystem = isHideBySystem;
    d->_view->clearSelection();
    if (d->_pIsSubMenu) {
        hide();
    } else {
        close();
    }
}

// void RoundMenu::mouseMoveEvent(QMouseEvent* e) {
//     Q_D(RoundMenu);

//     if (!d->_pIsSubMenu) {
//         return;
//     }

//     QPoint pos = e->globalPosition().toPoint();

//     // get the rect of menu item
//     QMargins margin = d->_view->contentsMargins();
//     QRect rect = d->_view->visualItemRect(nullptr)
//                      .translated(d->_view->mapToGlobal(QPoint(0, 0)));
//     rect.translate(margin.left(), margin.top() + 2);

//     if (d->_view->geometry().contains(pos) &&
//         !rect.contains(pos) &&
//         !this->geometry().contains(pos)) {
//         d->_view->clearSelection();
//         hideMenu(false);
//     }
// }

void RoundMenu::mouseMoveEvent(QMouseEvent* e) {
    Q_D(RoundMenu);
    if (!d->_pIsSubMenu) {
        return;
    }

    // 获取鼠标全局位置
    QPoint pos = e->globalPosition().toPoint();

    auto view = d->_parentMenu->view();
    // 获取当前菜单项的矩形区域
    QMargins margin = view->contentsMargins();
    QRect rect = view->visualItemRect(d->_menuItem).translated(view->mapToGlobal(QPoint(0, 0)));

    rect.translate(margin.left(), margin.top() + 2);

    // 检查鼠标是否在父菜单区域内，但不在当前菜单项矩形区域内，且也不在当前子菜单区域内
    if (d->_parentMenu && d->_parentMenu->geometry().contains(pos) &&
        !rect.contains(pos) &&
        !this->geometry().contains(pos)) {
        view->clearSelection();
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


int RoundMenu::adjustItemText(QListWidgetItem *item, QAction *action)
{
    Q_D(RoundMenu);

    if (!item || !action) return 0;

    QString text = action->text();
    text.remove('&');

    QFontMetrics fm(item->font());
    int shortcutWidth = d->longestShortcutWidth();
    if (shortcutWidth > 0) {
        shortcutWidth += 22; // 保留空间
    }

    int width;
    bool hasIcon = d->hasItemIcon();

    if (!hasIcon) {
        width = 40 + fm.horizontalAdvance(text) + shortcutWidth;
    } else {
        text = " " + text;
        int space = 4 - fm.horizontalAdvance(" ");
        width = 60 + fm.horizontalAdvance(text) + shortcutWidth + space;
        item->setText(text);
    }

    return width;
}
