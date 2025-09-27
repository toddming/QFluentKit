#include "MenuActionListWidget.h"

#include <QWheelEvent>

#include "Screen.h"
#include "MenuItemDelegate.h"
#include "QFluent/scrollbar/ScrollBar.h"

MenuActionListWidget::MenuActionListWidget(QWidget* parent)
    : QListWidget(parent) {

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ScrollBar* scrollBar = new ScrollBar(Qt::Vertical, this);
    setVerticalScrollBar(scrollBar);

    setItemDelegate(new ShortcutMenuItemDelegate(this));

    setViewportMargins(0, 6, 0, 6);
    setTextElideMode(Qt::ElideNone);
    setMouseTracking(true);
    setIconSize(QSize(14, 14));

    // setProperty("transparent", true);
    setAttribute(Qt::WA_Hover, true);
    installEventFilter(this);
}

void MenuActionListWidget::setItemHeight(int height) {
    if (m_itemHeight == height) return;
    m_itemHeight = height;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem* item = this->item(i);
        item->setSizeHint(QSize(item->sizeHint().width(), height));
    }
    adjustSize();
}

void MenuActionListWidget::setMaxVisibleItems(int num) {
    m_maxVisibleItems = num;
    adjustSize();
}

int MenuActionListWidget::maxVisibleItems() const {
    return m_maxVisibleItems;
}

void MenuActionListWidget::adjustSize(const QPoint& pos, MenuAnimationType::MenuAnimation aniType) {
    QSize size(0, 0);

    // 计算内容大小
    for (int i = 0; i < count(); ++i) {
        QSize itemSize = item(i)->sizeHint();
        size.setWidth(qMax(itemSize.width(), size.width()));
        size.setHeight(size.height() + itemSize.height());
    }

    QPoint point = availableViewSize(pos, aniType);
    int w = point.x();
    int h = point.y();

    QMargins m = viewportMargins();
    size += QSize(m.left()+m.right()+2, m.top()+m.bottom());
    size.setHeight(qMin(h, size.height()+3));
    size.setWidth(qMax(qMin(w, size.width()), minimumWidth()));

    // 限制最大可见项
    if (m_maxVisibleItems > 0) {
        int maxHeight = m_maxVisibleItems * m_itemHeight + m.top() + m.bottom() +  3;

        if (size.height() > maxHeight) {
            size.setHeight(maxHeight);
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    setFixedSize(size);
}

void MenuActionListWidget::wheelEvent(QWheelEvent* e) {
    QScrollBar* vBar = verticalScrollBar();
    int step = e->angleDelta().y() / 10;
    vBar->setValue(vBar->value() - step);
    e->accept();
}

int MenuActionListWidget::heightForAnimation(const QPoint &pos, MenuAnimationType::MenuAnimation aniType)
{
    int ih = itemsHeight();
    int sh = 0;
    QRect rect = Screen::getCurrentScreenGeometry();

    switch (aniType) {
    case MenuAnimationType::MenuAnimation::DROP_DOWN:
        sh = qMax(rect.bottom() - pos.y() - 10, 1);
        break;
    case MenuAnimationType::MenuAnimation::PULL_UP:
        sh = qMax(pos.y() - rect.top() - 28, 1);
        break;
    case MenuAnimationType::MenuAnimation::FADE_IN_DROP_DOWN:
        sh = qMax(rect.bottom() - pos.y() - 10, 1);
        break;
    case MenuAnimationType::MenuAnimation::FADE_IN_PULL_UP:
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
    return m_itemHeight;
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

QPoint MenuActionListWidget::availableViewSize(const QPoint &pos, MenuAnimationType::MenuAnimation aniType)
{
    QRect rect = Screen::getCurrentScreenGeometry();
    QPoint point;

    switch (aniType) {
    case MenuAnimationType::MenuAnimation::DROP_DOWN:
        point.setX(rect.width() - 100);
        point.setY(qMax(rect.bottom() - pos.y() - 10, 1));
        break;
    case MenuAnimationType::MenuAnimation::PULL_UP:
        point.setX(rect.width() - 100);
        point.setY(qMax(pos.y() - rect.top() - 28, 1));
        break;
    case MenuAnimationType::MenuAnimation::FADE_IN_DROP_DOWN:
        point.setX(rect.width() - 100);
        point.setY(qMax(rect.bottom() - pos.y() - 10, 1));
        break;
    case MenuAnimationType::MenuAnimation::FADE_IN_PULL_UP:
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
