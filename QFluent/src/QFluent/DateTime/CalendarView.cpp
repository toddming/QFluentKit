#include "CalendarView.h"
#include <QLocale>
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

#include "Screen.h"
#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"


ViewScrollBar::ViewScrollBar(Qt::Orientation orientation, QWidget* parent)
    : QScrollBar(orientation, parent) {
    m_animation = new QPropertyAnimation(this, "value", this);
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
}

void ViewScrollBar::setScrollAnimation(int duration, QEasingCurve curve) {
    m_animation->setDuration(duration);
    m_animation->setEasingCurve(curve);
}

void ViewScrollBar::scrollTo(int value) {
    m_animation->stop();
    m_animation->setStartValue(this->value());
    m_animation->setEndValue(value);
    m_animation->start();
}

void ViewScrollBar::setForceHidden(bool hidden) {
    this->setVisible(!hidden);
}

QPropertyAnimation* ViewScrollBar::ani() const
{
    return m_animation;
}


CalendarButton::CalendarButton(const FluentIconBase &icon, QWidget* parent)
    : TransparentToolButton(icon, parent) {
}

void CalendarButton::paintEvent(QPaintEvent* event)
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

    QHash<QString, QString> attrs;
    attrs["fill"] = Theme::instance()->isDarkTheme() ? "#5e5e5e" : "#9c9c9c";
    FluentIconUtils::drawIcon(*fluentIcon(), &painter, QRectF(x, y, w, h), Fluent::ThemeMode::AUTO, false, attrs);
}


ScrollItemDelegate::ScrollItemDelegate(QDate minDate, QDate maxDate, QObject* parent)
    : QStyledItemDelegate(parent), m_minDate(minDate), m_maxDate(maxDate) {}

void ScrollItemDelegate::setRange(QDate minDate, QDate maxDate) {
    this->m_minDate = minDate;
    this->m_maxDate = maxDate;
}

void ScrollItemDelegate::setPressedIndex(const QModelIndex& index) {
    m_pressedIndex = index;
}

void ScrollItemDelegate::setCurrentIndex(const QModelIndex& index) {
    m_currentIndex = index;
}

void ScrollItemDelegate::setSelectedIndex(const QModelIndex& index) {
    m_selectedIndex = index;
}

void ScrollItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    drawBackground(painter, option, index);
    drawText(painter, option, index);
}

void ScrollItemDelegate::drawBackground(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    if (index != m_selectedIndex) {
        painter->setPen(Qt::NoPen);
    } else {
        painter->setPen(Theme::instance()->themeColor());
    }

    if (index == m_currentIndex) {
        if (index == m_pressedIndex) {
            painter->setBrush(Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_2));
        } else if (option.state & QStyle::State_MouseOver) {
            painter->setBrush(Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_1));
        } else {
            painter->setBrush(Theme::instance()->themeColor());
        }
    } else {
        int c = Theme::instance()->isDarkTheme() ? 255 : 0;
        if (index == m_pressedIndex) {
            painter->setBrush(QColor(c, c, c, 7));
        } else if (option.state & QStyle::State_MouseOver) {
            painter->setBrush(QColor(c, c, c, 9));
        } else {
            painter->setBrush(Qt::transparent);
        }
    }

    int m = itemMargin();
    painter->drawEllipse(option.rect.adjusted(m, m, -m, -m));
    painter->restore();
}

void ScrollItemDelegate::drawText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    painter->setFont(m_font);

    if (index == m_currentIndex) {
        painter->setPen(Theme::instance()->isDarkTheme() ? Qt::black : Qt::white);
    } else {
        painter->setPen(Theme::instance()->isDarkTheme() ? Qt::white : Qt::black);
        if (!((m_minDate <= index.data(Qt::UserRole).toDate() && index.data(Qt::UserRole).toDate() <= m_maxDate) ||
              (option.state & QStyle::State_MouseOver)) || index == m_pressedIndex) {
            painter->setOpacity(0.6);
        }
    }

    QString text = index.data(Qt::DisplayRole).toString();
    painter->drawText(option.rect, Qt::AlignCenter, text);
    painter->restore();
}

// ScrollViewBase
ScrollViewBase::ScrollViewBase(ScrollItemDelegate* delegateType, QWidget* parent)
    : QListWidget(parent),
      m_delegate(delegateType),
      m_currentDate(QDate::currentDate()),
      m_date(QDate::currentDate()),
      m_minYear(m_currentDate.year() - 10),
      m_maxYear(m_currentDate.year() + 10),
      m_currentPage(0),
      m_vScrollBar(nullptr) {

    setMouseTracking(true);
    m_vScrollBar = new ViewScrollBar(Qt::Vertical, this);
    setVerticalScrollBar(m_vScrollBar);
}

ScrollViewBase::~ScrollViewBase() {
    // 安全网：如果delegate还没有被Qt接管（比如initWidget()之前抛出异常），则手动删除
    // itemDelegate()返回当前设置的delegate，如果没调用过setItemDelegate()则返回nullptr
    if (m_delegate && itemDelegate() != m_delegate) {
        delete m_delegate;
        m_delegate = nullptr;
    }
}


void ScrollViewBase::initWidget() {
    setSpacing(0);
    setMovement(QListView::Static);

    setGridSize(gridSize());
    setViewportMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);
    setItemDelegate(m_delegate);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);

    connect(m_vScrollBar->ani(), &QPropertyAnimation::finished, this, &ScrollViewBase::onFirstScrollFinished);
    m_vScrollBar->setScrollAnimation(1);
    setDate(m_date);

    m_vScrollBar->setForceHidden(true);
    setVerticalScrollMode(ScrollPerPixel);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setUniformItemSizes(true);
}

void ScrollViewBase::onFirstScrollFinished() {
    m_vScrollBar->setScrollAnimation(300, QEasingCurve(QEasingCurve::OutQuad));
    disconnect(m_vScrollBar->ani(), &QPropertyAnimation::finished, this, &ScrollViewBase::onFirstScrollFinished);
}

void ScrollViewBase::scrollUp() {
    scrollToPage(m_currentPage - 1);
}

void ScrollViewBase::scrollDown() {
    scrollToPage(m_currentPage + 1);
}

void ScrollViewBase::scrollToPage(int page) {
    int totalPages = qCeil(model()->rowCount() / (pageRows() * cols()));
    if (page < 0 || page > totalPages) return;

    m_currentPage = page;

    int y = gridSize().height() * pageRows() * page;
    m_vScrollBar->scrollTo(y);
    auto range = currentPageRange();
    m_delegate->setRange(range.first, range.second);
    emit pageChanged(page);
}

std::pair<QDate, QDate> ScrollViewBase::currentPageRange() {
    return {QDate(), QDate()};
}

void ScrollViewBase::setDate(const QDate& date) {
    this->m_date = date;
    scrollToDate(date);
}

void ScrollViewBase::scrollToDate(const QDate& date) {
    // Virtual
}

void ScrollViewBase::setPressedIndex(const QModelIndex& index) {
    m_delegate->setPressedIndex(index);
    viewport()->update();
}

void ScrollViewBase::setSelectedIndex(const QModelIndex& index) {
    m_delegate->setSelectedIndex(index);
    viewport()->update();
}

void ScrollViewBase::wheelEvent(QWheelEvent* e) {
    if (m_vScrollBar->ani()->state() == QPropertyAnimation::Running) return;

    if (e->angleDelta().y() < 0) {
        scrollDown();
    } else {
        scrollUp();
    }
}

void ScrollViewBase::mousePressEvent(QMouseEvent* e) {
    QListWidget::mousePressEvent(e);
    if (e->button() == Qt::LeftButton && indexAt(e->pos()).row() >= 0) {
        setPressedIndex(currentIndex());
    }
}

void ScrollViewBase::mouseReleaseEvent(QMouseEvent* e) {
    QListWidget::mouseReleaseEvent(e);
    setPressedIndex(QModelIndex());
}

// CalendarViewBase
CalendarViewBase::CalendarViewBase(QWidget* parent)
    : QFrame(parent),
      m_titleButton(new QPushButton(this)),
      m_resetButton(new CalendarButton(FluentIcon(FIF::CANCEL), this)),
      m_upButton(new CalendarButton(FluentIcon(FIF::CARE_UP_SOLID), this)),
      m_downButton(new CalendarButton(FluentIcon(FIF::CARE_DOWN_SOLID), this)),
      m_scrollView(nullptr),
      m_hBoxLayout(new QHBoxLayout()),
      m_vBoxLayout(new QVBoxLayout(this)) {

    initWidget();
}

void CalendarViewBase::initWidget() {
    setFixedSize(314, 355);
    m_upButton->setFixedSize(32, 34);
    m_downButton->setFixedSize(32, 34);
    m_resetButton->setFixedSize(32, 34);
    m_titleButton->setFixedHeight(34);

    m_hBoxLayout->setContentsMargins(9, 8, 9, 8);
    m_hBoxLayout->setSpacing(7);
    m_hBoxLayout->addWidget(m_titleButton, 1, Qt::AlignVCenter);
    m_hBoxLayout->addWidget(m_resetButton, 0, Qt::AlignVCenter);
    m_hBoxLayout->addWidget(m_upButton, 0, Qt::AlignVCenter);
    m_hBoxLayout->addWidget(m_downButton, 0, Qt::AlignVCenter);
    setResetEnabled(false);

    m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_vBoxLayout->setSpacing(0);
    m_vBoxLayout->addLayout(m_hBoxLayout);
    m_vBoxLayout->setAlignment(Qt::AlignTop);

    m_titleButton->setObjectName("titleButton");
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::CALENDAR_PICKER);

    connect(m_titleButton, &QPushButton::clicked, this, &CalendarViewBase::titleClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &CalendarViewBase::resetted);
    connect(m_upButton, &QPushButton::clicked, this, &CalendarViewBase::onScrollUp);
    connect(m_downButton, &QPushButton::clicked, this, &CalendarViewBase::onScrollDown);
}

void CalendarViewBase::setScrollView(ScrollViewBase* view) {
    m_scrollView = view;
    connect(m_scrollView, &ScrollViewBase::itemClicked, this, &CalendarViewBase::itemClicked);
    m_vBoxLayout->addWidget(view);
    connect(view, &ScrollViewBase::pageChanged, this, &CalendarViewBase::updateTitle);
    updateTitle();
}

void CalendarViewBase::setResetEnabled(bool isEnabled) {
    m_resetButton->setVisible(isEnabled);
}

bool CalendarViewBase::isResetEnabled() const {
    return m_resetButton->isVisible();
}

void CalendarViewBase::setDate(const QDate& date) {
    m_scrollView->setDate(date);
    updateTitle();
}

void CalendarViewBase::setTitle(const QString& title) {
    m_titleButton->setText(title);
}

void CalendarViewBase::onScrollUp() {
    m_scrollView->scrollUp();
    updateTitle();
}

void CalendarViewBase::onScrollDown() {
    m_scrollView->scrollDown();
    updateTitle();
}

void CalendarViewBase::updateTitle() {
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

    for (int i = m_minYear; i <= m_maxYear; ++i) {
        addItem(QString::number(i));
        QListWidgetItem* item = this->item(i - m_minYear);

        item->setData(Qt::UserRole, QDate(i, 1, 1));
        item->setSizeHint(gridSize());
        if (i == m_currentDate.year()) {
            m_delegate->setCurrentIndex(model()->index(i - m_minYear, 0));
        }
    }
}

void YearScrollView::scrollToDate(const QDate& date) {
    int page = (date.year() - m_minYear) / 12;
    scrollToPage(page);
}

std::pair<QDate, QDate> YearScrollView::currentPageRange() {
    int pageSize = pageRows() * cols();
    int left = m_currentPage * pageSize + m_minYear;

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
    m_titleButton->setEnabled(false);
}

void YearCalendarView::updateTitle() {
    auto range = static_cast<YearScrollView*>(m_scrollView)->currentPageRange();
    setTitle(QString("%1 - %2").arg(range.first.year()).arg(range.second.year()));
}



MonthScrollView::MonthScrollView(QWidget* parent)
    : ScrollViewBase(new YearScrollItemDelegate(QDate(), QDate()), parent) {
    m_months << tr("一月") << tr("二月") << tr("三月") << tr("四月")
             << tr("五月") << tr("六月") << tr("七月") << tr("八月")
             << tr("九月") << tr("十月") << tr("十一月") << tr("十二月");

    initItems();
    initWidget();
}

void MonthScrollView::initItems() {
    for (int i = 0; i < 201; ++i) {
        addItems(m_months);
    }

    for (int i = 0; i < 12 * 201; ++i) {
        int year = i / 12 + m_minYear;
        int m = i % 12 + 1;
        QListWidgetItem* item = this->item(i);
        item->setData(Qt::UserRole, QDate(year, m, 1));
        item->setSizeHint(gridSize());

        if (year == m_currentDate.year() && m == m_currentDate.month()) {
            QModelIndex index = indexFromItem(item);
            m_delegate->setCurrentIndex(index);
        }
    }
}

void MonthScrollView::scrollToDate(const QDate& date) {
    int page = date.year() - m_minYear;
    scrollToPage(page);
}

std::pair<QDate, QDate> MonthScrollView::currentPageRange() {
    int year = m_minYear + m_currentPage;
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

void MonthCalendarView::updateTitle() {
    auto range = static_cast<MonthScrollView*>(m_scrollView)->currentPageRange();
    QDate date = range.first;
    setTitle(QString::number(date.year()));
}

QDate MonthCalendarView::currentPageDate() const {
    auto range = static_cast<MonthScrollView*>(m_scrollView)->currentPageRange();
    QDate date = range.first;
    QListWidgetItem* item = m_scrollView->currentItem();
    int month = item ? item->data(Qt::UserRole).toDate().month() : 1;

    return QDate(date.year(), month, 1);
}

// DayScrollView
DayScrollView::DayScrollView(QWidget* parent)
    : ScrollViewBase(new DayScrollItemDelegate(QDate(), QDate()), parent),
      m_hBoxLayout(nullptr),
      m_weekDayGroup(nullptr),
      m_weekDayLayout(nullptr) {
}

void DayScrollView::initialize()
{
    initItems();
    initWidget();

    setViewportMargins(0, 38, 0, 0);
    m_hBoxLayout = new QHBoxLayout(this);
    m_weekDayGroup = new QWidget(this);
    m_weekDayLayout = new QHBoxLayout(m_weekDayGroup);
    m_weekDayGroup->setObjectName("weekDayGroup");

    const QStringList weekDays = {"一", "二", "三", "四", "五", "六", "日"};
    for (const QString& day : weekDays) {
        QLabel* label = new QLabel(day, m_weekDayGroup);
        label->setObjectName("weekDayLabel");
        m_weekDayLayout->addWidget(label, 1, Qt::AlignHCenter);
    }

    m_hBoxLayout->setAlignment(Qt::AlignTop);
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_hBoxLayout->setSpacing(0);
    m_weekDayLayout->setSpacing(0);
    m_weekDayLayout->setContentsMargins(3, 12, 3, 12);
    m_hBoxLayout->addWidget(m_weekDayGroup);
}

void DayScrollView::initItems()
{
    QDate startDate(m_minYear, 1, 1);
    QDate endDate(m_maxYear, 12, 31);
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

    int currentRow = dateToRow(m_date);
    QModelIndex currentIndex = this->model()->index(currentRow, 0);
    m_delegate->setCurrentIndex(currentIndex);
}

void DayScrollView::setDate(const QDate& date) {
    scrollToDate(date);
    setCurrentIndex(model()->index(dateToRow(date), 0));
    m_delegate->setSelectedIndex(currentIndex());
}

void DayScrollView::scrollToDate(const QDate& date) {
    int page = (date.year() - m_minYear) * 12 + date.month() - 1;
    scrollToPage(page);
}

void DayScrollView::scrollToPage(int page) {
    if (page < 0 || page > (201 * 12 - 1))
        return;

    m_currentPage = page;

    int index = dateToRow(pageToDate());
    int y = (index / cols()) * gridSize().height();
    m_vScrollBar->scrollTo(y);

    auto range = currentPageRange();
    m_delegate->setRange(range.first, range.second);
    emit pageChanged(page);
}

std::pair<QDate, QDate> DayScrollView::currentPageRange() {
    QDate date = pageToDate();
    return {date, date.addMonths(1).addDays(-1)};
}

QDate DayScrollView::pageToDate() {
    int year = m_currentPage / 12 + m_minYear;
    int month = m_currentPage % 12 + 1;

    return QDate(year, month, 1);
}

int DayScrollView::dateToRow(const QDate& date) {
    QDate startDate(m_minYear, 1, 1);
    int days = startDate.daysTo(date);
    return days + startDate.dayOfWeek() - 1;
}

void DayScrollView::mouseReleaseEvent(QMouseEvent* e) {
    ScrollViewBase::mouseReleaseEvent(e);
    setSelectedIndex(currentIndex());

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

void DayCalendarView::updateTitle() {
    QDate date = currentPageDate();
    QLocale locale;
    QString name = locale.monthName(date.month());
    setTitle(QString("%1 %2").arg(name).arg(date.year()));
}

QDate DayCalendarView::currentPageDate() const {
    auto range = static_cast<DayScrollView*>(m_scrollView)->currentPageRange();
    return range.first;
}

void DayCalendarView::scrollToDate(const QDate& date) {
    static_cast<DayScrollView*>(m_scrollView)->scrollToDate(date);
    updateTitle();
}

// CalendarView
CalendarView::CalendarView(QWidget* parent)
    : QWidget(parent),
      m_hBoxLayout(new QHBoxLayout(this)),
      m_stackedWidget(new QStackedWidget(this)),
      m_yearView(new YearCalendarView(this)),
      m_monthView(new MonthCalendarView(this)),
      m_dayView(new DayCalendarView(this)),
      m_opacityAnimation(new QPropertyAnimation(this, "windowOpacity", this)),
      m_slideAnimation(new QPropertyAnimation(this, "geometry", this)),
      m_animationGroup(new QParallelAnimationGroup(this)),
      m_shadowEffect(nullptr),
      m_isResetEnabled(false) {

    m_dayView->initialize();
    setLayout(m_hBoxLayout);
    initWidget();
}

CalendarView::~CalendarView() {
    // 清理shadow effect（如果存在）
    if (m_shadowEffect && m_shadowEffect->parent() != m_stackedWidget) {
        delete m_shadowEffect;
        m_shadowEffect = nullptr;
    }
}

void CalendarView::initWidget() {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose, true);

    m_stackedWidget->addWidget(m_dayView);
    m_stackedWidget->addWidget(m_monthView);
    m_stackedWidget->addWidget(m_yearView);

    m_hBoxLayout->setContentsMargins(12, 8, 12, 20);
    m_hBoxLayout->addWidget(m_stackedWidget);
    setShadowEffect();

    m_dayView->setDate(QDate::currentDate());

    m_animationGroup->addAnimation(m_opacityAnimation);
    m_animationGroup->addAnimation(m_slideAnimation);

    connect(m_dayView, &DayCalendarView::titleClicked, this, &CalendarView::onDayViewTitleClicked);
    connect(m_monthView, &MonthCalendarView::titleClicked, this, &CalendarView::onMonthTitleClicked);

    connect(m_monthView, &MonthCalendarView::itemClicked, this, &CalendarView::onMonthItemClicked);
    connect(m_yearView, &YearCalendarView::itemClicked, this, &CalendarView::onYearItemClicked);
    connect(m_dayView, &DayCalendarView::itemClicked, this, &CalendarView::onDayItemClicked);

    connect(m_monthView, &MonthCalendarView::resetted, this, &CalendarView::onResetted);
    connect(m_yearView, &YearCalendarView::resetted, this, &CalendarView::onResetted);
    connect(m_dayView, &DayCalendarView::resetted, this, &CalendarView::onResetted);

}

void CalendarView::setShadowEffect(int blurRadius, QPoint offset, QColor color) {
    // 先删除旧的shadow effect（如果存在且不是通过父对象管理的）
    if (m_shadowEffect && m_shadowEffect->parent() != m_stackedWidget) {
        delete m_shadowEffect;
        m_shadowEffect = nullptr;
    }

    // 创建新的shadow effect，设置m_stackedWidget为父对象以便自动管理内存
    m_shadowEffect = new QGraphicsDropShadowEffect(m_stackedWidget);
    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(offset);
    m_shadowEffect->setColor(color);
    m_stackedWidget->setGraphicsEffect(m_shadowEffect);
}

bool CalendarView::isResetEnabled() const {
    return m_isResetEnabled;
}

void CalendarView::setResetEnabled(bool isEnabled) {
    m_isResetEnabled = isEnabled;
    m_yearView->setResetEnabled(isEnabled);
    m_monthView->setResetEnabled(isEnabled);
    m_dayView->setResetEnabled(isEnabled);
}

void CalendarView::onResetted() {
    emit resetted();
    close();
}

void CalendarView::onDayViewTitleClicked() {
    m_stackedWidget->setCurrentWidget(m_monthView);
    m_monthView->setDate(m_dayView->currentPageDate());
}

void CalendarView::onMonthTitleClicked() {
    m_stackedWidget->setCurrentWidget(m_yearView);
    m_yearView->setDate(m_monthView->currentPageDate());
}

void CalendarView::onMonthItemClicked(const QDate& date) {
    m_stackedWidget->setCurrentWidget(m_dayView);
    m_dayView->scrollToDate(date);
}

void CalendarView::onYearItemClicked(const QDate& date) {
    m_stackedWidget->setCurrentWidget(m_monthView);
    m_monthView->setDate(date);
}

void CalendarView::onDayItemClicked(const QDate& date) {
    close();
    if (date != this->m_date) {
        this->m_date = date;
        emit dateChanged(date);
    }
}

void CalendarView::setDate(const QDate& date) {
    m_dayView->setDate(date);
    this->m_date = date;
}

void CalendarView::exec(const QPoint& pos, bool ani) {
    if (isVisible()) {
        return;
    }

    QRect screenRect = Screen::currentScreenGeometry();
    QPoint adjustedPos = pos;
    adjustedPos.setX(qMax(screenRect.left(), qMin(pos.x(), screenRect.right() - width())));
    adjustedPos.setY(qMax(screenRect.top(), qMin(pos.y() - 4, screenRect.bottom() - height() + 5)));
    move(adjustedPos);

    if (ani) {
        m_opacityAnimation->setStartValue(0);
        m_opacityAnimation->setEndValue(1);
        m_opacityAnimation->setDuration(150);
        m_opacityAnimation->setEasingCurve(QEasingCurve::OutQuad);

        m_slideAnimation->setStartValue(QRect(adjustedPos - QPoint(0, 8), size()));
        m_slideAnimation->setEndValue(QRect(adjustedPos, size()));
        m_slideAnimation->setDuration(150);
        m_slideAnimation->setEasingCurve(QEasingCurve::OutQuad);
        m_animationGroup->start();
    }
    show();
}
