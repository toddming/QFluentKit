#include "IndeterminateProgressBar.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPainter>
#include <QColor>
#include <QEasingCurve>

#include "Theme.h"

IndeterminateProgressBar::IndeterminateProgressBar(QWidget *parent, bool start)
    : QProgressBar(parent)
{
    // 初始化动画
    m_shortBarAni = new QPropertyAnimation(this, "shortPos", this);
    m_longBarAni = new QPropertyAnimation(this, "longPos", this);

    m_aniGroup = new QParallelAnimationGroup(this);
    m_longBarAniGroup = new QSequentialAnimationGroup(this);

    // 配置短条动画
    m_shortBarAni->setDuration(833);
    m_shortBarAni->setStartValue(0.0);
    m_shortBarAni->setEndValue(1.45);

    // 配置长条动画
    m_longBarAni->setDuration(1167);
    m_longBarAni->setStartValue(0.0);
    m_longBarAni->setEndValue(1.75);
    m_longBarAni->setEasingCurve(QEasingCurve::OutQuad);

    // 构建动画组
    m_longBarAniGroup->addPause(785);
    m_longBarAniGroup->addAnimation(m_longBarAni);

    m_aniGroup->addAnimation(m_shortBarAni);
    m_aniGroup->addAnimation(m_longBarAniGroup);
    m_aniGroup->setLoopCount(-1); // 无限循环

    setFixedHeight(4);

    if (start) {
        this->start();
    }
}

IndeterminateProgressBar::~IndeterminateProgressBar()
{
    // 由于所有动画对象都有父对象，会自动销毁
    // 不需要手动删除
}

QColor IndeterminateProgressBar::lightBarColor() const
{
    return m_lightBarColor.isValid() ? m_lightBarColor : Theme::instance()->themeColor();
}

QColor IndeterminateProgressBar::darkBarColor() const
{
    return m_darkBarColor.isValid() ? m_darkBarColor : Theme::instance()->themeColor();
}

void IndeterminateProgressBar::setCustomBarColor(const QColor &light, const QColor &dark)
{
    m_lightBarColor = light;
    m_darkBarColor = dark;
    update();
}

qreal IndeterminateProgressBar::shortPos() const
{
    return m_shortPos;
}

void IndeterminateProgressBar::setShortPos(qreal pos)
{
    if (qFuzzyCompare(m_shortPos, pos))
        return;

    m_shortPos = pos;
    update();
}

qreal IndeterminateProgressBar::longPos() const
{
    return m_longPos;
}

void IndeterminateProgressBar::setLongPos(qreal pos)
{
    if (qFuzzyCompare(m_longPos, pos))
        return;

    m_longPos = pos;
    update();
}

void IndeterminateProgressBar::start()
{
    m_shortPos = 0;
    m_longPos = 0;
    m_aniGroup->start();
    update();
}

void IndeterminateProgressBar::stop()
{
    m_aniGroup->stop();
    m_shortPos = 0;
    m_longPos = 0;
    update();
}

bool IndeterminateProgressBar::isStarted() const
{
    return m_aniGroup->state() == QAbstractAnimation::Running;
}

void IndeterminateProgressBar::pause()
{
    m_aniGroup->pause();
    update();
}

void IndeterminateProgressBar::resume()
{
    m_aniGroup->resume();
    update();
}

void IndeterminateProgressBar::setPaused(bool isPaused)
{
    m_aniGroup->setPaused(isPaused);
    update();
}

bool IndeterminateProgressBar::isPaused() const
{
    return m_aniGroup->state() == QAbstractAnimation::Paused;
}

void IndeterminateProgressBar::error()
{
    m_isError = true;
    m_aniGroup->stop();
    update();
}

void IndeterminateProgressBar::setError(bool isError)
{
    m_isError = isError;
    if (isError) {
        error();
    } else {
        start();
    }
}

bool IndeterminateProgressBar::isError() const
{
    return m_isError;
}

QColor IndeterminateProgressBar::barColor() const
{
    if (m_isError) {
        return Theme::isDark() ? QColor(255, 153, 164) : QColor(196, 43, 28);
    }

    if (isPaused()) {
        return Theme::isDark() ? QColor(252, 225, 0) : QColor(157, 93, 0);
    }

    return Theme::isDark() ? darkBarColor() : lightBarColor();
}

void IndeterminateProgressBar::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(barColor());

    // 绘制短条
    int x = static_cast<int>((m_shortPos - 0.4) * width());
    int w = static_cast<int>(0.4 * width());
    qreal r = height() / 2.0;
    painter.drawRoundedRect(x, 0, w, height(), r, r);

    // 绘制长条
    x = static_cast<int>((m_longPos - 0.6) * width());
    w = static_cast<int>(0.6 * width());
    painter.drawRoundedRect(x, 0, w, height(), r, r);
}
