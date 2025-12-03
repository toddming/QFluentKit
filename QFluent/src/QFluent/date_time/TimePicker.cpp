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
    m_AM = tr("上午");
    m_PM = tr("下午");
}

QString AMPMFormatter::encode(const QVariant& value)
{
    if (!value.toString().isEmpty() && !value.canConvert<int>()) {
        return value.toString();
    }
    
    int hour = value.toInt();
    return hour < 12 ? m_AM : m_PM;
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
    int w = showSeconds ? 80 : 120;
    
    QList<QVariant> hours, minutes, seconds;
    for (int i = 0; i < 24; ++i) {
        hours << i;
    }
    for (int i = 0; i < 60; ++i) {
        minutes << i;
        seconds << i;
    }
    
    addColumn(tr("时"), hours, w, Qt::AlignCenter, new DigitFormatter(this));
    addColumn(tr("分"), minutes, w, Qt::AlignCenter, new MinuteFormatter(this));
    addColumn(tr("秒"), seconds, w, Qt::AlignCenter, new MinuteFormatter(this));
    
    setColumnVisible(2, showSeconds);
}

void TimePicker::setTime(const QTime& time)
{
    if (!time.isValid() || time.isNull()) {
        return;
    }
    
    m_time = time;
    setColumnValue(0, time.hour());
    setColumnValue(1, time.minute());
    setColumnValue(2, time.second());
}

void TimePicker::setSecondVisible(bool isVisible)
{
    m_isSecondVisible = isVisible;
    setColumnVisible(2, isVisible);
    
    int w = isVisible ? 80 : 120;
    for (PickerColumnButton* button : m_columns) {
        button->setFixedWidth(w);
    }
}

void TimePicker::onConfirmed(const QStringList& value)
{
    PickerBase::onConfirmed(value);
    
    int h = decodeValue(0, value[0]).toInt();
    int m = decodeValue(1, value[1]).toInt();
    int s = (value.size() == 2) ? 0 : decodeValue(2, value[2]).toInt();
    
    QTime time(h, m, s);
    QTime oldTime = m_time;
    setTime(time);
    
    if (oldTime != time) {
        emit timeChanged(time);
    }
}

QStringList TimePicker::panelInitialValue()
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
    
    QTime time = QTime::currentTime();
    QString h = encodeValue(0, time.hour());
    QString m = encodeValue(1, time.minute());
    QString s = encodeValue(2, time.second());
    
    return isSecondVisible() ? QStringList{h, m, s} : QStringList{h, m};
}

// AMTimePicker 实现
AMTimePicker::AMTimePicker(QWidget* parent, bool showSeconds)
    : TimePickerBase(parent, showSeconds)
{
    m_AM = tr("上午");
    m_PM = tr("下午");
    
    QList<QVariant> hours, minutes, seconds, ampm;
    for (int i = 1; i <= 12; ++i) {
        hours << i;
    }
    for (int i = 0; i < 60; ++i) {
        minutes << i;
        seconds << i;
    }
    ampm << m_AM << m_PM;
    
    addColumn(tr("时"), hours, 80, Qt::AlignCenter, new AMHourFormatter(this));
    addColumn(tr("分"), minutes, 80, Qt::AlignCenter, new MinuteFormatter(this));
    addColumn(tr("秒"), seconds, 80, Qt::AlignCenter, new MinuteFormatter(this));
    setColumnVisible(2, showSeconds);
    
    addColumn(m_AM, ampm, 80, Qt::AlignCenter, new AMPMFormatter(this));
}

void AMTimePicker::setSecondVisible(bool isVisible)
{
    m_isSecondVisible = isVisible;
    setColumnVisible(2, isVisible);
}

void AMTimePicker::setTime(const QTime& time)
{
    if (!time.isValid() || time.isNull()) {
        return;
    }
    
    m_time = time;
    setColumnValue(0, time.hour());
    setColumnValue(1, time.minute());
    setColumnValue(2, time.second());
    setColumnValue(3, time.hour());
}

void AMTimePicker::onConfirmed(const QStringList& value)
{
    PickerBase::onConfirmed(value);
    
    int h, m, s;
    QString p;
    
    if (value.size() == 3) {
        h = decodeValue(0, value[0]).toInt();
        m = decodeValue(1, value[1]).toInt();
        p = value[2];
        s = 0;
    } else {
        h = decodeValue(0, value[0]).toInt();
        m = decodeValue(1, value[1]).toInt();
        s = decodeValue(2, value[2]).toInt();
        p = value[3];
    }
    
    if (p == m_AM) {
        h = (h == 12) ? 0 : h;
    } else if (p == m_PM) {
        h = (h == 12) ? h : h + 12;
    }
    
    QTime time(h, m, s);
    QTime oldTime = m_time;
    setTime(time);
    
    if (oldTime != time) {
        emit timeChanged(time);
    }
}

QStringList AMTimePicker::panelInitialValue()
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
    
    QTime time = QTime::currentTime();
    QString h = encodeValue(0, time.hour());
    QString m = encodeValue(1, time.minute());
    QString s = encodeValue(2, time.second());
    QString p = encodeValue(3, time.hour());
    
    return isSecondVisible() ? QStringList{h, m, s, p} : QStringList{h, m, p};
}
