#pragma once

#include <QStyledItemDelegate>
#include <QEasingCurve>
#include <QListWidget>
#include <QScrollBar>
#include <QFrame>
#include <QDate>

#include "FluentGlobal.h"
#include "QFluent/ToolButton.h"

class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QStackedWidget;
class QPropertyAnimation;
class QParallelAnimationGroup;
class QGraphicsDropShadowEffect;

class ViewScrollBar : public QScrollBar {
    Q_OBJECT
public:
    ViewScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);
    void setScrollAnimation(int duration, QEasingCurve curve = QEasingCurve());
    void scrollTo(int value);
    void setForceHidden(bool hidden);
    QPropertyAnimation* ani() const;

private:
    QPropertyAnimation* m_animation;
};


class CalendarButton : public TransparentToolButton {
    Q_OBJECT
public:
    CalendarButton(const QIcon &icon, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};


class ScrollItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ScrollItemDelegate(QDate minDate, QDate maxDate, QObject* parent = nullptr);
    void setRange(QDate minDate, QDate maxDate);
    void setPressedIndex(const QModelIndex& index);
    void setCurrentIndex(const QModelIndex& index);
    void setSelectedIndex(const QModelIndex& index);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
    virtual void drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void drawText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual int itemMargin() const { return 0; }

    QDate m_minDate;
    QDate m_maxDate;
    QFont m_font;
    QModelIndex m_pressedIndex;
    QModelIndex m_currentIndex;
    QModelIndex m_selectedIndex;
};


class YearScrollItemDelegate : public ScrollItemDelegate {
    Q_OBJECT
public:
    YearScrollItemDelegate(QDate minDate, QDate maxDate, QObject* parent = nullptr)
        : ScrollItemDelegate(minDate, maxDate, parent) {}
protected:
    int itemMargin() const override { return 8; }
};


class DayScrollItemDelegate : public ScrollItemDelegate {
    Q_OBJECT
public:
    DayScrollItemDelegate(QDate minDate, QDate maxDate, QObject* parent = nullptr)
        : ScrollItemDelegate(minDate, maxDate, parent) {}
protected:
    int itemMargin() const override { return 3; }
};


class ScrollViewBase : public QListWidget {
    Q_OBJECT
signals:
    void pageChanged(int page);
    void itemClicked(const QDate& date);

public:
    ScrollViewBase(ScrollItemDelegate* delegateType, QWidget* parent = nullptr);
    ~ScrollViewBase() override;
    void scrollUp();
    void scrollDown();
    virtual void scrollToPage(int page);
    virtual std::pair<QDate, QDate> currentPageRange();
    virtual void setDate(const QDate& date);
    virtual void scrollToDate(const QDate& date);

protected:
    void initWidget();
    void onFirstScrollFinished();
    void setPressedIndex(const QModelIndex& index);
    void setSelectedIndex(const QModelIndex& index);
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    virtual QSize gridSize() const { return QSize(76, 76); }
    virtual int cols() const { return 4; }
    virtual int pageRows() const { return 3; }

    int m_currentPage;
    ViewScrollBar* m_vScrollBar;
    ScrollItemDelegate* m_delegate;
    QDate m_currentDate;
    QDate m_date;
    int m_minYear;
    int m_maxYear;
};


class CalendarViewBase : public QFrame {
    Q_OBJECT
signals:
    void resetted();
    void titleClicked();
    void itemClicked(const QDate& date);

public:
    CalendarViewBase(QWidget* parent = nullptr);
    void setScrollView(ScrollViewBase* view);
    void setResetEnabled(bool isEnabled);
    bool isResetEnabled() const;
    void setDate(const QDate& date);
    void setTitle(const QString& title);
    virtual QDate currentPageDate() const = 0;

protected:
    void initWidget();
    void onScrollUp();
    void onScrollDown();
    virtual void updateTitle();

    QPushButton* m_titleButton;
    CalendarButton* m_resetButton;
    CalendarButton* m_upButton;
    CalendarButton* m_downButton;
    ScrollViewBase* m_scrollView;
    QHBoxLayout* m_hBoxLayout;
    QVBoxLayout* m_vBoxLayout;
};


class YearScrollView : public ScrollViewBase {
    Q_OBJECT
public:
    YearScrollView(QWidget* parent = nullptr);
    std::pair<QDate, QDate> currentPageRange() override;

protected:
    void initItems();
    void scrollToDate(const QDate& date) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
};


class YearCalendarView : public CalendarViewBase {
    Q_OBJECT
public:
    YearCalendarView(QWidget* parent = nullptr);

protected:
    void updateTitle() override;
    QDate currentPageDate() const override { return QDate(); }
};


class MonthScrollView : public ScrollViewBase {
    Q_OBJECT
public:
    MonthScrollView(QWidget* parent = nullptr);
    void initItems();
    void scrollToDate(const QDate& date) override;
    std::pair<QDate, QDate> currentPageRange() override;

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;

    QStringList m_months;
};


class MonthCalendarView : public CalendarViewBase {
    Q_OBJECT
public:
    MonthCalendarView(QWidget* parent = nullptr);

    void updateTitle() override;
    QDate currentPageDate() const override;
};


class DayScrollView : public ScrollViewBase {
    Q_OBJECT
public:
    DayScrollView(QWidget* parent = nullptr);
    void initialize();
    QSize gridSize() const override { return QSize(44, 44); }
    void setDate(const QDate& date) override;
    void scrollToDate(const QDate& date) override;
    void scrollToPage(int page) override;
    std::pair<QDate, QDate> currentPageRange() override;

protected:
    void initItems();
    QDate pageToDate();
    int dateToRow(const QDate& date);
    void mouseReleaseEvent(QMouseEvent* e) override;
    int cols() const override { return 7; }
    int pageRows() const override { return 4; }

private:
    QHBoxLayout* m_hBoxLayout;
    QWidget* m_weekDayGroup;
    QHBoxLayout* m_weekDayLayout;
};


class DayCalendarView : public CalendarViewBase {
    Q_OBJECT
public:
    DayCalendarView(QWidget* parent = nullptr);
    void initialize();
    void scrollToDate(const QDate& date);
    QDate currentPageDate() const override;

protected:
    void updateTitle() override;
};


class QFLUENT_EXPORT CalendarView : public QWidget {
    Q_OBJECT
signals:
    void resetted();
    void dateChanged(const QDate& date);

public:
    CalendarView(QWidget* parent = nullptr);
    ~CalendarView() override;
    void setShadowEffect(int blurRadius = 30, QPoint offset = QPoint(0, 8), QColor color = QColor(0, 0, 0, 30));
    bool isResetEnabled() const;
    void setResetEnabled(bool isEnabled);
    void setDate(const QDate& date);
    void exec(const QPoint& pos, bool ani = true);

protected:
    void initWidget();
    void onResetted();
    void onDayViewTitleClicked();
    void onMonthTitleClicked();
    void onMonthItemClicked(const QDate& date);
    void onYearItemClicked(const QDate& date);
    void onDayItemClicked(const QDate& date);

private:
    QHBoxLayout* m_hBoxLayout;
    QDate m_date;
    bool m_isResetEnabled;
    QStackedWidget* m_stackedWidget;
    YearCalendarView* m_yearView;
    MonthCalendarView* m_monthView;
    DayCalendarView* m_dayView;
    QPropertyAnimation* m_opacityAnimation;
    QPropertyAnimation* m_slideAnimation;
    QParallelAnimationGroup* m_animationGroup;
    QGraphicsDropShadowEffect* m_shadowEffect;
};
