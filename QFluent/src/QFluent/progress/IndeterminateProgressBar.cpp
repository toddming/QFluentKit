#include "IndeterminateProgressBar.h"
#include <QApplication>
#include <QPalette>
#include <QPainter>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionProgressBar>

#include "Theme.h"

IndeterminateProgressBar::IndeterminateProgressBar(QWidget *parent, bool start)
    : QProgressBar(parent), _shortPos(0), _longPos(0),
      _isError(false)
{
    setFixedHeight(4);
    setMinimum(0);
    setMaximum(100);
    setFormat("");

    // 配置动画
    shortBarAni.setTargetObject(this);
    shortBarAni.setPropertyName("shortPos");
    shortBarAni.setDuration(833);
    shortBarAni.setStartValue(0);
    shortBarAni.setEndValue(1.45);

    longBarAni.setTargetObject(this);
    longBarAni.setPropertyName("longPos");
    longBarAni.setDuration(1167);
    longBarAni.setStartValue(0);
    longBarAni.setEndValue(1.75);
    longBarAni.setEasingCurve(QEasingCurve::OutQuad);

    longBarAniGroup.addPause(785);
    longBarAniGroup.addAnimation(&longBarAni);
    aniGroup.addAnimation(&shortBarAni);
    aniGroup.addAnimation(&longBarAniGroup);
    aniGroup.setLoopCount(-1);

    if (start) this->start();
}

IndeterminateProgressBar::~IndeterminateProgressBar()
{

}

bool IndeterminateProgressBar::isUseAni() const
{
    return aniGroup.state() != QAbstractAnimation::Stopped;
}

void IndeterminateProgressBar::setUseAni(bool isUse)
{
    if (isUse) {
        if (aniGroup.state() == QAbstractAnimation::Stopped) {
            start();
        }
    } else {
        stop();
    }
}

float IndeterminateProgressBar::shortPos() const
{
    return _shortPos;
}

void IndeterminateProgressBar::setShortPos(float p)
{
    if (qFuzzyCompare(_shortPos, p)) return;
    _shortPos = p;
    emit shortPosChanged(p);
    update();
}

float IndeterminateProgressBar::longPos() const
{
    return _longPos;
}

void IndeterminateProgressBar::setLongPos(float p)
{
    if (qFuzzyCompare(_longPos, p)) return;
    _longPos = p;
    emit longPosChanged(p);
    update();
}

void IndeterminateProgressBar::start()
{
    _shortPos = 0;
    _longPos = 0;
    aniGroup.start();
    update();
}

void IndeterminateProgressBar::stop()
{
    aniGroup.stop();
    _shortPos = 0;
    _longPos = 0;
    update();
}

bool IndeterminateProgressBar::isStarted() const
{
    return aniGroup.state() == QAbstractAnimation::Running;
}

void IndeterminateProgressBar::pause()
{
    aniGroup.pause();
    update();
}

void IndeterminateProgressBar::resume()
{
    aniGroup.resume();
    update();
}

void IndeterminateProgressBar::setPaused(bool isPaused)
{
    aniGroup.setPaused(isPaused);
    update();
}

bool IndeterminateProgressBar::isPaused() const
{
    return aniGroup.state() == QAbstractAnimation::Paused;
}

void IndeterminateProgressBar::error()
{
    _isError = true;
    aniGroup.stop();
    update();
}

void IndeterminateProgressBar::setError(bool isError)
{
    _isError = isError;
    if (isError) {
        error();
    } else {
        start();
    }
}

bool IndeterminateProgressBar::isError() const
{
    return _isError;
}

void IndeterminateProgressBar::setCustomBarColor(const QColor &light, const QColor &dark)
{
    _lightBarColor = light;
    _darkBarColor = dark;
    update();
}

QColor IndeterminateProgressBar::barColor() const
{
    if (isError()) {
        return Theme::instance()->isDarkMode() ? QColor(255, 153, 164) : QColor(196, 43, 28);
    }
    if (isPaused()) {
        return Theme::instance()->isDarkMode() ? QColor(252, 225, 0) : QColor(157, 93, 0);
    }
    return Theme::instance()->isDarkMode() ? _darkBarColor : _lightBarColor;
}

void IndeterminateProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(barColor());

    // 绘制短进度条
    int shortX = static_cast<int>((_shortPos - 0.4) * width());
    int shortW = static_cast<int>(0.4 * width());
    int r = height() / 2;
    painter.drawRoundedRect(shortX, 0, shortW, height(), r, r);

    // 绘制长进度条
    int longX = static_cast<int>((_longPos - 0.6) * width());
    int longW = static_cast<int>(0.6 * width());
    painter.drawRoundedRect(longX, 0, longW, height(), r, r);
}
