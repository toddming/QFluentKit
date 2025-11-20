#include "TableView.h"
#include <QPainter>
#include <QScrollBar>
#include "QFluent/scrollbar/ScrollBar.h"

TableItemDelegate::TableItemDelegate(QTableView* parent) : QStyledItemDelegate(parent) {}

void TableItemDelegate::setHoverRow(int row) {
    hoverRow = row;
}

void TableItemDelegate::setPressedRow(int row) {
    pressedRow = row;
}

void TableItemDelegate::setSelectedRows(const QList<QModelIndex>& indexes) {
    selectedRows.clear();
    for (const auto& index : indexes) {
        selectedRows.insert(index.row());
        if (index.row() == pressedRow) {
            pressedRow = -1;
        }
    }
}

QSize TableItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size += QSize(0, margin * 2); // grownBy(QMargins(0, margin, 0, margin))
    return size;
}

QWidget* TableItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    // LineEdit 未实现，假设LineEdit是自定义QLineEdit
    // LineEdit* lineEdit = new LineEdit(parent);
    // lineEdit->setProperty("transparent", false);
    // lineEdit->setStyle(QApplication::style());
    // lineEdit->setText(option.text);
    // lineEdit->setClearButtonEnabled(true);
    // return lineEdit;
    return nullptr; // 未实现
}

void TableItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QRect rect = option.rect;
    int y = rect.y() + (rect.height() - editor->height()) / 2;
    int x = qMax(8, rect.x());
    int w = rect.width();
    if (index.column() == 0) {
        w -= 8;
    }
    editor->setGeometry(x, y, w, rect.height());
}

void TableItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
    QStyledItemDelegate::initStyleOption(option, index);

    // option->font = index.data(Qt::FontRole).value<QFont>() || Theme::instance()->getFont(13);
    // option->font = Theme::instance()->getFont(13);

    QColor textColor = Theme::instance()->isDarkTheme() ? Qt::white : Qt::black;
    QVariant textBrushVar = index.data(Qt::ForegroundRole);
    if (textBrushVar.canConvert<QBrush>()) {
        QBrush textBrush = textBrushVar.value<QBrush>();
        textColor = textBrush.color();
    }

    option->palette.setColor(QPalette::Text, textColor);
    option->palette.setColor(QPalette::HighlightedText, textColor);
}

void TableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);

    painter->setClipping(true);
    painter->setClipRect(option.rect);

    QStyleOptionViewItem adjustedOption = option;
    adjustedOption.rect.adjust(0, margin, 0, -margin);

    bool isHover = hoverRow == index.row();
    bool isPressed = pressedRow == index.row();
    bool isAlternate = index.row() % 2 == 0 && qobject_cast<const QTableView*>(parent())->alternatingRowColors();
    bool isDark = Theme::instance()->isDarkTheme();

    int c = isDark ? 255 : 0;
    int alpha = 0;

    if (!selectedRows.contains(index.row())) {
        if (isPressed) {
            alpha = isDark ? 9 : 6;
        } else if (isHover) {
            alpha = 12;
        } else if (isAlternate) {
            alpha = 5;
        }
    } else {
        if (isPressed) {
            alpha = isDark ? 15 : 9;
        } else if (isHover) {
            alpha = 25;
        } else {
            alpha = 17;
        }
    }

    QVariant bgVar = index.data(Qt::BackgroundRole);
    if (bgVar.isValid()) {
        painter->setBrush(bgVar.value<QBrush>());
    } else {
        painter->setBrush(QColor(c, c, c, alpha));
    }

    _drawBackground(painter, adjustedOption, index);

    if (selectedRows.contains(index.row()) && index.column() == 0 && qobject_cast<const QTableView*>(parent())->horizontalScrollBar()->value() == 0) {
        _drawIndicator(painter, adjustedOption, index);
    }

    QVariant checkStateVar = index.data(Qt::CheckStateRole);
    if (checkStateVar.isValid()) {
        _drawCheckBox(painter, adjustedOption, index);
    }

    painter->restore();
    QStyledItemDelegate::paint(painter, adjustedOption, index);
}

void TableItemDelegate::_drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    int r = 5;
    QRect rect = option.rect;
    if (index.column() == 0) {
        rect.adjust(4, 0, r + 1, 0);
        painter->drawRoundedRect(rect, r, r);
    } else if (index.column() == index.model()->columnCount(index.parent()) - 1) {
        rect.adjust(-r - 1, 0, -4, 0);
        painter->drawRoundedRect(rect, r, r);
    } else {
        rect.adjust(-1, 0, 1, 0);
        painter->drawRect(rect);
    }
}

void TableItemDelegate::_drawIndicator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    int y = option.rect.y();
    int h = option.rect.height();
    int ph = (pressedRow == index.row()) ? qRound(0.35 * h) : qRound(0.257 * h);
    painter->setBrush(Theme::instance()->themeColor());
    painter->drawRoundedRect(4, ph + y, 3, h - 2 * ph, 1.5, 1.5);
}

void TableItemDelegate::_drawCheckBox(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    QVariant checkStateVar = index.data(Qt::CheckStateRole);
    Qt::CheckState checkState = static_cast<Qt::CheckState>(checkStateVar.toInt());

    bool isDark = Theme::instance()->isDarkTheme();

    qreal r = 4.5;
    qreal x = option.rect.x() + 15;
    qreal y = option.rect.center().y() - 9.5;
    QRectF rect(x, y, 19, 19);

    if (checkState == Qt::Unchecked) {
        painter->setBrush(QColor(0, 0, 0, isDark ? 26 : 6));
        painter->setPen(QColor(255, 255, 255, isDark ? 142 : 122)); // 修正：暗主题为白，亮为黑
        painter->drawRoundedRect(rect, r, r);
    } else {
        painter->setPen(Theme::instance()->themeColor()); // 未实现
        painter->setBrush(Theme::instance()->themeColor());
        painter->drawRoundedRect(rect, r, r);

        if (checkState == Qt::Checked) {
            // CheckBoxIcon::ACCEPT.render(painter, rect); // 未实现
        } else {
            // CheckBoxIcon::PARTIAL_ACCEPT.render(painter, rect); // 未实现
        }
    }

    painter->restore();
}

// TableWidget 实现
TableWidget::TableWidget(QWidget* parent) : TableBase<QTableWidget>(parent)
{
    auto scrollDelegate = new SmoothScrollDelegate(this);
    Q_UNUSED(scrollDelegate);

}

void TableWidget::setCurrentCell(int row, int column, QItemSelectionModel::SelectionFlags command) {
    if (command == QItemSelectionModel::NoUpdate) {
        QTableWidget::setCurrentCell(row, column);
    } else {
        QTableWidget::setCurrentCell(row, column, command);
    }
    updateSelectedRows();
}

void TableWidget::setCurrentItem(QTableWidgetItem* item, QItemSelectionModel::SelectionFlags command) {
    if (command == QItemSelectionModel::NoUpdate) {
        QTableWidget::setCurrentItem(item);
    } else {
        QTableWidget::setCurrentItem(item, command);
    }
    updateSelectedRows();
}

// TableView 实现
TableView::TableView(QWidget* parent) : TableBase<QTableView>(parent)
{
    auto scrollDelegate = new SmoothScrollDelegate(this);
    Q_UNUSED(scrollDelegate);
}
