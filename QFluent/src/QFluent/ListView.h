#pragma once

#include <QSet>
#include <QList>
#include <QListView>
#include <QListWidget>
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QItemSelectionModel>

class QPainter;
class QEvent;
class QKeyEvent;
class QResizeEvent;
class QMouseEvent;
class QStyleOptionViewItem;
class QWidget;
class QListWidgetItem;

#include "TableView.h"
#include "StyleSheet.h"
#include "FluentGlobal.h"

/**
 * @brief 自定义列表项代理，继承自 TableItemDelegate 以复用大部分逻辑
 */
class ListItemDelegate : public TableItemDelegate {
    Q_OBJECT
public:
    explicit ListItemDelegate(QAbstractItemView* parent = nullptr);

    void setCheckedColor(const QColor& lightColor, const QColor& darkColor);

protected:
    void drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void drawIndicator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    QColor m_lightCheckedColor;
    QColor m_darkCheckedColor;
};


/**
 * @brief 列表基类模板，用于混入 QListView 或 QListWidget
 * 注意：模板类的实现必须放在头文件中
 */
template <typename Base>
class ListBase : public Base {
public:
    explicit ListBase(QWidget* parent = nullptr)
        : Base(parent)
        , m_delegate(new ListItemDelegate(this))
        , m_isRightClickSelection(false)
    {
        // 样式注册
        StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::LIST_VIEW);

        // 初始化UI
        this->setItemDelegate(m_delegate);
        this->setMouseTracking(true);

        QObject::connect(this, &QAbstractItemView::entered, [this](const QModelIndex& index) {
            setHoverRow(index.row());
        });
        QObject::connect(this, &QAbstractItemView::pressed, [this](const QModelIndex& index) {
            setPressedRow(index.row());
        });
    }

    virtual ~ListBase() = default;

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
        if (event->button() == Qt::LeftButton || m_isRightClickSelection) {
            Base::mousePressEvent(event);
            return;
        }

        QModelIndex index = this->indexAt(event->pos());
        if (index.isValid()) {
            setPressedRow(index.row());
        }

        QWidget::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        Base::mouseReleaseEvent(event);
        updateSelectedRows();

        if (this->indexAt(event->pos()).row() < 0 || event->button() == Qt::RightButton) {
            setPressedRow(-1);
        }
    }

    // --- 选中状态管理 ---

    void clearSelection() {
        Base::clearSelection();
        updateSelectedRows();
    }

    void setCurrentIndex(const QModelIndex& index) {
        Base::setCurrentIndex(index);
        updateSelectedRows();
    }

    void setItemDelegate(ListItemDelegate* delegate) {
        m_delegate = delegate;
        Base::setItemDelegate(delegate);
    }

    // --- 属性 & Setters ---

    void setCheckedColor(const QColor& light, const QColor& dark) {
        m_delegate->setCheckedColor(light, dark);
    }

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

protected:
    ListItemDelegate* m_delegate;
    bool m_isRightClickSelection;
};


class QFLUENT_EXPORT ListWidget : public ListBase<QListWidget> {
    Q_OBJECT
    Q_PROPERTY(bool selectRightClickedRow READ isRightClickSelectionEnabled WRITE setRightClickSelectionEnabled)

public:
    explicit ListWidget(QWidget* parent = nullptr);

    void setCurrentItem(QListWidgetItem* item, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::NoUpdate);
    void setCurrentRow(int row, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::NoUpdate);
};

class QFLUENT_EXPORT ListView : public ListBase<QListView> {
    Q_OBJECT
    Q_PROPERTY(bool selectRightClickedRow READ isRightClickSelectionEnabled WRITE setRightClickSelectionEnabled)

public:
    explicit ListView(QWidget* parent = nullptr);
};
