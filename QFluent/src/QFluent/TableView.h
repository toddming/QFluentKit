#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <QStyledItemDelegate>
#include <QTableView>
#include <QTableWidget>
#include <QKeyEvent>
#include <QModelIndex>
#include <QSet>
#include <QWidget>
#include <QApplication>
#include <QHeaderView>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>

#include "Theme.h"
#include "Property.h"
#include "StyleSheet.h"


class QPainter;
class TableItemDelegate : public QStyledItemDelegate {
public:
    explicit TableItemDelegate(QTableView* parent = nullptr);

    void setHoverRow(int row);
    void setPressedRow(int row);
    void setSelectedRows(const QList<QModelIndex>& indexes);

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    void _drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void _drawIndicator(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void _drawCheckBox(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    int margin = 2;
    int hoverRow = -1;
    int pressedRow = -1;
    QSet<int> selectedRows;
};

template <typename Base>
class TableBase : public Base {
public:
    explicit TableBase(QWidget* parent = nullptr) : Base(parent),
        delegate(new TableItemDelegate(this)),

        _isSelectRightClickedRow(false) {

        StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::TABLE_VIEW);

        this->setShowGrid(false);
        this->setMouseTracking(true);
        this->setAlternatingRowColors(true);
        this->setItemDelegate(delegate);
        this->setSelectionBehavior(QAbstractItemView::SelectRows);
        this->horizontalHeader()->setHighlightSections(false);
        this->verticalHeader()->setHighlightSections(false);
        this->verticalHeader()->setDefaultSectionSize(38);

        QObject::connect(this, &QAbstractItemView::entered, [this](const QModelIndex& index) {
            _setHoverRow(index.row());
        });
        QObject::connect(this, &QAbstractItemView::pressed, [this](const QModelIndex& index) {
            _setPressedRow(index.row());
        });
        QObject::connect(this->verticalHeader(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
            this->selectRow(logicalIndex);
        });
    }

    ~TableBase() {
        delete delegate;
    }

    void setBorderVisible(bool isVisible) {
        this->setProperty("isBorderVisible", isVisible);
        this->setStyle(QApplication::style());
    }

    void setBorderRadius(int radius) {
        // QString qss = QString("QTableView{border-radius: %1px}").arg(radius);
        // // setCustomStyleSheet(this, qss, qss); // 未实现
        // this->setProperty("lightCustomQss", qss);
        // this->setProperty("darkCustomQss", qss);
    }

    void leaveEvent(QEvent* event) override {
        Base::leaveEvent(event);
        _setHoverRow(-1);
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
        if (event->button() == Qt::LeftButton || _isSelectRightClickedRow) {
            Base::mousePressEvent(event);
            return;
        }

        QModelIndex index = this->indexAt(event->pos());
        if (index.isValid()) {
            _setPressedRow(index.row());
        }

        QWidget::mousePressEvent(event); // 调用基类QWidget版本
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        Base::mouseReleaseEvent(event);
        updateSelectedRows();

        if (this->indexAt(event->pos()).row() < 0 || event->button() == Qt::RightButton) {
            _setPressedRow(-1);
        }
    }

    void setItemDelegate(TableItemDelegate* delegate) {
        this->delegate = delegate;
        Base::setItemDelegate(delegate);
    }

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

    bool isSelectRightClickedRow() const {
        return _isSelectRightClickedRow;
    }

    void setSelectRightClickedRow(bool isSelect) {
        _isSelectRightClickedRow = isSelect;
    }

protected:
    void _setHoverRow(int row) {
        delegate->setHoverRow(row);
        this->viewport()->update();
    }

    void _setPressedRow(int row) {
        if (this->selectionMode() == QAbstractItemView::NoSelection) {
            return;
        }
        delegate->setPressedRow(row);
        this->viewport()->update();
    }

    void _setSelectedRows(const QList<QModelIndex>& indexes) {
        if (this->selectionMode() == QAbstractItemView::NoSelection) {
            return;
        }
        delegate->setSelectedRows(indexes);
        this->viewport()->update();
    }

    void updateSelectedRows() {
        _setSelectedRows(this->selectedIndexes());
    }

protected:
    TableItemDelegate* delegate;
    bool _isSelectRightClickedRow;
};

class QFLUENT_EXPORT TableWidget : public TableBase<QTableWidget> {
public:
    explicit TableWidget(QWidget* parent = nullptr);

    void setCurrentCell(int row, int column, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::NoUpdate);
    void setCurrentItem(QTableWidgetItem* item, QItemSelectionModel::SelectionFlags command = QItemSelectionModel::NoUpdate);
};

class QFLUENT_EXPORT TableView : public TableBase<QTableView> {
public:
    explicit TableView(QWidget* parent = nullptr);
};

#endif // TABLE_VIEW_H
