#pragma once

#include <QPushButton>

#include "FluentGlobal.h"
#include "CalendarView.h"

class QFLUENT_EXPORT CalendarPicker : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate)
    Q_PROPERTY(QString dateFormat READ dateFormat WRITE setDateFormat)

public:
    explicit CalendarPicker(QWidget* parent = nullptr);
    ~CalendarPicker() = default;

    QDate date() const;
    void setDate(const QDate& date);

    void reset();
    QString dateFormat() const;
    void setDateFormat(const QString& format);
    bool isResetEnabled() const;
    void setResetEnabled(bool isEnabled);

signals:
    void dateChanged(const QDate& date);

protected:
    void paintEvent(QPaintEvent* e) override;

private slots:
    void onShowCalendarView();
    void onDateChanged(const QDate& date);

private:
    QDate m_date;
    QString m_dateFormat;
    bool m_isResetEnabled;
};
