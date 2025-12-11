#include "DateTimeInputInterface.h"
#include <QDebug>
#include "QFluent/date_time/DatePicker.h"
#include "QFluent/date_time/TimePicker.h"
#include "QFluent/date_time/CalendarPicker.h"

DateTimeInputInterface::DateTimeInputInterface(QWidget *parent)
    : GalleryInterface("日期和时间", "", parent)
{
    setObjectName("DateTimeInputInterface");

    auto calendar = new CalendarPicker(this);
    connect(calendar, &CalendarPicker::dateChanged, this, [](const QDate &date){
        qDebug() << date;
    });
    addExampleCard("日历选择器", calendar);
    addExampleCard("日期选择器", new ZhDatePicker(this));
    addExampleCard("时间选择器", new AMTimePicker(this));

    addExampleCard("24小时制的时间选择器", new TimePicker(this, false));

    auto timePicker = new TimePicker(this, true);
    connect(timePicker, &TimePicker::timeChanged, this, [](const QTime &time){
        qDebug() << time;
    });
    addExampleCard("显示秒的时间选择器", timePicker);
}
