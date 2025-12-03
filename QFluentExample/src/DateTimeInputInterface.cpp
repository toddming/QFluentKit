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
    addExampleCard("日期选择器", new ZhDatePicker(this));
    addExampleCard("时间选择器", new AMTimePicker(this));
    addExampleCard("24小时制的时间选择器", new TimePicker(this, false));
    addExampleCard("显示秒的时间选择器", new TimePicker(this, true));
}
