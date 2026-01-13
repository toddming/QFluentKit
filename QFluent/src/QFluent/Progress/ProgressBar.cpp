#include "ProgressBar.h"
#include <QApplication>
#include <QPalette>
#include <QPainter>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionProgressBar>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QPainter>
#include <QColor>

#include "Theme.h"

ProgressBar::ProgressBar(QWidget *parent, bool useAni)
    : QProgressBar(parent), _val(0), _useAni(useAni),
      lightBackgroundColor(QColor(0, 0, 0, 155)),
      darkBackgroundColor(QColor(255, 255, 255, 155)),
      _isPaused(false), _isError(false)
{
    setFixedHeight(4);
    setMinimum(0);
    setMaximum(100);
    setValue(0);

    ani.setTargetObject(this);
    ani.setPropertyName("val");
    ani.setEasingCurve(QEasingCurve::OutQuad);
    connect(this, &QProgressBar::valueChanged, this, &ProgressBar::_onValueChanged);
}

bool ProgressBar::isUseAni() const
{
    return _useAni;
}

void ProgressBar::setUseAni(bool isUse)
{
    if (_useAni != isUse) {
        _useAni = isUse;
        emit useAniChanged(isUse);
    }
}

float ProgressBar::getVal() const
{
    return _val;
}

void ProgressBar::setVal(float v)
{
    if (qFuzzyCompare(_val, v)) return;
    _val = v;
    emit valChanged(_val);
    update();
}

void ProgressBar::_onValueChanged(int value)
{
    if (!_useAni) {
        _val = value;
        return;
    }

    ani.stop();
    ani.setEndValue(value);
    ani.setDuration(150);
    ani.start();
    QProgressBar::setValue(value); // 触发值更新
}

void ProgressBar::resume()
{
    _isPaused = false;
    _isError = false;
    update();
}

void ProgressBar::pause()
{
    _isPaused = true;
    update();
}

void ProgressBar::setPaused(bool isPaused)
{
    _isPaused = isPaused;
    update();
}

bool ProgressBar::isPaused() const
{
    return _isPaused;
}

void ProgressBar::error()
{
    _isError = true;
    update();
}

void ProgressBar::setError(bool isError)
{
    _isError = isError;
    if (isError) {
        error();
    } else {
        resume();
    }
}

bool ProgressBar::isError() const
{
    return _isError;
}

void ProgressBar::setCustomBarColor(const QColor &light, const QColor &dark)
{
    _lightBarColor = light;
    _darkBarColor = dark;
    update();
}

void ProgressBar::setCustomBackgroundColor(const QColor &light, const QColor &dark)
{
    lightBackgroundColor = light;
    darkBackgroundColor = dark;
    update();
}

QColor ProgressBar::barColor() const
{
    if (isPaused()) {
        return Theme::instance()->isDarkTheme() ? QColor(252, 225, 0) : QColor(157, 93, 0);
    }
    if (isError()) {
        return Theme::instance()->isDarkTheme() ? QColor(255, 153, 164) : QColor(196, 43, 28);
    }
    QColor color = Theme::instance()->isDarkTheme() ? _darkBarColor : _lightBarColor;
    color = color.isValid() ? color : Theme::instance()->themeColor();
    return color;
}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // 绘制背景线
    QColor bc = Theme::instance()->isDarkTheme() ? darkBackgroundColor : lightBackgroundColor;
    painter.setPen(bc);
    int y = height() / 2;
    painter.drawLine(0, y, width(), y);

    if (minimum() >= maximum()) return;

    // 绘制进度条
    painter.setPen(Qt::NoPen);
    painter.setBrush(barColor());
    int w = static_cast<int>(_val / (maximum() - minimum()) * width());
    int r = height() / 2;
    painter.drawRoundedRect(0, 0, w, height(), r, r);
}
