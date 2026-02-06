#include "CalendarPicker.h"
#include <QPainter>
#include <QStyle>
#include <QApplication>
#include <QPoint>
#include <QDate>
#include <QVariant>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"


CalendarPicker::CalendarPicker(QWidget* parent)
    : QPushButton(parent),
      m_date(),
      m_dateFormat(QLocale::system().dateFormat(QLocale::ShortFormat)),
      m_isResetEnabled(false)
{
    setText("选择日期");

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::CALENDAR_PICKER);
    connect(this, &QPushButton::clicked, this, &CalendarPicker::onShowCalendarView);
}

QDate CalendarPicker::getDate() const
{
    return m_date;
}

void CalendarPicker::setDate(const QDate& date)
{
    onDateChanged(date);
}

void CalendarPicker::reset()
{
    m_date = QDate();
    setText("选择日期");
    setProperty("hasDate", false);
    setStyle(QApplication::style());
    update();
}

QString CalendarPicker::getDateFormat() const
{
    return m_dateFormat;
}

void CalendarPicker::setDateFormat(const QString& format)
{
    m_dateFormat = format;
    if (m_date.isValid()) {
        setText(m_date.toString(m_dateFormat));
    }
}

bool CalendarPicker::isResetEnabled() const
{
    return m_isResetEnabled;
}

void CalendarPicker::setResetEnabled(bool isEnabled)
{
    m_isResetEnabled = isEnabled;
}

void CalendarPicker::onShowCalendarView()
{
    CalendarView* view = new CalendarView(window());
    view->setAttribute(Qt::WA_DeleteOnClose);  // 修复内存泄漏：窗口关闭时自动删除
    view->setResetEnabled(isResetEnabled());

    connect(view, &CalendarView::resetted, this, &CalendarPicker::reset);
    connect(view, &CalendarView::dateChanged, this, &CalendarPicker::onDateChanged);

    if (m_date.isValid()) {
        view->setDate(m_date);
    }

    int x = this->width() / 2 - view->sizeHint().width() / 2;
    int y = this->height();
    view->exec(mapToGlobal(QPoint(x, y)));
}

void CalendarPicker::onDateChanged(const QDate& date)
{
    m_date = QDate(date);
    setText(date.toString(m_dateFormat));
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
