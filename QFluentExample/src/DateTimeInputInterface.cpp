#include "DateTimeInputInterface.h"

#include "QFluent/date_time/CalendarPicker.h"

DateTimeInputInterface::DateTimeInputInterface(QWidget *parent)
    : GalleryInterface("基本输入", "", parent)
{
    setObjectName("DateTimeInputInterface");

    addExampleCard("日历选择器", new CalendarPicker(this));

    auto calendarView = new CalendarView(this);
    calendarView->setDisabled(true);
    addExampleCard("流畅日历", calendarView);
}
