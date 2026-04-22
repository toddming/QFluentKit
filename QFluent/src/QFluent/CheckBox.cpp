#include "CheckBox.h"

#include <QRect>
#include <QPainterPath>
#include <QPainter>
#include <QColor>
#include <QStyleOptionButton>

#include "Theme.h"
#include "FluentIcon.h"
#include "FluentGlobal.h"
#include "StyleSheet.h"

#include "Private/CheckBoxPrivate.h"

CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent)
  , d_ptr(new CheckBoxPrivate)
{
    Q_D(CheckBox);
    d->q_ptr = this;

    StyleSheet::registerWidget(this, Fluent::ThemeStyle::CHECK_BOX);
}

CheckBox::CheckBox(const QString &text, QWidget *parent) : CheckBox(parent)
{
    setText(text);
}

CheckBox::~CheckBox() = default;

void CheckBox::mousePressEvent(QMouseEvent *event) {
    Q_D(CheckBox);
    d->m_isPressed = true;
    QCheckBox::mousePressEvent(event);
}

void CheckBox::mouseReleaseEvent(QMouseEvent *event) {
    Q_D(CheckBox);

    d->m_isPressed = false;
    QCheckBox::mouseReleaseEvent(event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void CheckBox::enterEvent(QEnterEvent *event) {
    Q_D(CheckBox);

    d->m_isHover = true;
    update();
    QCheckBox::enterEvent(event);
}
#else
void CheckBox::enterEvent(QEvent *event) {
    Q_D(CheckBox);

    d->m_isHover = true;
    update();
    QCheckBox::enterEvent(event);
}
#endif

void CheckBox::leaveEvent(QEvent *event) {
    Q_D(CheckBox);

    d->m_isHover = false;
    update();
    QCheckBox::leaveEvent(event);
}



void CheckBox::paintEvent(QPaintEvent *event) {
    Q_D(CheckBox);

    QCheckBox::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 获取指示器矩形
    QStyleOptionButton opt;
    opt.initFrom(this);
    QRect rect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, this);

    // 绘制形状：圆角矩形
    painter.setPen(d->borderColor());
    painter.setBrush(d->backgroundColor());
    painter.drawRoundedRect(rect, 4.5, 4.5);

    if (!isEnabled()) {
        painter.setOpacity(0.8);
    }

    const QString fillPath = ":/qfluent/images/check_box/%1_{color}.svg";
    if (checkState() == Qt::Checked) {
        FluentIconUtils::drawIcon(FluentIcon(fillPath.arg("Accept")), &painter, rect);
    } else if (checkState() == Qt::PartiallyChecked) {
        FluentIconUtils::drawIcon(FluentIcon(fillPath.arg("PartialAccept")), &painter, rect);
    }
}
