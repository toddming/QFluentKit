#include "CalendarPicker.h"
#include <QPainter>
#include <QStyle>
#include <QApplication>
#include <QPoint>
#include <QDate>
#include <QPainter>
#include <QStyle>
#include <QApplication>
#include <QPoint>
#include <QVariant>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"


CalendarPicker::CalendarPicker(QWidget* parent)
    : QPushButton(parent),
      _date(),
      _dateFormat(QLocale::system().dateFormat(QLocale::ShortFormat)),
      _isResetEnabled(false)
{
    setText("选择日期");

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::CALENDAR_PICKER);
    connect(this, &QPushButton::clicked, this, &CalendarPicker::_showCalendarView);
}

QDate CalendarPicker::getDate() const
{
    return _date;
}

void CalendarPicker::setDate(const QDate& date)
{
    _onDateChanged(date);
}

void CalendarPicker::reset()
{
    _date = QDate();
    setText("选择日期");
    setProperty("hasDate", false);
    setStyle(QApplication::style());
    update();
}

QString CalendarPicker::getDateFormat() const
{
    return _dateFormat;
}

void CalendarPicker::setDateFormat(QString format)
{
    _dateFormat = format;
    if (_date.isValid()) {
        setText(_date.toString(_dateFormat));
    }
}

bool CalendarPicker::isResetEnabled() const
{
    return _isResetEnabled;
}

void CalendarPicker::setResetEnabled(bool isEnabled)
{
    _isResetEnabled = isEnabled;
}

void CalendarPicker::_showCalendarView()
{
    CalendarView* view = new CalendarView(window());
    view->setResetEnabled(isResetEnabled());

    connect(view, &CalendarView::resetted, this, &CalendarPicker::reset);
    connect(view, &CalendarView::dateChanged, this, &CalendarPicker::_onDateChanged);

    if (_date.isValid()) {
        view->setDate(_date);
    }

    int x = this->width() / 2 - view->sizeHint().width() / 2;
    int y = this->height();
    view->exec(mapToGlobal(QPoint(x, y)));
}

void CalendarPicker::_onDateChanged(const QDate& date)
{
    _date = QDate(date);
    setText(date.toString(_dateFormat));
    setProperty("hasDate", true);
    setStyle(QApplication::style());
    update();
    emit dateChanged(date);
}

void CalendarPicker::paintEvent(QPaintEvent* e)
{
    QPushButton::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    if (!property("hasDate").toBool()) {
        painter.setOpacity(0.6);
    }

    int w = 12;
    QRectF rect(this->width() - 23, this->height() / 2 - w / 2, w, w);
    FluentIcon(Fluent::IconType::CALENDAR).render(&painter, rect);
}
