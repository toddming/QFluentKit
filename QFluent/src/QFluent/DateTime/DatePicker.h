#pragma once

#include "PickerBase.h"
#include <QDate>
#include <QCalendar>

#include "FluentGlobal.h"

class QCalendar;
class MonthFormatter : public PickerColumnFormatter
{
    Q_OBJECT

public:
    explicit MonthFormatter(QObject* parent = nullptr);
    QString encode(const QVariant& value) override;
    QVariant decode(const QString& value) override;

private:
    QStringList m_months;
};

class QFLUENT_EXPORT DatePickerBase : public PickerBase
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ getDate WRITE setDate)

public:
    explicit DatePickerBase(QWidget* parent = nullptr);

    QDate getDate() const { return m_date; }
    virtual void setDate(const QDate& date) = 0;
    
    void setYearFormatter(PickerColumnFormatter* formatter);
    void setMonthFormatter(PickerColumnFormatter* formatter);
    void setDayFormatter(PickerColumnFormatter* formatter);
    
    PickerColumnFormatter* yearFormatter() const;
    PickerColumnFormatter* monthFormatter() const;
    PickerColumnFormatter* dayFormatter() const;

public slots:
    void reset() override;

signals:
    void dateChanged(const QDate& date);

protected:
    QDate m_date;
    QCalendar m_calendar;
    PickerColumnFormatter* m_yearFormatter;
    PickerColumnFormatter* m_monthFormatter;
    PickerColumnFormatter* m_dayFormatter;
};

class QFLUENT_EXPORT DatePicker : public DatePickerBase
{
    Q_OBJECT

public:
    enum DateFormat {
        MM_DD_YYYY = 0,
        YYYY_MM_DD = 1
    };

    explicit DatePicker(QWidget* parent = nullptr, DateFormat format = MM_DD_YYYY, bool isMonthTight = true);

    void setDateFormat(DateFormat format);
    void setDate(const QDate& date) override;
    void setMonthTight(bool isTight);

protected:
    QStringList panelInitialValue() override;
    void onConfirmed(const QStringList& value) override;
    void onColumnValueChanged(PickerPanel* panel, int index, const QString& value) override;

private:
    int monthColumnWidth();

    bool m_isMonthTight;
    DateFormat m_dateFormat;
    int m_monthIndex;
    int m_dayIndex;
    int m_yearIndex;
    QString m_MONTH;
    QString m_YEAR;
    QString m_DAY;
};

class ZhFormatter : public PickerColumnFormatter
{
    Q_OBJECT

public:
    explicit ZhFormatter(const QString& suffix, QObject* parent = nullptr);
    QString encode(const QVariant& value) override;
    QVariant decode(const QString& value) override;

protected:
    QString m_suffix;
};

class ZhYearFormatter : public ZhFormatter
{
    Q_OBJECT

public:
    explicit ZhYearFormatter(QObject* parent = nullptr)
        : ZhFormatter("年", parent) {}
};

class ZhMonthFormatter : public ZhFormatter
{
    Q_OBJECT

public:
    explicit ZhMonthFormatter(QObject* parent = nullptr)
        : ZhFormatter("月", parent) {}
};

class ZhDayFormatter : public ZhFormatter
{
    Q_OBJECT

public:
    explicit ZhDayFormatter(QObject* parent = nullptr)
        : ZhFormatter("日", parent) {}
};

class QFLUENT_EXPORT ZhDatePicker : public DatePicker
{
    Q_OBJECT

public:
    explicit ZhDatePicker(QWidget* parent = nullptr);
};
