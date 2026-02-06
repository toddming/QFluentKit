#include "TimePicker.h"

// MinuteFormatter 实现
QString MinuteFormatter::encode(const QVariant& value)
{
    return QString::number(value.toInt()).rightJustified(2, '0');
}

// AMHourFormatter 实现
QString AMHourFormatter::encode(const QVariant& value)
{
    int hour = value.toInt();
    if (hour == 0 || hour == 12) {
        return "12";
    }
    return QString::number(hour % 12);
}

// AMPMFormatter 实现
AMPMFormatter::AMPMFormatter(QObject* parent)
    : PickerColumnFormatter(parent)
{
    m_amLabel = tr("上午");
    m_pmLabel = tr("下午");
}

QString AMPMFormatter::encode(const QVariant& value)
{
    bool isValid = false;
    int hour = value.toInt(&isValid);

    if (!isValid) {
        return value.toString();
    }
    return hour < 12 ? m_amLabel : m_pmLabel;
}

// TimePickerBase 实现
TimePickerBase::TimePickerBase(QWidget* parent, bool showSeconds)
    : PickerBase(parent), m_isSecondVisible(showSeconds)
{
}

void TimePickerBase::reset()
{
    m_time = QTime();
    PickerBase::reset();
}

// TimePicker 实现
TimePicker::TimePicker(QWidget* parent, bool showSeconds)
    : TimePickerBase(parent, showSeconds)
{
    int columnWidth = showSeconds ? 80 : 120;
    
    QList<QVariant> hours, minutes, seconds;
    for (int hour = 0; hour < 24; ++hour) {
        hours << hour;
    }
    for (int minute = 0; minute < 60; ++minute) {
        minutes << minute;
        seconds << minute;
    }
    
    addColumn(tr("时"), hours, columnWidth, Qt::AlignCenter, new DigitFormatter(this));
    addColumn(tr("分"), minutes, columnWidth, Qt::AlignCenter, new MinuteFormatter(this));
    addColumn(tr("秒"), seconds, columnWidth, Qt::AlignCenter, new MinuteFormatter(this));
    
    setColumnVisible(SECOND_INDEX, showSeconds);
}

void TimePicker::setTime(const QTime& time)
{
    if (!time.isValid() || time.isNull()) {
        return;
    }
    
    m_time = time;
    setColumnValue(HOUR_INDEX, time.hour());
    setColumnValue(MINUTE_INDEX, time.minute());
    setColumnValue(SECOND_INDEX, time.second());
}

void TimePicker::setSecondVisible(bool visible)
{
    if (m_isSecondVisible == visible) {
        return;
    }
    
    m_isSecondVisible = visible;
    setColumnVisible(SECOND_INDEX, visible);
    
    // 根据秒列的可见性调整列宽
    int columnWidth = visible ? 80 : 120;
    for (PickerColumnButton* button : std::as_const(m_columns)) {
        button->setFixedWidth(columnWidth);
    }
}

void TimePicker::onConfirmed(const QStringList& value)
{
    PickerBase::onConfirmed(value);
    
    int hour = decodeValue(HOUR_INDEX, value[HOUR_INDEX]).toInt();
    int minute = decodeValue(MINUTE_INDEX, value[MINUTE_INDEX]).toInt();
    int second = (value.size() == 2) ? 0 : decodeValue(SECOND_INDEX, value[SECOND_INDEX]).toInt();
    
    QTime newTime(hour, minute, second);
    QTime oldTime = m_time;
    setTime(newTime);
    
    if (oldTime != newTime) {
        emit timeChanged(newTime);
    }
}

QStringList TimePicker::panelInitialValue()
{
    QStringList val = value();
    bool hasValue = false;
    for (const QString& v : std::as_const(val)) {
        if (!v.isEmpty()) {
            hasValue = true;
            break;
        }
    }
    
    if (hasValue) {
        return val;
    }
    
    QTime currentTime = QTime::currentTime();
    QString hourValue = encodeValue(HOUR_INDEX, currentTime.hour());
    QString minuteValue = encodeValue(MINUTE_INDEX, currentTime.minute());
    QString secondValue = encodeValue(SECOND_INDEX, currentTime.second());
    
    return isSecondVisible() ? QStringList{hourValue, minuteValue, secondValue} 
                             : QStringList{hourValue, minuteValue};
}

// AMTimePicker 实现
AMTimePicker::AMTimePicker(QWidget* parent, bool showSeconds)
    : TimePickerBase(parent, showSeconds)
{
    m_amLabel = tr("上午");
    m_pmLabel = tr("下午");
    
    QList<QVariant> hours, minutes, seconds, ampmValues;
    for (int hour = 1; hour <= 12; ++hour) {
        hours << hour;
    }
    for (int minute = 0; minute < 60; ++minute) {
        minutes << minute;
        seconds << minute;
    }
    ampmValues << m_amLabel << m_pmLabel;
    
    addColumn(tr("时"), hours, 80, Qt::AlignCenter, new AMHourFormatter(this));
    addColumn(tr("分"), minutes, 80, Qt::AlignCenter, new MinuteFormatter(this));
    addColumn(tr("秒"), seconds, 80, Qt::AlignCenter, new MinuteFormatter(this));
    setColumnVisible(SECOND_INDEX, showSeconds);
    
    addColumn(m_amLabel, ampmValues, 80, Qt::AlignCenter, new AMPMFormatter(this));
}

void AMTimePicker::setSecondVisible(bool visible)
{
    m_isSecondVisible = visible;
    setColumnVisible(SECOND_INDEX, visible);
}

void AMTimePicker::setTime(const QTime& time)
{
    if (!time.isValid() || time.isNull()) {
        return;
    }
    
    m_time = time;
    setColumnValue(HOUR_INDEX, time.hour());
    setColumnValue(MINUTE_INDEX, time.minute());
    setColumnValue(SECOND_INDEX, time.second());
    setColumnValue(AMPM_INDEX, time.hour());
}

void AMTimePicker::onConfirmed(const QStringList& value)
{
    PickerBase::onConfirmed(value);
    
    int hour, minute, second;
    QString ampmValue;
    
    if (value.size() == 3) {
        hour = decodeValue(HOUR_INDEX, value[HOUR_INDEX]).toInt();
        minute = decodeValue(MINUTE_INDEX, value[MINUTE_INDEX]).toInt();
        ampmValue = value[2];
        second = 0;
    } else {
        hour = decodeValue(HOUR_INDEX, value[HOUR_INDEX]).toInt();
        minute = decodeValue(MINUTE_INDEX, value[MINUTE_INDEX]).toInt();
        second = decodeValue(SECOND_INDEX, value[SECOND_INDEX]).toInt();
        ampmValue = value[3];
    }
    
    // 转换 12 小时制到 24 小时制
    if (ampmValue == m_amLabel) {
        hour = (hour == 12) ? 0 : hour;
    } else if (ampmValue == m_pmLabel) {
        hour = (hour == 12) ? hour : hour + 12;
    }
    
    QTime newTime(hour, minute, second);
    QTime oldTime = m_time;
    setTime(newTime);
    
    if (oldTime != newTime) {
        emit timeChanged(newTime);
    }
}

QStringList AMTimePicker::panelInitialValue()
{
    QStringList val = value();
    bool hasValue = false;
    for (const QString& v : std::as_const(val)) {
        if (!v.isEmpty()) {
            hasValue = true;
            break;
        }
    }
    
    if (hasValue) {
        return val;
    }
    
    QTime currentTime = QTime::currentTime();
    QString hourValue = encodeValue(HOUR_INDEX, currentTime.hour());
    QString minuteValue = encodeValue(MINUTE_INDEX, currentTime.minute());
    QString secondValue = encodeValue(SECOND_INDEX, currentTime.second());
    QString ampmValue = encodeValue(AMPM_INDEX, currentTime.hour());
    
    return isSecondVisible() ? QStringList{hourValue, minuteValue, secondValue, ampmValue} 
                             : QStringList{hourValue, minuteValue, ampmValue};
}
