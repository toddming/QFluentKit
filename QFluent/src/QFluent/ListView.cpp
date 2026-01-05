#include "ListView.h"

#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QTableView>

#include "Theme.h"
#include "ScrollBar.h"

// ==========================================
// ListItemDelegate Implementation
// ==========================================

ListItemDelegate::ListItemDelegate(QAbstractItemView* parent)
    : TableItemDelegate(parent)
    , m_lightCheckedColor(Qt::white) // 默认值
    , m_darkCheckedColor(Qt::black)  // 默认值
{
}

void ListItemDelegate::setCheckedColor(const QColor& lightColor, const QColor& darkColor) {
    m_lightCheckedColor = lightColor;
    m_darkCheckedColor = darkColor;
}

void ListItemDelegate::drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->drawRoundedRect(option.rect, 5, 5);
}

void ListItemDelegate::drawIndicator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    int y = option.rect.y();
    int h = option.rect.height();

    int ph = qRound((pressedRow() == index.row()) ? 0.35 * h : 0.257 * h);

    painter->setBrush(Theme::instance()->themeColor());

    painter->drawRoundedRect(0, ph + y, 3, h - 2 * ph, 1.5, 1.5);
}


// ==========================================
// ListWidget Implementation
// ==========================================

ListWidget::ListWidget(QWidget* parent) : ListBase<QListWidget>(parent)
{
    setHorizontalScrollBar(new ScrollBar(this));
    setVerticalScrollBar(new ScrollBar(this));

}

void ListWidget::setCurrentItem(QListWidgetItem* item, QItemSelectionModel::SelectionFlags command) {
    if (item) {
        setCurrentRow(row(item), command);
    }
}

void ListWidget::setCurrentRow(int row, QItemSelectionModel::SelectionFlags command) {
    if (command == QItemSelectionModel::NoUpdate) {
        QListWidget::setCurrentRow(row);
    } else {
        QListWidget::setCurrentRow(row, command);
    }
    updateSelectedRows();
}

// ==========================================
// ListView Implementation
// ==========================================

ListView::ListView(QWidget* parent) : ListBase<QListView>(parent)
{
    setHorizontalScrollBar(new ScrollBar(this));
    setVerticalScrollBar(new ScrollBar(this));

}
