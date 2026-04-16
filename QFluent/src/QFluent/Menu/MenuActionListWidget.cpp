#include "MenuActionListWidget.h"

#include <QWheelEvent>
#include <QApplication>

#include "Screen.h"
#include "MenuItemDelegate.h"
#include "QFluent/ScrollBar.h"

MenuActionListWidget::MenuActionListWidget(QWidget* parent)
    : QListWidget(parent) {

    this->setProperty("_itemHeight", 28);
    this->setProperty("_maxVisibleItems", -1);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBar(new ScrollBar(this));

    setItemDelegate(new ShortcutMenuItemDelegate(this));

    setViewportMargins(0, 6, 0, 6);
    setTextElideMode(Qt::ElideNone);
    setMouseTracking(true);
    setIconSize(QSize(14, 14));

    setAttribute(Qt::WA_Hover, true);
    installEventFilter(this);
}

void MenuActionListWidget::setItemHeight(int height) {
    if (this->property("_itemHeight").toInt() == height) return;
    this->setProperty("_itemHeight", height);

    for (int i = 0; i < count(); ++i) {
        QListWidgetItem* item = this->item(i);
        item->setSizeHint(QSize(item->sizeHint().width(), height));
    }
    adjustSize();
}

void MenuActionListWidget::setMaxVisibleItems(int num) {
    this->setProperty("_maxVisibleItems", num);
    adjustSize();
}

int MenuActionListWidget::maxVisibleItems() const {
    return this->property("_maxVisibleItems").toInt();
}

void MenuActionListWidget::adjustSize(const QPoint& pos, Fluent::MenuAnimation aniType) {
    QSize size(0, 0);

    // 计算内容大小
    for (int i = 0; i < count(); ++i) {
        QSize itemSize = item(i)->sizeHint();
        size.setWidth(qMax(itemSize.width(), size.width()));
        size.setHeight(size.height() + itemSize.height());
    }

    QPoint point = availableViewSize(pos, Fluent::MenuAnimation::NONE);
    int w = point.x();
    int h = point.y();

    QMargins m = viewportMargins();
    size += QSize(m.left()+m.right()+2, m.top()+m.bottom());

    size.setHeight(qMin(h, size.height()+3));
    size.setWidth(qMax(qMin(w, size.width()), minimumWidth()));

    // 限制最大可见项
    if (this->property("_maxVisibleItems").toInt() > 0) {
        int maxHeight = this->property("_maxVisibleItems").toInt() *
                            this->property("_itemHeight").toInt() + m.top() + m.bottom() +  3;

        if (size.height() > maxHeight) {
            size.setHeight(maxHeight);
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    setFixedSize(size);
}

void MenuActionListWidget::wheelEvent(QWheelEvent *event)
{
    QApplication::sendEvent(verticalScrollBar(), event);
}

int MenuActionListWidget::heightForAnimation(const QPoint &pos, Fluent::MenuAnimation aniType)
{
    int ih = itemsHeight();
    int sh = 0;
    QRect rect = Screen::currentScreenGeometry();

    switch (aniType) {
    case Fluent::MenuAnimation::DROP_DOWN:
        sh = qMax(rect.bottom() - pos.y() - 10, 1);
        break;
    case Fluent::MenuAnimation::PULL_UP:
        sh = qMax(pos.y() - rect.top() - 28, 1);
        break;
    case Fluent::MenuAnimation::FADE_IN_DROP_DOWN:
        sh = qMax(rect.bottom() - pos.y() - 10, 1);
        break;
    case Fluent::MenuAnimation::FADE_IN_PULL_UP:
        sh = qMax(pos.y() - rect.top() - 28, 1);
        break;
    default:
        sh = rect.height() - 100;
        break;
    }

    return qMin(ih, sh);
}

int MenuActionListWidget::itemHeight() const
{
    return this->property("_itemHeight").toInt();
}

int MenuActionListWidget::itemsHeight() const
{
    int maxVisible = maxVisibleItems();

    int N = (maxVisible < 0) ? count() : qMin(maxVisible, count());

    int h = 0;
    for (int i = 0; i < N; ++i) {
        h += item(i)->sizeHint().height();
    }
    QMargins m = viewportMargins();
    return h + m.top() + m.bottom();
}

QPoint MenuActionListWidget::availableViewSize(const QPoint &pos, Fluent::MenuAnimation aniType)
{
    QRect rect = Screen::currentScreenGeometry();
    QPoint point;

    switch (aniType) {
    case Fluent::MenuAnimation::DROP_DOWN:
        point.setX(rect.width() - 100);
        point.setY(qMax(rect.bottom() - pos.y() - 10, 1));
        break;
    case Fluent::MenuAnimation::PULL_UP:
        point.setX(rect.width() - 100);
        point.setY(qMax(pos.y() - rect.top() - 28, 1));
        break;
    case Fluent::MenuAnimation::FADE_IN_DROP_DOWN:
        point.setX(rect.width() - 100);
        point.setY(qMax(rect.bottom() - pos.y() - 10, 1));
        break;
    case Fluent::MenuAnimation::FADE_IN_PULL_UP:
        point.setX(rect.width() - 100);
        point.setY(qMax(pos.y() - rect.top() - 28, 1));
        break;
    default:
        point.setX(rect.width() - 100);
        point.setY(rect.height() - 100);
        break;
    }
    return point;
}

void MenuActionListWidget::addItem(QListWidgetItem *item)
{
    insertItem(count(), item);
}

void MenuActionListWidget::hideEvent(QHideEvent *event)
{
    setCurrentRow(-1);

    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(viewport(), &leave);
    viewport()->update();

    QListView::hideEvent(event);
}
