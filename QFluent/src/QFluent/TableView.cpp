#include "TableView.h"

#include <QPainter>
#include <QScrollBar>
#include <QApplication>
#include <QMouseEvent>
#include <QHeaderView>
#include <QListView>

#include "Theme.h"
#include "QFluent/LineEdit.h"
#include "QFluent/ScrollBar.h"=============================
// TableItemDelegate Implementation
// ==========================================

TableItemDelegate::TableItemDelegate(QAbstractItemView* parent)
    : QStyledItemDelegate(parent)
    , m_margin(2)
    , m_hoverRow(-1)
    , m_pressedRow(-1)
{
}

void TableItemDelegate::setHoverRow(int row) {
    m_hoverRow = row;
}

void TableItemDelegate::setPressedRow(int row) {
    m_pressedRow = row;
}

void TableItemDelegate::setSelectedRows(const QList<QModelIndex>& indexes) {
    m_selectedRows.clear();
    m_selectedRows.reserve(indexes.size());
    for (const auto& index : indexes) {
        m_selectedRows.insert(index.row());
        if (index.row() == m_pressedRow) {
            m_pressedRow = -1;
        }
    }
}

QSize TableItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    // 增加高度边距
    size.setHeight(size.height() + m_margin * 2);
    return size;
}

QWidget* TableItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    LineEdit* lineEdit = new LineEdit(parent);
    lineEdit->setProperty("transparent", false);
    lineEdit->setText(option.text);
    lineEdit->setClearButtonEnabled(true);
    return lineEdit;
}

void TableItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QRect rect = option.rect;
    // 垂直居中
    int y = rect.y() + (rect.height() - editor->height()) / 2;
    int x = qMax(8, rect.x()); // 保持左侧最小边距
    int w = rect.width();

    if (index.column() == 0) {
        w -= 8;
    }

    // 确保宽度不为负
    w = qMax(0, w);

    editor->setGeometry(x, y, w, rect.height()); // 高度通常应适应 editor 的 sizeHint，或者填满 rect
}

void TableItemDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
    QStyledItemDelegate::initStyleOption(option, index);

    // 字体设置示例
    // option->font = Theme::instance()->font(13);

    QColor textColor = Theme::isDark() ? Qt::white : Qt::black;
    QVariant textBrushVar = index.data(Qt::ForegroundRole);

    if (textBrushVar.canConvert<QBrush>()) {
        QBrush textBrush = textBrushVar.value<QBrush>();
        if (textBrush.style() != Qt::NoBrush) {
            textColor = textBrush.color();
        }
    }

    option->palette.setColor(QPalette::Text, textColor);
    option->palette.setColor(QPalette::HighlightedText, textColor);
}

void TableItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);

    // 裁剪区域,防止绘制溢出
    painter->setClipping(true);
    painter->setClipRect(option.rect);

    QStyleOptionViewItem adjustedOption = option;
    adjustedOption.rect.adjust(0, m_margin, 0, -m_margin);

    // 状态判断
    bool isHover = (m_hoverRow == index.row());
    bool isPressed = (m_pressedRow == index.row());

    // 修正:检查 parent 是否存在
    const auto* tableView = qobject_cast<const QTableView*>(parent());
    bool isAlternate = tableView && (index.row() % 2 == 0) && tableView->alternatingRowColors();
    bool isDark = Theme::isDark();
    bool isSelected = m_selectedRows.contains(index.row());

    // 背景色 Alpha 值计算
    int alpha = 0;
    if (!isSelected) {
        if (isPressed)       alpha = isDark ? 9 : 6;
        else if (isHover)    alpha = 12;
        else if (isAlternate) alpha = 5;
    } else {
        if (isPressed)       alpha = isDark ? 15 : 9;
        else if (isHover)    alpha = 25;
        else                 alpha = 17;
    }

    int baseColorVal = isDark ? 255 : 0;

    // 绘制背景
    QVariant bgVar = index.data(Qt::BackgroundRole);
    if (bgVar.isValid() && bgVar.canConvert<QBrush>()) {
        painter->setBrush(bgVar.value<QBrush>());
    } else {
        painter->setBrush(QColor(baseColorVal, baseColorVal, baseColorVal, alpha));
    }

    drawBackground(painter, adjustedOption, index);

    // 绘制选中指示器 (仅当水平滚动条在最左侧时)
    if (isSelected && index.column() == 0) {
        QScrollBar* hScrollBar = nullptr;

        if (const auto* tableView = qobject_cast<const QTableView*>(parent())) {
            hScrollBar = tableView->horizontalScrollBar();
        } else if (const auto* listView = qobject_cast<const QListView*>(parent())) {
            hScrollBar = listView->horizontalScrollBar();
        }

        if (hScrollBar && hScrollBar->value() == 0) {
            drawIndicator(painter, adjustedOption, index);
        }
    }

    // 绘制复选框
    if (index.data(Qt::CheckStateRole).isValid()) {
        drawCheckBox(painter, adjustedOption, index);
    }

    painter->restore();

    // 调用基类绘制文本等内容
    QStyledItemDelegate::paint(painter, adjustedOption, index);
}

void TableItemDelegate::drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    int r = 5; // 圆角半径
    QRect rect = option.rect;
    int colCount = index.model()->columnCount(index.parent());

    // 处理圆角逻辑：第一列左圆角，最后一列右圆角
    if (index.column() == 0) {
        rect.adjust(4, 0, r + 1, 0);
        painter->drawRoundedRect(rect, r, r);
    } else if (index.column() == colCount - 1) {
        rect.adjust(-r - 1, 0, -4, 0);
        painter->drawRoundedRect(rect, r, r);
    } else {
        rect.adjust(-1, 0, 1, 0); // 中间列稍微重叠以避免缝隙
        painter->drawRect(rect);
    }
}

void TableItemDelegate::drawIndicator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    int y = option.rect.y();
    int h = option.rect.height();
    // 根据按下状态调整指示器高度
    int ph = (m_pressedRow == index.row()) ? qRound(0.35 * h) : qRound(0.257 * h);

    painter->setBrush(Theme::instance()->themeColor());
    // 居中绘制小的指示条
    painter->drawRoundedRect(4, ph + y, 3, h - 2 * ph, 1.5, 1.5);
}

void TableItemDelegate::drawCheckBox(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    Qt::CheckState checkState = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
    bool isDark = Theme::isDark();

    qreal r = 4.5; // 圆角
    // 居中计算
    qreal x = option.rect.x() + 15;
    qreal y = option.rect.center().y() - 9.5;
    QRectF rect(x, y, 19, 19);

    if (checkState == Qt::Unchecked) {
        painter->setBrush(QColor(0, 0, 0, isDark ? 26 : 6));
        painter->setPen(QColor(255, 255, 255, isDark ? 142 : 122));
        painter->drawRoundedRect(rect, r, r);
    } else {
        // 选中或半选状态
        QColor themeColor = Theme::instance()->themeColor();
        painter->setPen(themeColor);
        painter->setBrush(themeColor);
        painter->drawRoundedRect(rect, r, r);

        // TODO: 绘制具体的对勾图标
        // 由于原代码中 CheckBoxIcon 未提供定义，这里建议使用 drawPolyline 或 QIcon 绘制
        painter->setPen(isDark ? Qt::black : Qt::white);
        if (checkState == Qt::Checked) {
            // 简单绘制对勾
            QVector<QPointF> points = {
                rect.topLeft() + QPointF(4, 9),
                rect.topLeft() + QPointF(8, 13),
                rect.topLeft() + QPointF(14, 5)
            };
            painter->drawPolyline(points);
        } else if (checkState == Qt::PartiallyChecked) {
            // 绘制横线
            painter->drawLine(rect.center() - QPointF(5, 0), rect.center() + QPointF(5, 0));
        }
    }

    painter->restore();
}

int TableItemDelegate::pressedRow() const
{
    return m_pressedRow;
}

// ==========================================
// TableWidget Implementation
// ==========================================

TableWidget::TableWidget(QWidget* parent) : TableBase<QTableWidget>(parent)
{
    setHorizontalScrollBar(new ScrollBar(this));
    setVerticalScrollBar(new ScrollBar(this));
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

// ==========================================
// TableView Implementation
// ==========================================

TableView::TableView(QWidget* parent) : TableBase<QTableView>(parent)
{
    setHorizontalScrollBar(new ScrollBar(this));
    setVerticalScrollBar(new ScrollBar(this));
}
