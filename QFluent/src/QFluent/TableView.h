#pragma once

#include <QTableView>
#include <QTableWidget>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QSet>
#include <QList>
#include <QModelIndex>
#include <QMouseEvent>

class QPainter;
class QEvent;
class QKeyEvent;
class QResizeEvent;
class QMouseEvent;
class QStyleOptionViewItem;
class QWidget;
class TableItemDelegate;

#include "FluentGlobal.h"
#include "StyleSheet.h"

/**
 * @brief 自定义表格项代理，处理绘制和编辑
 */
class TableItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit TableItemDelegate(QAbstractItemView* parent = nullptr);

    void setHoverRow(int row);
    void setPressedRow(int row);
    void setSelectedRows(const QList<QModelIndex>& indexes);

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    int pressedRow() const;

private:
    virtual void drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void drawIndicator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void drawCheckBox(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    int m_margin;
    int m_hoverRow;
    int m_pressedRow;
    QSet<int> m_selectedRows;
};

/**
 * @brief 表格基类模板，用于混入 QTableView 或 QTableWidget
 * 注意：模板类的实现必须放在头文件中
 */
template <typename Base>
class TableBase : public Base {
public:
    explicit TableBase(QWidget* parent = nullptr)
        : Base(parent)
        , m_delegate(new TableItemDelegate(this)) // delegate 父对象为 this，无需手动 delete
        , m_isRightClickSelection(false)
    {
        // 注册样式
        StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::TABLE_VIEW);

        setupUI();
        setupConnections();
    }

    virtual ~TableBase() = default;

    void setBorderVisible(bool isVisible) {
        this->setProperty("isBorderVisible", isVisible);
        this->style()->polish(this); // 强制刷新样式
    }

    // 预留接口
    void setBorderRadius(int radius) {
        Q_UNUSED(radius);
        // this->setProperty("lightCustomQss", ...);
    }

    void setItemDelegate(TableItemDelegate* delegate) {
        m_delegate = delegate;
        Base::setItemDelegate(delegate);
    }

    // --- 事件重写 ---

    void leaveEvent(QEvent* event) override {
        Base::leaveEvent(event);
        setHoverRow(-1);
    }

    void resizeEvent(QResizeEvent* event) override {
        Base::resizeEvent(event);
        this->viewport()->update();
    }

    void keyPressEvent(QKeyEvent* event) override {
        Base::keyPressEvent(event);
        updateSelectedRows();
    }

    void mousePressEvent(QMouseEvent* event) override {
        // 如果是左键或者是启用了右键选中
        if (event->button() == Qt::LeftButton || m_isRightClickSelection) {
            Base::mousePressEvent(event);
            return;
        }

        // 处理其他情况（如右键且未启用选中），仅更新按下状态但不改变选中项
        QModelIndex index = this->indexAt(event->pos());
        if (index.isValid()) {
            setPressedRow(index.row());
        }

        // 调用 QWidget 的处理而不是 Base (QTableView) 的处理，以避免触发默认的选择行为
        QWidget::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        Base::mouseReleaseEvent(event);
        updateSelectedRows();

        // 点击空白处或右键释放时清除按下状态
        if (this->indexAt(event->pos()).row() < 0 || event->button() == Qt::RightButton) {
            setPressedRow(-1);
        }
    }

    // --- 选中状态管理 ---

    void selectAll() override {
        Base::selectAll();
        updateSelectedRows();
    }

    void selectRow(int row) {
        Base::selectRow(row);
        updateSelectedRows();
    }

    void clearSelection() {
        Base::clearSelection();
        updateSelectedRows();
    }

    void setCurrentIndex(const QModelIndex& index) {
        Base::setCurrentIndex(index);
        updateSelectedRows();
    }

    // --- Getters & Setters ---

    bool isRightClickSelectionEnabled() const {
        return m_isRightClickSelection;
    }

    void setRightClickSelectionEnabled(bool enabled) {
        m_isRightClickSelection = enabled;
    }

protected:
    void setHoverRow(int row) {
        m_delegate->setHoverRow(row);
        this->viewport()->update();
    }

    void setPressedRow(int row) {
        if (this->selectionMode() == QAbstractItemView::NoSelection) {
            return;
        }
        m_delegate->setPressedRow(row);
        this->viewport()->update();
    }

    void setSelectedRows(const QList<QModelIndex>& indexes) {
        if (this->selectionMode() == QAbstractItemView::NoSelection) {
            return;
        }
        m_delegate->setSelectedRows(indexes);
        this->viewport()->update();
    }

    void updateSelectedRows() {
        setSelectedRows(this->selectedIndexes());
    }

private:
    void setupUI() {
        this->setShowGrid(false);
        this->setMouseTracking(true);
        this->setAlternatingRowColors(true);
        this->setItemDelegate(m_delegate);
        this->setSelectionBehavior(QAbstractItemView::SelectRows);
        this->horizontalHeader()->setHighlightSections(false);
        this->verticalHeader()->setHighlightSections(false);
        this->verticalHeader()->setDefaultSectionSize(38);
    }

    void setupConnections() {
        QObject::connect(this, &QAbstractItemView::entered, [this](const QModelIndex& index) {
            setHoverRow(index.row());
        });
        QObject::connect(this, &QAbstractItemView::pressed, [this](const QModelIndex& index) {
            setPressedRow(index.row());
        });
        QObject::connect(this->verticalHeader(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
            this->selectRow(logicalIndex);
        });
    }

protected:
    TableItemDelegate* m_delegate;
    bool m_isRightClickSelection;
};

// 导出类定义

class QFLUENT_EXPORT TableWidget : public TableBase<QTableWidget> {
    Q_OBJECT
public:
    explicit TableWidget(QWidget* parent = nullptr);

    void setCurrentCell(int row, int column, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::NoUpdate);
    void setCurrentItem(QTableWidgetItem* item, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::NoUpdate);
};

class QFLUENT_EXPORT TableView : public TableBase<QTableView> {
    Q_OBJECT
public:
    explicit TableView(QWidget* parent = nullptr);
};

