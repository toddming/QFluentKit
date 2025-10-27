#ifndef CALENDARPICKER_H
#define CALENDARPICKER_H

#include <QPushButton>

#include "Property.h"
#include "CalendarView.h"

using FIF = FluentIconType::IconType;


class QFLUENT_EXPORT CalendarPicker : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ getDate WRITE setDate)
    Q_PROPERTY(QString dateFormat READ getDateFormat WRITE setDateFormat)

public:
    explicit CalendarPicker(QWidget* parent = nullptr);
    ~CalendarPicker() = default;

    QDate getDate() const;
    void setDate(const QDate& date);

    void reset();
    QString getDateFormat() const;
    void setDateFormat(QString format);
    bool isResetEnabled() const;
    void setResetEnabled(bool isEnabled);

signals:
    void dateChanged(const QDate& date);

protected:
    void paintEvent(QPaintEvent* e) override;

private slots:
    void _showCalendarView();
    void _onDateChanged(const QDate& date);

private:
    QDate _date;
    QString _dateFormat;
    bool _isResetEnabled;
};

#endif // CALENDARPICKER_H
