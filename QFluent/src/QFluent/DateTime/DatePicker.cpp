#include "DatePicker.h"
#include "CycleListWidget.h"

// MonthFormatter 实现
MonthFormatter::MonthFormatter(QObject* parent)
    : PickerColumnFormatter(parent)
{
    m_months << tr("一月") << tr("二月") << tr("三月")
             << tr("四月") << tr("五月") << tr("六月")
             << tr("七月") << tr("八月") << tr("九月")
             << tr("十月") << tr("十一月") << tr("十二月");
}

QString MonthFormatter::encode(const QVariant& value)
{
    int month = value.toInt();
    if (month >= 1 && month <= 12) {
        return m_months[month - 1];
    }
    return QString::number(month);
}

QVariant MonthFormatter::decode(const QString& value)
{
    int index = m_months.indexOf(value);
    if (index >= 0) {
        return index + 1;
    }
    return value.toInt();
}

// DatePickerBase 实现
DatePickerBase::DatePickerBase(QWidget* parent)
    : PickerBase(parent)
    , m_yearFormatter(nullptr)
    , m_monthFormatter(nullptr)
    , m_dayFormatter(nullptr)
{
}

DatePickerBase::~DatePickerBase() = default;

void DatePickerBase::setYearFormatter(PickerColumnFormatter* formatter)
{
    m_yearFormatter = formatter;
}

void DatePickerBase::setMonthFormatter(PickerColumnFormatter* formatter)
{
    m_monthFormatter = formatter;
}

void DatePickerBase::setDayFormatter(PickerColumnFormatter* formatter)
{
    m_dayFormatter = formatter;
}

PickerColumnFormatter* DatePickerBase::yearFormatter() const
{
    if (m_yearFormatter) {
        return m_yearFormatter;
    }
    
    // 懒加载：只在第一次需要时创建默认 formatter
    if (!m_defaultYearFormatter) {
        m_defaultYearFormatter.reset(new DigitFormatter());
    }
    return m_defaultYearFormatter.data();
}

PickerColumnFormatter* DatePickerBase::monthFormatter() const
{
    if (m_monthFormatter) {
        return m_monthFormatter;
    }
    
    // 懒加载：只在第一次需要时创建默认 formatter
    if (!m_defaultMonthFormatter) {
        m_defaultMonthFormatter.reset(new MonthFormatter());
    }
    return m_defaultMonthFormatter.data();
}

PickerColumnFormatter* DatePickerBase::dayFormatter() const
{
    if (m_dayFormatter) {
        return m_dayFormatter;
    }
    
    // 懒加载：只在第一次需要时创建默认 formatter
    if (!m_defaultDayFormatter) {
        m_defaultDayFormatter.reset(new DigitFormatter());
    }
    return m_defaultDayFormatter.data();
}

void DatePickerBase::reset()
{
    m_date = QDate();
    PickerBase::reset();
}

// DatePicker 实现
DatePicker::DatePicker(QWidget* parent, DateFormat format, bool tight)
    : DatePickerBase(parent)
    , m_isMonthTight(tight)
    , m_dateFormat(format)
    , m_monthIndex(0)
    , m_dayIndex(0)
    , m_yearIndex(0)
{
    m_monthLabel = tr("月");
    m_yearLabel = tr("年");
    m_dayLabel = tr("日");
    
    setDateFormat(format);
}

void DatePicker::setDateFormat(DateFormat format)
{
    clearColumns();
    int currentYear = QDate::currentDate().year();
    m_dateFormat = format;
    
    QList<QVariant> years, months, days;
    for (int year = currentYear - 100; year <= currentYear + 100; ++year) {
        years << year;
    }
    for (int month = 1; month <= 12; ++month) {
        months << month;
    }
    for (int day = 1; day <= 31; ++day) {
        days << day;
    }
    
    if (format == MM_DD_YYYY) {
        m_monthIndex = 0;
        m_dayIndex = 1;
        m_yearIndex = 2;
        
        addColumn(m_monthLabel, months, 80, Qt::AlignLeft, monthFormatter());
        addColumn(m_dayLabel, days, 80, Qt::AlignCenter, dayFormatter());
        addColumn(m_yearLabel, years, 80, Qt::AlignCenter, yearFormatter());
    } else {
        m_yearIndex = 0;
        m_monthIndex = 1;
        m_dayIndex = 2;
        
        addColumn(m_yearLabel, years, 80, Qt::AlignCenter, yearFormatter());
        addColumn(m_monthLabel, months, 80, Qt::AlignCenter, monthFormatter());
        addColumn(m_dayLabel, days, 80, Qt::AlignCenter, dayFormatter());
    }
    
    setColumnWidth(m_monthIndex, monthColumnWidth());
}

QStringList DatePicker::panelInitialValue()
{
    QStringList val = value();
    bool hasValue = false;
    for (const QString& v : val) {
        if (!v.isEmpty()) {
            hasValue = true;
            break;
        }
    }
    
    if (hasValue) {
        return val;
    }
    
    QDate currentDate = QDate::currentDate();
    QString yearValue = encodeValue(m_yearIndex, currentDate.year());
    QString monthValue = encodeValue(m_monthIndex, currentDate.month());
    QString dayValue = encodeValue(m_dayIndex, currentDate.day());
    
    if (m_dateFormat == YYYY_MM_DD) {
        return QStringList() << yearValue << monthValue << dayValue;
    } else {
        return QStringList() << monthValue << dayValue << yearValue;
    }
}

void DatePicker::setMonthTight(bool tight)
{
    if (m_isMonthTight == tight) {
        return;
    }
    
    m_isMonthTight = tight;
    setColumnWidth(m_monthIndex, monthColumnWidth());
}

int DatePicker::monthColumnWidth()
{
    if (m_monthIndex < 0 || m_monthIndex >= m_columns.size()) {
        return 80;
    }

    QFontMetrics fontMetrics = this->fontMetrics();
    int maxWidth = 0;

    QStringList items = m_columns[m_monthIndex]->items();
    for (const QString& item : items) {
        maxWidth = qMax(maxWidth, fontMetrics.horizontalAdvance(item));
    }
    maxWidth += 20;
    
    if (m_monthLabel == "month") {
        return maxWidth + 49;
    }
    
    return m_isMonthTight ? qMax(80, maxWidth) : maxWidth + 49;
}

void DatePicker::onColumnValueChanged(PickerPanel* panel, int index, const QString& value)
{
    if (index == m_dayIndex) {
        return;
    }
    
    int month = decodeValue(m_monthIndex, panel->columnValue(m_monthIndex)).toInt();
    int year = decodeValue(m_yearIndex, panel->columnValue(m_yearIndex)).toInt();
    int daysInMonth = m_calendar.daysInMonth(month, year);
    
    CycleListWidget* dayColumn = panel->column(m_dayIndex);
    QString currentDay = dayColumn->currentItem()->text();
    
    QList<QVariant> dayList;
    for (int day = 1; day <= daysInMonth; ++day) {
        dayList << day;
    }
    setColumnItems(m_dayIndex, dayList);
    
    dayColumn->setItems(m_columns[m_dayIndex]->items());
    dayColumn->setSelectedItem(currentDay);
}

void DatePicker::onConfirmed(const QStringList& value)
{
    int year = decodeValue(m_yearIndex, value[m_yearIndex]).toInt();
    int month = decodeValue(m_monthIndex, value[m_monthIndex]).toInt();
    int day = decodeValue(m_dayIndex, value[m_dayIndex]).toInt();
    
    QDate newDate(year, month, day);
    QDate oldDate = m_date;
    setDate(newDate);
    
    if (oldDate != newDate) {
        emit dateChanged(newDate);
    }
}

void DatePicker::setDate(const QDate& date)
{
    if (!date.isValid() || date.isNull()) {
        return;
    }
    
    m_date = date;
    setColumnValue(m_monthIndex, date.month());
    setColumnValue(m_dayIndex, date.day());
    setColumnValue(m_yearIndex, date.year());
    
    QList<QVariant> dayList;
    for (int day = 1; day <= date.daysInMonth(); ++day) {
        dayList << day;
    }
    setColumnItems(m_dayIndex, dayList);
}

// ZhFormatter 实现
ZhFormatter::ZhFormatter(const QString& suffix, QObject* parent)
    : PickerColumnFormatter(parent), m_suffix(suffix)
{
}

QString ZhFormatter::encode(const QVariant& value)
{
    return QString::number(value.toInt()) + m_suffix;
}

QVariant ZhFormatter::decode(const QString& value)
{
    QString numStr = value;
    numStr.remove(m_suffix);
    return numStr.toInt();
}

// ZhDatePicker 实现
ZhDatePicker::ZhDatePicker(QWidget* parent)
    : DatePicker(parent, YYYY_MM_DD)
{    
    setDayFormatter(new ZhDayFormatter(this));
    setYearFormatter(new ZhYearFormatter(this));
    setMonthFormatter(new ZhMonthFormatter(this));
    setDateFormat(YYYY_MM_DD);
}
