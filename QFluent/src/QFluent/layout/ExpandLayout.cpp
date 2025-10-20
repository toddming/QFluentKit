#include "ExpandLayout.h"
#include <QResizeEvent>
#include <QMargins>
#include <QList>
#include <QWidget>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QEvent>

ExpandLayout::ExpandLayout(QWidget *parent)
    : QLayout(parent)
{
}

ExpandLayout::~ExpandLayout()
{
    qDeleteAll(m_items);
    m_items.clear();
}

void ExpandLayout::addWidget(QWidget *widget)
{
    if (!widget || m_widgets.contains(widget))
        return;

    m_widgets.append(widget);
    widget->installEventFilter(this);
    addItem(new QWidgetItem(widget));
}

void ExpandLayout::addItem(QLayoutItem *item)
{
    if (!item)
        return;
    m_items.append(item);
}

int ExpandLayout::count() const
{
    return m_items.size();
}

QLayoutItem *ExpandLayout::itemAt(int index) const
{
    if (index >= 0 && index < m_items.size())
        return m_items.at(index);
    return nullptr;
}

QLayoutItem *ExpandLayout::takeAt(int index)
{
    if (index >= 0 && index < m_items.size()) {
        QLayoutItem *item = m_items.takeAt(index);
        if (index < m_widgets.size())
            m_widgets.removeAt(index); // 同步移除 widget 引用
        return item;
    }
    return nullptr;
}

Qt::Orientations ExpandLayout::expandingDirections() const
{
    return Qt::Vertical;
}

bool ExpandLayout::hasHeightForWidth() const
{
    return true;
}

int ExpandLayout::heightForWidth(int width) const
{
    QRect rect(0, 0, width, 0);
    return doLayout(rect, false);
}

void ExpandLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, true);
}

QSize ExpandLayout::sizeHint() const
{
    return minimumSize();
}

QSize ExpandLayout::minimumSize() const
{
    QSize size;
    for (QWidget *w : m_widgets) {
        if (w && !w->isHidden())
            size = size.expandedTo(w->minimumSize());
    }

    QMargins m = contentsMargins();
    size += QSize(m.left() + m.right(), m.top() + m.bottom());
    return size;
}

int ExpandLayout::doLayout(const QRect &rect, bool move) const
{
    QMargins margin = contentsMargins();
    int x = rect.x() + margin.left();
    int y = rect.y() + margin.top();
    int width = rect.width() - margin.left() - margin.right();

    for (int i = 0; i < m_widgets.size(); ++i) {
        QWidget *w = m_widgets.at(i);
        if (!w || w->isHidden())
            continue;

        if (i > 0)
            y += spacing();

        if (move) {
            w->setGeometry(QRect(QPoint(x, y), QSize(width, w->height())));
        }

        y += w->height();
    }

    return y - rect.y();
}

bool ExpandLayout::eventFilter(QObject *obj, QEvent *event)
{
    if (m_widgets.contains(static_cast<QWidget*>(obj)) && event->type() == QEvent::Resize) {
        auto resizeEvent = static_cast<QResizeEvent*>(event);
        QSize delta = resizeEvent->size() - resizeEvent->oldSize();

        if (delta.height() != 0 && delta.width() == 0) {
            QWidget *parentWidget = this->parentWidget();
            if (parentWidget) {
                parentWidget->resize(parentWidget->width(), parentWidget->height() + delta.height());
            }
        }
    }

    return QLayout::eventFilter(obj, event);
}
