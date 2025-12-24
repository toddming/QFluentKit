#pragma once

#include "PickerBase.h"
#include <QTime>

#include "FluentGlobal.h"

class MinuteFormatter : public DigitFormatter
{
    Q_OBJECT

public:
    using DigitFormatter::DigitFormatter;
    QString encode(const QVariant& value) override;
};

class AMHourFormatter : public DigitFormatter
{
    Q_OBJECT

public:
    using DigitFormatter::DigitFormatter;
    QString encode(const QVariant& value) override;
};

class AMPMFormatter : public PickerColumnFormatter
{
    Q_OBJECT

public:
    explicit AMPMFormatter(QObject* parent = nullptr);
    QString encode(const QVariant& value) override;

private:
    QString m_AM;
    QString m_PM;
};

class QFLUENT_EXPORT TimePickerBase : public PickerBase
{
    Q_OBJECT
    Q_PROPERTY(QTime time READ getTime WRITE setTime)
    Q_PROPERTY(bool secondVisible READ isSecondVisible WRITE setSecondVisible)

public:
    explicit TimePickerBase(QWidget* parent = nullptr, bool showSeconds = false);

    QTime getTime() const { return m_time; }
    virtual void setTime(const QTime& time) = 0;
    
    bool isSecondVisible() const { return m_isSecondVisible; }
    virtual void setSecondVisible(bool isVisible) = 0;

public slots:
    void reset() override;

signals:
    void timeChanged(const QTime& time);

protected:
    bool m_isSecondVisible;
    QTime m_time;
};

class QFLUENT_EXPORT TimePicker : public TimePickerBase
{
    Q_OBJECT

public:
    explicit TimePicker(QWidget* parent = nullptr, bool showSeconds = false);

    void setTime(const QTime& time) override;
    void setSecondVisible(bool isVisible) override;

protected:
    QStringList panelInitialValue() override;
    void onConfirmed(const QStringList& value) override;
};

class QFLUENT_EXPORT AMTimePicker : public TimePickerBase
{
    Q_OBJECT

public:
    explicit AMTimePicker(QWidget* parent = nullptr, bool showSeconds = false);

    void setTime(const QTime& time) override;
    void setSecondVisible(bool isVisible) override;

protected:
    QStringList panelInitialValue() override;
    void onConfirmed(const QStringList& value) override;

private:
    QString m_AM;
    QString m_PM;
};
