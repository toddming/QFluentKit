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
    : PickerBase(parent), m_yearFormatter(nullptr), 
      m_monthFormatter(nullptr), m_dayFormatter(nullptr)
{
}

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
    return new DigitFormatter();
}

PickerColumnFormatter* DatePickerBase::monthFormatter() const
{
    if (m_monthFormatter) {
        return m_monthFormatter;
    }
    return new MonthFormatter();
}

PickerColumnFormatter* DatePickerBase::dayFormatter() const
{
    if (m_dayFormatter) {
        return m_dayFormatter;
    }
    return new DigitFormatter();
}

void DatePickerBase::reset()
{
    m_date = QDate();
    PickerBase::reset();
}

// DatePicker 实现
DatePicker::DatePicker(QWidget* parent, DateFormat format, bool isMonthTight)
    : DatePickerBase(parent), m_isMonthTight(isMonthTight)
{
    m_MONTH = tr("月");
    m_YEAR = tr("年");
    m_DAY = tr("日");
    
    setDateFormat(format);
}

void DatePicker::setDateFormat(DateFormat format)
{
    clearColumns();
    int y = QDate::currentDate().year();
    m_dateFormat = format;
    
    QList<QVariant> years, months, days;
    for (int i = y - 100; i <= y + 100; ++i) {
        years << i;
    }
    for (int i = 1; i <= 12; ++i) {
        months << i;
    }
    for (int i = 1; i <= 31; ++i) {
        days << i;
    }
    
    if (format == MM_DD_YYYY) {
        m_monthIndex = 0;
        m_dayIndex = 1;
        m_yearIndex = 2;
        
        addColumn(m_MONTH, months, 80, Qt::AlignLeft, monthFormatter());
        addColumn(m_DAY, days, 80, Qt::AlignCenter, dayFormatter());
        addColumn(m_YEAR, years, 80, Qt::AlignCenter, yearFormatter());
    } else {
        m_yearIndex = 0;
        m_monthIndex = 1;
        m_dayIndex = 2;
        
        addColumn(m_YEAR, years, 80, Qt::AlignCenter, yearFormatter());
        addColumn(m_MONTH, months, 80, Qt::AlignCenter, monthFormatter());
        addColumn(m_DAY, days, 80, Qt::AlignCenter, dayFormatter());
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
    
    QDate date = QDate::currentDate();
    QString y = encodeValue(m_yearIndex, date.year());
    QString m = encodeValue(m_monthIndex, date.month());
    QString d = encodeValue(m_dayIndex, date.day());
    
    if (m_dateFormat == YYYY_MM_DD) {
        return QStringList() << y << m << d;
    } else {
        return QStringList() << m << d << y;
    }
}

void DatePicker::setMonthTight(bool isTight)
{
    if (m_isMonthTight == isTight) {
        return;
    }
    
    m_isMonthTight = isTight;
    setColumnWidth(m_monthIndex, monthColumnWidth());
}

int DatePicker::monthColumnWidth()
{
    QFontMetrics fm = fontMetrics();
    int maxWidth = 0;
    
    QStringList items = m_columns[m_monthIndex]->items();
    for (const QString& item : items) {
        maxWidth = qMax(maxWidth, fm.horizontalAdvance(item));
    }
    maxWidth += 20;
    
    if (m_MONTH == "month") {
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
    int days = m_calendar.daysInMonth(month, year);
    
    CycleListWidget* c = panel->column(m_dayIndex);
    QString day = c->currentItem()->text();
    
    QList<QVariant> dayList;
    for (int i = 1; i <= days; ++i) {
        dayList << i;
    }
    setColumnItems(m_dayIndex, dayList);
    
    c->setItems(m_columns[m_dayIndex]->items());
    c->setSelectedItem(day);
}

void DatePicker::onConfirmed(const QStringList& value)
{
    int year = decodeValue(m_yearIndex, value[m_yearIndex]).toInt();
    int month = decodeValue(m_monthIndex, value[m_monthIndex]).toInt();
    int day = decodeValue(m_dayIndex, value[m_dayIndex]).toInt();
    
    QDate date(year, month, day);
    QDate oldDate = m_date;
    setDate(date);
    
    if (oldDate != date) {
        emit dateChanged(date);
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
    for (int i = 1; i <= date.daysInMonth(); ++i) {
        dayList << i;
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
