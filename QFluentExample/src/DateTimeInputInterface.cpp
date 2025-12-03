#include "DateTimeInputInterface.h"

#include "QFluent/date_time/DatePicker.h"
#include "QFluent/date_time/TimePicker.h"
#include "QFluent/date_time/CalendarPicker.h"

DateTimeInputInterface::DateTimeInputInterface(QWidget *parent)
    : GalleryInterface("日期和时间", "", parent)
{
    setObjectName("DateTimeInputInterface");

    addExampleCard("日历选择器", new CalendarPicker(this));

    auto calendarView = new CalendarView(this);
    calendarView->setDisabled(true);
    addExampleCard("流畅日历", calendarView);

    ZhDatePicker* zhPicker = new ZhDatePicker(this);
    // zhPicker->setDate(QDate::currentDate());
    addExampleCard("日期选择器", zhPicker);

    AMTimePicker* amPicker = new AMTimePicker(this);
    // amPicker->setTime(QTime::currentTime());
    addExampleCard("时间选择器", amPicker);


    TimePicker* timePicker = new TimePicker(this, false);
    timePicker->setTime(QTime::currentTime());
    addExampleCard("24小时制的时间选择器", timePicker);

    TimePicker* timePicker2 = new TimePicker(this, true);
    timePicker2->setTime(QTime::currentTime());
    addExampleCard("显示秒的时间选择器", timePicker2);
}
