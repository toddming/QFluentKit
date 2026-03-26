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
    : QProgressBar(parent), m_val(0), m_useAni(useAni),
      m_lightBackgroundColor(QColor(0, 0, 0, 155)),
      m_darkBackgroundColor(QColor(255, 255, 255, 155)),
      m_isPaused(false), m_isError(false)
{
    setFixedHeight(4);
    setMinimum(0);
    setMaximum(100);
    setValue(0);

    m_ani.setTargetObject(this);
    m_ani.setPropertyName("val");
    m_ani.setEasingCurve(QEasingCurve::OutQuad);
    connect(this, &QProgressBar::valueChanged, this, &ProgressBar::onValueChanged);
}

bool ProgressBar::isUseAni() const
{
    return m_useAni;
}

void ProgressBar::setUseAni(bool isUse)
{
    if (m_useAni != isUse) {
        m_useAni = isUse;
        emit useAniChanged(isUse);
    }
}

float ProgressBar::value() const
{
    return m_val;
}

void ProgressBar::setVal(float v)
{
    if (qFuzzyCompare(m_val, v)) return;
    m_val = v;
    emit valChanged(m_val);
    update();
}

void ProgressBar::onValueChanged(int value)
{
    if (!m_useAni) {
        m_val = value;
        return;
    }

    m_ani.stop();
    m_ani.setEndValue(value);
    m_ani.setDuration(150);
    m_ani.start();
    QProgressBar::setValue(value); // 触发值更新
}

void ProgressBar::resume()
{
    m_isPaused = false;
    m_isError = false;
    update();
}

void ProgressBar::pause()
{
    m_isPaused = true;
    update();
}

void ProgressBar::setPaused(bool isPaused)
{
    m_isPaused = isPaused;
    update();
}

bool ProgressBar::isPaused() const
{
    return m_isPaused;
}

void ProgressBar::error()
{
    m_isError = true;
    update();
}

void ProgressBar::setError(bool isError)
{
    m_isError = isError;
    if (isError) {
        error();
    } else {
        resume();
    }
}

bool ProgressBar::isError() const
{
    return m_isError;
}

void ProgressBar::setCustomBarColor(const QColor &light, const QColor &dark)
{
    m_lightBarColor = light;
    m_darkBarColor = dark;
    update();
}

void ProgressBar::setCustomBackgroundColor(const QColor &light, const QColor &dark)
{
    m_lightBackgroundColor = light;
    m_darkBackgroundColor = dark;
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
    QColor color = Theme::instance()->isDarkTheme() ? m_darkBarColor : m_lightBarColor;
    color = color.isValid() ? color : Theme::instance()->themeColor();
    return color;
}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // 绘制背景线
    QColor bc = Theme::instance()->isDarkTheme() ? m_darkBackgroundColor : m_lightBackgroundColor;
    painter.setPen(bc);
    int y = height() / 2;
    painter.drawLine(0, y, width(), y);

    if (minimum() >= maximum()) return;

    // 绘制进度条
    painter.setPen(Qt::NoPen);
    painter.setBrush(barColor());
    int w = static_cast<int>(m_val / (maximum() - minimum()) * width());
    int r = height() / 2;
    painter.drawRoundedRect(0, 0, w, height(), r, r);
}
