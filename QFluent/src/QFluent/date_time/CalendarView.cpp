#include "CalendarView.h"
#include <QLocale>
#include <QTimer>
#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QLabel>
#include <QWidget>
#include <QStackedWidget>
#include <QGraphicsDropShadowEffect>
#include <QListView>
#include <QPainter>
#include <QColor>
#include <QCursor>
#include <QRectF>
#include <QModelIndex>
#include <QCalendar>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QPoint>
#include <QRect>
#include <QStringListModel>
#include <QSize>
#include <QtMath>
#include <QWheelEvent>
#include <QMouseEvent>
#include <memory>

#include "Screen.h"
#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"


ViewScrollBar::ViewScrollBar(Qt::Orientation orientation, QWidget* parent)
    : QScrollBar(orientation, parent) {
    m_ani = new QPropertyAnimation(this, "value", this);
    m_ani->setDuration(300);
    m_ani->setEasingCurve(QEasingCurve::OutCubic);
}

void ViewScrollBar::setScrollAnimation(int duration, QEasingCurve curve) {
    m_ani->setDuration(duration);
    m_ani->setEasingCurve(curve);
}

void ViewScrollBar::scrollTo(int value) {
    m_ani->stop();
    m_ani->setStartValue(this->value());
    m_ani->setEndValue(value);
    m_ani->start();
}

void ViewScrollBar::setForceHidden(bool hidden) {
    this->setVisible(!hidden);
}

QPropertyAnimation* ViewScrollBar::ani() const
{
    return m_ani;
}


ScrollButton::ScrollButton(const FluentIconBase &icon, QWidget* parent)
    : TransparentToolButton(icon, parent) {
}

void ScrollButton::paintEvent(QPaintEvent* event)
{
    QToolButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (!isEnabled()) {
        painter.setOpacity(0.43);
    } else if (isPressed()) {
        painter.setOpacity(0.63);
    }

    int w = isPressed() ? 12 : 11;
    int h = isPressed() ? 12 : 11;
    int y = (height() - h) / 2;
    int x = (width() - w) / 2;

    QMap<QString, QString> attrs;
    attrs["fill"] = Theme::instance()->isDarkTheme() ? "#5e5e5e" : "#9c9c9c";
    FluentIconUtils::drawIcon(*fluentIcon(), &painter, QRectF(x, y, w, h), Fluent::ThemeMode::AUTO, QIcon::Off, attrs);
}


ScrollItemDelegate::ScrollItemDelegate(QDate min, QDate max, QObject* parent)
    : QStyledItemDelegate(parent), min(min), max(max) {}

void ScrollItemDelegate::setRange(QDate min, QDate max) {
    this->min = min;
    this->max = max;
}

void ScrollItemDelegate::setPressedIndex(const QModelIndex& index) {
    pressedIndex = index;
}

void ScrollItemDelegate::setCurrentIndex(const QModelIndex& index) {
    currentIndex = index;
}

void ScrollItemDelegate::setSelectedIndex(const QModelIndex& index) {
    selectedIndex = index;
}

void ScrollItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    _drawBackground(painter, option, index);
    _drawText(painter, option, index);
}

void ScrollItemDelegate::_drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    if (index != selectedIndex) {
        painter->setPen(Qt::NoPen);
    } else {
        painter->setPen(Theme::instance()->themeColor());
    }

    if (index == currentIndex) {
        if (index == pressedIndex) {
            painter->setBrush(Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_2));
        } else if (option.state & QStyle::State_MouseOver) {
            painter->setBrush(Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_1));
        } else {
            painter->setBrush(Theme::instance()->themeColor());
        }
    } else {
        int c = Theme::instance()->isDarkTheme() ? 255 : 0;
        if (index == pressedIndex) {
            painter->setBrush(QColor(c, c, c, 7));
        } else if (option.state & QStyle::State_MouseOver) {
            painter->setBrush(QColor(c, c, c, 9));
        } else {
            painter->setBrush(Qt::transparent);
        }
    }

    int m = _itemMargin();
    painter->drawEllipse(option.rect.adjusted(m, m, -m, -m));
    painter->restore();
}

void ScrollItemDelegate::_drawText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    painter->setFont(font);

    if (index == currentIndex) {
        painter->setPen(Theme::instance()->isDarkTheme() ? Qt::black : Qt::white);
    } else {
        painter->setPen(Theme::instance()->isDarkTheme() ? Qt::white : Qt::black);
        if (!((min <= index.data(Qt::UserRole).toDate() && index.data(Qt::UserRole).toDate() <= max )|| (option.state & QStyle::State_MouseOver)) || index == pressedIndex) {
            painter->setOpacity(0.6);
        }
    }

    QString text = index.data(Qt::DisplayRole).toString();
    painter->drawText(option.rect, Qt::AlignCenter, text);
    painter->restore();
}

// ScrollViewBase
ScrollViewBase::ScrollViewBase(ScrollItemDelegate* delegateType, QWidget* parent)
    : QListWidget(parent), delegate(delegateType), currentDate(QDate::currentDate()), date(QDate::currentDate()),
      minYear(currentDate.year() - 10), maxYear(currentDate.year() + 10) {

    setMouseTracking(true);
    vScrollBar = new ViewScrollBar(Qt::Vertical, this);
    setVerticalScrollBar(vScrollBar);
}


void ScrollViewBase::initWidget() {
    setSpacing(0);
    setMovement(QListView::Static);

    setGridSize(gridSize());
    setViewportMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
    setItemDelegate(delegate);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);

    connect(vScrollBar->ani(), &QPropertyAnimation::finished, this, &ScrollViewBase::_onFirstScrollFinished);
    vScrollBar->setScrollAnimation(1);
    setDate(date);

    vScrollBar->setForceHidden(true);
    setVerticalScrollMode(ScrollPerPixel);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setUniformItemSizes(true);
}

void ScrollViewBase::_onFirstScrollFinished() {
    vScrollBar->setScrollAnimation(300, QEasingCurve(QEasingCurve::OutQuad));
    disconnect(vScrollBar->ani(), nullptr, nullptr, nullptr);
}

void ScrollViewBase::scrollUp() {
    scrollToPage(currentPage - 1);
}

void ScrollViewBase::scrollDown() {
    scrollToPage(currentPage + 1);
}

void ScrollViewBase::scrollToPage(int page) {
    int totalPages = qCeil(model()->rowCount() / (pageRows() * cols()));
    if (page < 0 || page > totalPages) return;

    currentPage = page;

    int y = gridSize().height() * pageRows() * page;
    vScrollBar->scrollTo(y);  // 改为 scrollTo
    auto range = currentPageRange();
    delegate->setRange(range.first, range.second);
    emit pageChanged(page);
}

std::pair<QDate, QDate> ScrollViewBase::currentPageRange() {
    return {QDate(), QDate()};
}

void ScrollViewBase::setDate(const QDate& date) {
    this->date = date;
    scrollToDate(date);
}

void ScrollViewBase::scrollToDate(const QDate& date) {
    // Virtual
}

void ScrollViewBase::_setPressedIndex(const QModelIndex& index) {
    delegate->setPressedIndex(index);
    viewport()->update();
}

void ScrollViewBase::_setSelectedIndex(const QModelIndex& index) {
    delegate->setSelectedIndex(index);
    viewport()->update();
}

void ScrollViewBase::wheelEvent(QWheelEvent* e) {
    if (vScrollBar->ani()->state() == QPropertyAnimation::Running) return;

    if (e->angleDelta().y() < 0) {
        scrollDown();
    } else {
        scrollUp();
    }
}

void ScrollViewBase::mousePressEvent(QMouseEvent* e) {
    QListWidget::mousePressEvent(e);
    if (e->button() == Qt::LeftButton && indexAt(e->pos()).row() >= 0) {
        _setPressedIndex(currentIndex());
    }
}

void ScrollViewBase::mouseReleaseEvent(QMouseEvent* e) {
    QListWidget::mouseReleaseEvent(e);
    _setPressedIndex(QModelIndex());
}

// CalendarViewBase
CalendarViewBase::CalendarViewBase(QWidget* parent)
    : QFrame(parent), titleButton(new QPushButton(this)), resetButton(new ScrollButton(FluentIcon(FIF::CANCEL), this)),
    upButton(new ScrollButton(FluentIcon(FIF::CARE_UP_SOLID), this)), downButton(new ScrollButton(FluentIcon(FIF::CARE_DOWN_SOLID), this)),
      hBoxLayout(new QHBoxLayout()), vBoxLayout(new QVBoxLayout(this)) {

    initWidget();
}

void CalendarViewBase::initWidget() {
    setFixedSize(314, 355);
    upButton->setFixedSize(32, 34);
    downButton->setFixedSize(32, 34);
    resetButton->setFixedSize(32, 34);
    titleButton->setFixedHeight(34);

    hBoxLayout->setContentsMargins(9, 8, 9, 8);
    hBoxLayout->setSpacing(7);
    hBoxLayout->addWidget(titleButton, 1, Qt::AlignVCenter);
    hBoxLayout->addWidget(resetButton, 0, Qt::AlignVCenter);
    hBoxLayout->addWidget(upButton, 0, Qt::AlignVCenter);
    hBoxLayout->addWidget(downButton, 0, Qt::AlignVCenter);
    setResetEnabled(false);

    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->setAlignment(Qt::AlignTop);

    titleButton->setObjectName("titleButton");
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::CALENDAR_PICKER);

    connect(titleButton, &QPushButton::clicked, this, &CalendarViewBase::titleClicked);
    connect(resetButton, &QPushButton::clicked, this, &CalendarViewBase::resetted);
    connect(upButton, &QPushButton::clicked, this, &CalendarViewBase::_onScrollUp);
    connect(downButton, &QPushButton::clicked, this, &CalendarViewBase::_onScrollDown);
}

void CalendarViewBase::setScrollView(ScrollViewBase* view) {
    scrollView = view;
    connect(scrollView, &ScrollViewBase::itemClicked, this, &CalendarViewBase::itemClicked);
    vBoxLayout->addWidget(view);
    connect(view, &ScrollViewBase::pageChanged, this, &CalendarViewBase::_updateTitle);
    _updateTitle();
}

void CalendarViewBase::setResetEnabled(bool isEnabled) {
    resetButton->setVisible(isEnabled);
}

bool CalendarViewBase::isResetEnabled() const {
    return resetButton->isVisible();
}

void CalendarViewBase::setDate(const QDate& date) {
    scrollView->setDate(date);
    _updateTitle();
}

void CalendarViewBase::setTitle(const QString& title) {
    titleButton->setText(title);
}

void CalendarViewBase::_onScrollUp() {
    scrollView->scrollUp();
    _updateTitle();
}

void CalendarViewBase::_onScrollDown() {
    scrollView->scrollDown();
    _updateTitle();
}

void CalendarViewBase::_updateTitle() {
    // Virtual
}

// YearScrollView
YearScrollView::YearScrollView(QWidget* parent)
    : ScrollViewBase(new YearScrollItemDelegate(QDate(), QDate()), parent) {

    initItems();
    initWidget();
}

void YearScrollView::initItems() {
    clear();

    for (int i = minYear; i <= maxYear; ++i) {
        addItem(QString::number(i));
        QListWidgetItem* item = this->item(i - minYear);

        item->setData(Qt::UserRole, QDate(i, 1, 1));
        item->setSizeHint(gridSize());
        if (i == currentDate.year()) {
            delegate->setCurrentIndex(model()->index(i - minYear, 0));
        }
    }
}

void YearScrollView::scrollToDate(const QDate& date) {
    int page = (date.year() - minYear) / 12;
    scrollToPage(page);
}

std::pair<QDate, QDate> YearScrollView::currentPageRange() {
    int pageSize = pageRows() * cols();
    int left = currentPage * pageSize + minYear;

    QMap<int, int> decadeCounts;

    for (int i = left; i < left + 16; ++i) {
        int decadeStart = (i / 10) * 10;
        decadeCounts[decadeStart]++;
    }

    int mostCommonDecade = 0;
    int maxCount = -1;
    for (auto it = decadeCounts.constBegin(); it != decadeCounts.constEnd(); ++it) {
        if (it.value() > maxCount) {
            maxCount = it.value();
            mostCommonDecade = it.key();
        }
    }

    return std::make_pair(
                QDate(mostCommonDecade, 1, 1),
                QDate(mostCommonDecade + 10, 1, 1)
                );
}

void YearScrollView::mouseReleaseEvent(QMouseEvent* e)
{
    ScrollViewBase::mouseReleaseEvent(e);
    if (QListWidgetItem* item = currentItem()) {
        QDate date = item->data(Qt::UserRole).toDate();
        if (date.isValid()) {
            emit itemClicked(date);
        }
    }
}

// YearCalendarView
YearCalendarView::YearCalendarView(QWidget* parent)
    : CalendarViewBase(parent) {
    setScrollView(new YearScrollView(this));
    titleButton->setEnabled(false);
}

void YearCalendarView::_updateTitle() {
    auto range = static_cast<YearScrollView*>(scrollView)->currentPageRange();
    setTitle(QString("%1 - %2").arg(range.first.year()).arg(range.second.year()));
}



MonthScrollView::MonthScrollView(QWidget* parent)
    : ScrollViewBase(new YearScrollItemDelegate(QDate(), QDate()), parent) {
    months << tr("一月") << tr("二月") << tr("三月") << tr("四月")
           << tr("五月") << tr("六月") << tr("七月") << tr("八月")
           << tr("九月") << tr("十月") << tr("十一月") << tr("十二月");

    initItems();
    initWidget();
}

void MonthScrollView::initItems() {
    for (int i = 0; i < 201; ++i) {
        addItems(months);
    }

    for (int i = 0; i < 12 * 201; ++i) {
        int year = i / 12 + minYear;
        int m = i % 12 + 1;
        QListWidgetItem* item = this->item(i);
        item->setData(Qt::UserRole, QDate(year, m, 1));
        item->setSizeHint(gridSize());

        if (year == currentDate.year() && m == currentDate.month()) {
            QModelIndex index = indexFromItem(item);
            delegate->setCurrentIndex(index);
        }
    }
}

void MonthScrollView::scrollToDate(const QDate& date) {
    int page = date.year() - minYear;
    scrollToPage(page);
}

std::pair<QDate, QDate> MonthScrollView::currentPageRange() {
    int year = minYear + currentPage;
    return std::make_pair(QDate(year, 1, 1), QDate(year, 12, 31));
}

void MonthScrollView::mouseReleaseEvent(QMouseEvent* e)
{
    ScrollViewBase::mouseReleaseEvent(e);
    if (QListWidgetItem* item = currentItem()) {
        QDate date = item->data(Qt::UserRole).toDate();
        if (date.isValid()) {
            emit itemClicked(date);
        }
    }
}


MonthCalendarView::MonthCalendarView(QWidget* parent)
    : CalendarViewBase(parent) {
    setScrollView(new MonthScrollView(this));
}

void MonthCalendarView::_updateTitle() {
    auto range = static_cast<MonthScrollView*>(scrollView)->currentPageRange();
    QDate date = range.first;
    setTitle(QString::number(date.year()));
}

QDate MonthCalendarView::currentPageDate() const {
    auto range = static_cast<MonthScrollView*>(scrollView)->currentPageRange();
    QDate date = range.first;
    QListWidgetItem* item = scrollView->currentItem();
    int month = item ? item->data(Qt::UserRole).toDate().month() : 1;

    return QDate(date.year(), month, 1);
}

// DayScrollView
DayScrollView::DayScrollView(QWidget* parent)
    : ScrollViewBase(new DayScrollItemDelegate(QDate(), QDate()), parent) {


}

void DayScrollView::initialize()
{
    initItems();
    initWidget();

    setViewportMargins(0, 38, 0, 0);
    vBoxLayout = new QHBoxLayout(this);
    weekDayGroup = new QWidget(this);
    weekDayLayout = new QHBoxLayout(weekDayGroup);
    weekDayGroup->setObjectName("weekDayGroup");

    const QStringList weekDays = {"一", "二", "三", "四", "五", "六", "日"};
    for (const QString& day : weekDays) {
        QLabel* label = new QLabel(day);
        label->setObjectName("weekDayLabel");
        weekDayLayout->addWidget(label, 1, Qt::AlignHCenter);
    }

    vBoxLayout->setAlignment(Qt::AlignTop);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    weekDayLayout->setSpacing(0);
    weekDayLayout->setContentsMargins(3, 12, 3, 12);
    vBoxLayout->addWidget(weekDayGroup);
}

void DayScrollView::initItems()
{
    QDate startDate(minYear, 1, 1);
    QDate endDate(maxYear, 12, 31);
    QDate currentDate = startDate;

    // Step 1: 添加前置占位符（使第一天对齐星期几）
    int bias = currentDate.dayOfWeek() - 1;
    for (int i = 0; i < bias; ++i) {
        QListWidgetItem* item = new QListWidgetItem(this);
        item->setFlags(Qt::NoItemFlags);
        this->addItem(item);
    }

    // Step 2: 添加日期数字项和对应的 QDate 数据
    QStringList items;
    QVector<QDate> dates;

    while (currentDate <= endDate) {
        items << QString::number(currentDate.day());
        dates << QDate(currentDate);
        currentDate = currentDate.addDays(1);
    }

    this->addItems(items);

    // Step 3: 为每个日期项设置 UserRole 数据和尺寸
    for (int i = bias; i < this->count(); ++i) {
        QListWidgetItem* item = this->item(i);
        if (item) {
            item->setData(Qt::UserRole, dates[i - bias]);
            item->setSizeHint(QSize(44, 44));
        }
    }

    int currentRow = _dateToRow(date);
    QModelIndex currentIndex = this->model()->index(currentRow, 0);
    delegate->setCurrentIndex(currentIndex);
}

void DayScrollView::setDate(const QDate& date) {
    scrollToDate(date);
    setCurrentIndex(model()->index(_dateToRow(date), 0));
    delegate->setSelectedIndex(currentIndex());
}

void DayScrollView::scrollToDate(const QDate& date) {
    int page = (date.year() - minYear) * 12 + date.month() - 1;
    scrollToPage(page);
}

void DayScrollView::scrollToPage(int page) {
    if (page < 0 || page > (201 * 12 - 1))
        return;

    currentPage = page;

    int index = _dateToRow(_pageToDate());
    int y = (index / cols()) * gridSize().height();
    vScrollBar->scrollTo(y);

    auto range = currentPageRange();
    delegate->setRange(range.first, range.second);
}

std::pair<QDate, QDate> DayScrollView::currentPageRange() {
    QDate date = _pageToDate();
    return {date, date.addMonths(1).addDays(-1)};
}

QDate DayScrollView::_pageToDate() {
    int year = currentPage / 12 + minYear;
    int month = currentPage % 12 + 1;

    return QDate(year, month, 1);
}

int DayScrollView::_dateToRow(const QDate& date) {
    QDate startDate(minYear, 1, 1);
    int days = startDate.daysTo(date);
    return days + startDate.dayOfWeek() - 1;
}

void DayScrollView::mouseReleaseEvent(QMouseEvent* e) {
    ScrollViewBase::mouseReleaseEvent(e);
    _setSelectedIndex(currentIndex());

    if (QListWidgetItem* item = currentItem()) {
        QDate date = item->data(Qt::UserRole).toDate();
        if (date.isValid()) {
            emit itemClicked(date);
        }
    }
}

// DayCalendarView
DayCalendarView::DayCalendarView(QWidget* parent)
    : CalendarViewBase(parent) {
}

void DayCalendarView::initialize()
{
    auto dayScroll = new DayScrollView(this);
    dayScroll->initialize();
    setScrollView(dayScroll);
}

void DayCalendarView::_updateTitle() {
    QDate date = currentPageDate();
    QLocale locale;
    QString name = locale.monthName(date.month());
    setTitle(QString("%1 %2").arg(name).arg(date.year()));
}

QDate DayCalendarView::currentPageDate() const {
    auto range = static_cast<DayScrollView*>(scrollView)->currentPageRange();
    return range.first;
}

void DayCalendarView::scrollToDate(const QDate& date) {
    static_cast<DayScrollView*>(scrollView)->scrollToDate(date);
    _updateTitle();
}

// CalendarView
CalendarView::CalendarView(QWidget* parent)
    : QWidget(parent), hBoxLayout(new QHBoxLayout(this)), stackedWidget(new QStackedWidget(this)),
      yearView(new YearCalendarView(this)), monthView(new MonthCalendarView(this)), dayView(new DayCalendarView(this)),
      opacityAni(new QPropertyAnimation(this, "windowOpacity", this)),
      slideAni(new QPropertyAnimation(this, "geometry", this)),
      aniGroup(new QParallelAnimationGroup(this)) {

    dayView->initialize();
    setLayout(hBoxLayout);
    initWidget();
}

void CalendarView::initWidget() {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose, true);

    stackedWidget->addWidget(dayView);
    stackedWidget->addWidget(monthView);
    stackedWidget->addWidget(yearView);

    hBoxLayout->setContentsMargins(12, 8, 12, 20);
    hBoxLayout->addWidget(stackedWidget);
    setShadowEffect();

    dayView->setDate(QDate::currentDate());

    aniGroup->addAnimation(opacityAni);
    aniGroup->addAnimation(slideAni);

    connect(dayView, &DayCalendarView::titleClicked, this, &CalendarView::_onDayViewTitleClicked);
    connect(monthView, &MonthCalendarView::titleClicked, this, &CalendarView::_onMonthTitleClicked);

    connect(monthView, &MonthCalendarView::itemClicked, this, &CalendarView::_onMonthItemClicked);
    connect(yearView, &YearCalendarView::itemClicked, this, &CalendarView::_onYearItemClicked);
    connect(dayView, &DayCalendarView::itemClicked, this, &CalendarView::_onDayItemClicked);

    connect(monthView, &MonthCalendarView::resetted, this, &CalendarView::_onResetted);
    connect(yearView, &YearCalendarView::resetted, this, &CalendarView::_onResetted);
    connect(dayView, &DayCalendarView::resetted, this, &CalendarView::_onResetted);

}

void CalendarView::setShadowEffect(int blurRadius, QPoint offset, QColor color) {
    shadowEffect = new QGraphicsDropShadowEffect(stackedWidget);
    shadowEffect->setBlurRadius(blurRadius);
    shadowEffect->setOffset(offset);
    shadowEffect->setColor(color);
    stackedWidget->setGraphicsEffect(nullptr);
    stackedWidget->setGraphicsEffect(shadowEffect);
}

bool CalendarView::isResetEnabled() const {
    return _isResetEnabled;
}

void CalendarView::setResetEnabled(bool isEnabled) {
    _isResetEnabled = isEnabled;
    yearView->setResetEnabled(isEnabled);
    monthView->setResetEnabled(isEnabled);
    dayView->setResetEnabled(isEnabled);
}

void CalendarView::_onResetted() {
    emit resetted();
    close();
}

void CalendarView::_onDayViewTitleClicked() {
    stackedWidget->setCurrentWidget(monthView);
    monthView->setDate(dayView->currentPageDate());
}

void CalendarView::_onMonthTitleClicked() {
    stackedWidget->setCurrentWidget(yearView);
    yearView->setDate(monthView->currentPageDate());
}

void CalendarView::_onMonthItemClicked(const QDate& date) {
    stackedWidget->setCurrentWidget(dayView);
    dayView->scrollToDate(date);
}

void CalendarView::_onYearItemClicked(const QDate& date) {
    stackedWidget->setCurrentWidget(monthView);
    monthView->setDate(date);
}

void CalendarView::_onDayItemClicked(const QDate& date) {
    close();
    if (date != this->date) {
        this->date = date;
        emit dateChanged(date);
    }
}

void CalendarView::setDate(const QDate& date) {
    dayView->setDate(date);
    this->date = date;
}

void CalendarView::exec(const QPoint& pos, bool ani) {
    if (isVisible()) {
        return;
    }

    QRect screenRect = Screen::getCurrentScreenGeometry();
    QPoint adjustedPos = pos;
    adjustedPos.setX(qMax(screenRect.left(), qMin(pos.x(), screenRect.right() - width())));
    adjustedPos.setY(qMax(screenRect.top(), qMin(pos.y() - 4, screenRect.bottom() - height() + 5)));
    move(adjustedPos);

    if (ani) {
        opacityAni->setStartValue(0);
        opacityAni->setEndValue(1);
        opacityAni->setDuration(150);
        opacityAni->setEasingCurve(QEasingCurve::OutQuad);

        slideAni->setStartValue(QRect(adjustedPos - QPoint(0, 8), size()));
        slideAni->setEndValue(QRect(adjustedPos, size()));
        slideAni->setDuration(150);
        slideAni->setEasingCurve(QEasingCurve::OutQuad);
        aniGroup->start();
    }
    show();
}
