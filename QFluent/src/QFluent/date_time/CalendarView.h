#ifndef CALENDAR_VIEW_H
#define CALENDAR_VIEW_H

#include <QStyledItemDelegate>
#include <QEasingCurve>
#include <QListWidget>
#include <QScrollBar>
#include <QFrame>
#include <QDate>

#include "Define.h"
#include "QFluent/ToolButton.h"

using FIF = FluentIconType::IconType;

class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QStackedWidget;
class QPropertyAnimation;
class QParallelAnimationGroup;
class QGraphicsDropShadowEffect;
class SmoothScrollBar : public QScrollBar {
    Q_OBJECT
public:
    SmoothScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);
    void setScrollAnimation(int duration, QEasingCurve curve = QEasingCurve());
    void scrollTo(int value);
    void setForceHidden(bool hidden);
    QPropertyAnimation* ani() const;

private:
    QPropertyAnimation* m_ani;
};



class ScrollButton : public TransparentToolButton {
    Q_OBJECT
public:
    ScrollButton(const FluentIconBase &icon, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};




class ScrollItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    ScrollItemDelegate(QDate min, QDate max, QObject* parent = nullptr);
    void setRange(QDate min, QDate max);
    void setPressedIndex(const QModelIndex& index);
    void setCurrentIndex(const QModelIndex& index);
    void setSelectedIndex(const QModelIndex& index);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
protected:
    virtual void _drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void _drawText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual int _itemMargin() const { return 0; }

    QDate min, max;
    QFont font;
    QModelIndex pressedIndex;
    QModelIndex currentIndex;
    QModelIndex selectedIndex;
};




class YearScrollItemDelegate : public ScrollItemDelegate {
    Q_OBJECT
public:
    YearScrollItemDelegate(QDate min, QDate max, QObject* parent = nullptr) : ScrollItemDelegate(min, max, parent) {}
protected:
    int _itemMargin() const override { return 8; }
};

class DayScrollItemDelegate : public ScrollItemDelegate {
    Q_OBJECT
public:
    DayScrollItemDelegate(QDate min, QDate max, QObject* parent = nullptr) : ScrollItemDelegate(min, max, parent) {}
protected:
    int _itemMargin() const override { return 3; }
};

class ScrollViewBase : public QListWidget {
    Q_OBJECT
signals:
    void pageChanged(int page);
    void itemClicked(const QDate& date);
public:
    ScrollViewBase(ScrollItemDelegate* delegateType, QWidget* parent = nullptr);
    void scrollUp();
    void scrollDown();
    virtual void scrollToPage(int page);
    virtual std::pair<QDate, QDate> currentPageRange();
    virtual void setDate(const QDate& date);
    virtual void scrollToDate(const QDate& date);
protected:
    // virtual void initItems();
    void initWidget();
    void _onFirstScrollFinished();
    void _setPressedIndex(const QModelIndex& index);
    void _setSelectedIndex(const QModelIndex& index);
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    virtual QSize gridSize() const { return QSize(76, 76); }
    virtual int cols() const { return 4; }
    virtual int pageRows() const { return 3; }

    int currentPage = 0;
    SmoothScrollBar* vScrollBar;
    ScrollItemDelegate* delegate;
    QDate currentDate;
    QDate date;
    int minYear;
    int maxYear;
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
    void _onScrollUp();
    void _onScrollDown();
    virtual void _updateTitle();
    QPushButton* titleButton;
    ScrollButton* resetButton;
    ScrollButton* upButton;
    ScrollButton* downButton;
    ScrollViewBase* scrollView = nullptr;
    QHBoxLayout* hBoxLayout;
    QVBoxLayout* vBoxLayout;
};

class YearScrollView : public ScrollViewBase {
    Q_OBJECT
public:
    YearScrollView(QWidget* parent = nullptr);
    std::pair<QDate, QDate> currentPageRange() override;

protected:
    void initItems();
    void scrollToDate(const QDate& date) override;

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;

};

class YearCalendarView : public CalendarViewBase {
    Q_OBJECT
public:
    YearCalendarView(QWidget* parent = nullptr);
protected:
    void _updateTitle() override;
    QDate currentPageDate() const override { return QDate(); }

};

class MonthScrollView : public ScrollViewBase {
    Q_OBJECT
public:
    MonthScrollView(QWidget* parent = nullptr);
public:
    void initItems();
    void scrollToDate(const QDate& date) override;
    std::pair<QDate, QDate> currentPageRange() override;
    QStringList months;

protected:
    void mouseReleaseEvent(QMouseEvent* e) override;
};

class MonthCalendarView : public CalendarViewBase {
    Q_OBJECT
public:
    MonthCalendarView(QWidget* parent = nullptr);
public:
    void _updateTitle() override;
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
    QDate _pageToDate();
    int _dateToRow(const QDate& date);
    void mouseReleaseEvent(QMouseEvent* e) override;
    int cols() const override { return 7; }
    int pageRows() const override { return 4; }

private:
    QHBoxLayout* vBoxLayout;
    QWidget* weekDayGroup;
    QHBoxLayout* weekDayLayout;
};

class DayCalendarView : public CalendarViewBase {
    Q_OBJECT
public:
    DayCalendarView(QWidget* parent = nullptr);
    void initialize();
    void scrollToDate(const QDate& date);
    QDate currentPageDate() const override;

protected:
    void _updateTitle() override;
};

class QFLUENT_EXPORT CalendarView : public QWidget {
    Q_OBJECT
signals:
    void resetted();
    void dateChanged(const QDate& date);
public:
    CalendarView(QWidget* parent = nullptr);
    void setShadowEffect(int blurRadius = 30, QPoint offset = QPoint(0, 8), QColor color = QColor(0, 0, 0, 30));
    bool isResetEnabled() const;
    void setResetEnabled(bool isEnabled);
    void setDate(const QDate& date);
    void exec(const QPoint& pos, bool ani = true);
protected:
    void initWidget();
    void _onResetted();
    void _onDayViewTitleClicked();
    void _onMonthTitleClicked();
    void _onMonthItemClicked(const QDate& date);
    void _onYearItemClicked(const QDate& date);
    void _onDayItemClicked(const QDate& date);
private:
    QHBoxLayout* hBoxLayout;
    QDate date;
    bool _isResetEnabled = false;
    QStackedWidget* stackedWidget;
    YearCalendarView* yearView;
    MonthCalendarView* monthView;
    DayCalendarView* dayView;
    QPropertyAnimation* opacityAni;
    QPropertyAnimation* slideAni;
    QParallelAnimationGroup* aniGroup;
    QGraphicsDropShadowEffect* shadowEffect;
};

#endif // CALENDAR_VIEW_H
