#pragma once

#include <QListWidget>
#include <QToolButton>
#include "FluentIcon.h"

class ScrollButton : public QToolButton
{
    Q_OBJECT

public:
    explicit ScrollButton(const FluentIconBase &icon, QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    bool m_isPressed;
    std::unique_ptr<FluentIconBase> m_fluentIcon;
};

class SmoothScrollBar;
class CycleListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit CycleListWidget(const QStringList& items, const QSize& itemSize, 
                           Qt::Alignment align = Qt::AlignCenter, QWidget* parent = nullptr);

    void setItems(const QStringList& items);
    void setSelectedItem(const QString& text);
    void scrollToItem(QListWidgetItem* item, QAbstractItemView::ScrollHint hint = QAbstractItemView::PositionAtCenter);
    
    QListWidgetItem* currentItem();
    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);

signals:
    void currentItemChanged(QListWidgetItem* item);

public slots:
    void scrollDown();
    void scrollUp();

protected:
    void wheelEvent(QWheelEvent* e) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *e) override;
#else
    void enterEvent(QEvent *e) override;
#endif
    void leaveEvent(QEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

private slots:
    void onItemClicked(QListWidgetItem* item);

private:
    void createItems(const QStringList& items);
    void addColumnItems(const QStringList& items, bool disabled = false);

    QSize m_itemSize;
    Qt::Alignment m_align;
    ScrollButton* m_upButton;
    ScrollButton* m_downButton;
    int m_scrollDuration;
    QStringList m_originItems;
    SmoothScrollBar* m_vScrollBar;
    int m_visibleNumber;
    int m_currentIndex;
    bool m_isCycle;
};

