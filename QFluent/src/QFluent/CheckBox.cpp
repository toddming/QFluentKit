#include "CheckBox.h"

#include <QRect>
#include <QPainterPath>
#include <QPainter>
#include <QColor>
#include <QStyleOptionButton>

#include "Theme.h"
#include "Icon.h"
#include "Define.h"
#include "Private/CheckBoxPrivate.h"

CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent), d_ptr(new CheckBoxPrivate())
{
    Q_D(CheckBox);
    d->q_ptr = this;

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::CHECK_BOX);

    d->_isPressed = false;
    d->_isHover = false;
}

CheckBox::CheckBox(const QString &text, QWidget *parent) : CheckBox(parent)
{
    setText(text);
}

CheckBox::~CheckBox() {}

void CheckBox::mousePressEvent(QMouseEvent *event) {
    Q_D(CheckBox);
    d->_isPressed = true;
    QCheckBox::mousePressEvent(event);
}

void CheckBox::mouseReleaseEvent(QMouseEvent *event) {
    Q_D(CheckBox);

    d->_isPressed = false;
    QCheckBox::mouseReleaseEvent(event);
}

void CheckBox::enterEvent(QEnterEvent *event) {
    Q_D(CheckBox);

    d->_isHover = true;
    update();
    QCheckBox::enterEvent(event);
}

void CheckBox::leaveEvent(QEvent *event) {
    Q_D(CheckBox);

    d->_isHover = false;
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


    if (checkState() == Qt::Checked) {
        Icon::drawSvgIcon(&painter, IconType::FLuentIcon::ACCEPT, rect.adjusted(3, 3, -3, -3));
    } else if (checkState() == Qt::PartiallyChecked) {

    }
}
