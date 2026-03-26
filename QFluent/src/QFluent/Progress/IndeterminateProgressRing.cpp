#include "IndeterminateProgressRing.h"
#include <QPainter>
#include <QColor>
#include <QRectF>
#include <QPen>
#include <QPainter>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QVariant>
#include "Theme.h"

IndeterminateProgressRing::IndeterminateProgressRing(QWidget *parent, bool start)
    : QProgressBar(parent),
    m_lightBackgroundColor(QColor(0, 0, 0, 0)),
    m_darkBackgroundColor(QColor(255, 255, 255, 0)),
    m_strokeWidth(6),
    m_startAngle(-180),
    m_spanAngle(0),
    m_startAngleAni1(new QPropertyAnimation(this, "startAngle")),
    m_startAngleAni2(new QPropertyAnimation(this, "startAngle")),
    m_spanAngleAni1(new QPropertyAnimation(this, "spanAngle")),
    m_spanAngleAni2(new QPropertyAnimation(this, "spanAngle")),
    m_startAngleAniGroup(new QSequentialAnimationGroup(this)),
    m_spanAngleAniGroup(new QSequentialAnimationGroup(this)),
    m_aniGroup(new QParallelAnimationGroup(this))
{
    // Initialize start angle animation
    m_startAngleAni1->setDuration(1000);
    m_startAngleAni1->setStartValue(0);
    m_startAngleAni1->setEndValue(450);

    m_startAngleAni2->setDuration(1000);
    m_startAngleAni2->setStartValue(450);
    m_startAngleAni2->setEndValue(1080);

    m_startAngleAniGroup->addAnimation(m_startAngleAni1);
    m_startAngleAniGroup->addAnimation(m_startAngleAni2);

    // Initialize span angle animation
    m_spanAngleAni1->setDuration(1000);
    m_spanAngleAni1->setStartValue(0);
    m_spanAngleAni1->setEndValue(180);

    m_spanAngleAni2->setDuration(1000);
    m_spanAngleAni2->setStartValue(180);
    m_spanAngleAni2->setEndValue(0);

    m_spanAngleAniGroup->addAnimation(m_spanAngleAni1);
    m_spanAngleAniGroup->addAnimation(m_spanAngleAni2);

    m_aniGroup->addAnimation(m_startAngleAniGroup);
    m_aniGroup->addAnimation(m_spanAngleAniGroup);
    m_aniGroup->setLoopCount(-1);

    setFixedSize(80, 80);

    if (start) {
        this->start();
    }
}

IndeterminateProgressRing::~IndeterminateProgressRing()
{
    // Qt will automatically clean up child objects (animations with parent set)
    // No need for manual deletion
}

int IndeterminateProgressRing::startAngle() const
{
    return m_startAngle;
}

void IndeterminateProgressRing::setStartAngle(int angle)
{
    m_startAngle = angle;
    update();
}

int IndeterminateProgressRing::spanAngle() const
{
    return m_spanAngle;
}

void IndeterminateProgressRing::setSpanAngle(int angle)
{
    m_spanAngle = angle;
    update();
}

int IndeterminateProgressRing::strokeWidth() const
{
    return m_strokeWidth;
}

void IndeterminateProgressRing::setStrokeWidth(int w)
{
    m_strokeWidth = w;
    update();
}

void IndeterminateProgressRing::start()
{
    m_startAngle = 0;
    m_spanAngle = 0;
    m_aniGroup->start();
}

void IndeterminateProgressRing::stop()
{
    m_aniGroup->stop();
    m_startAngle = 0;
    m_spanAngle = 0;
    update();
}

QColor IndeterminateProgressRing::lightBarColor() const
{
    if (m_lightBarColor.isValid()) {
        return m_lightBarColor;
    }
    return Theme::instance()->themeColor(Fluent::ThemeColor::LIGHT_1);
}

QColor IndeterminateProgressRing::darkBarColor() const
{
    if (m_darkBarColor.isValid()) {
        return m_darkBarColor;
    }
    return Theme::instance()->themeColor(Fluent::ThemeColor::DARK_1);
}

void IndeterminateProgressRing::setCustomBarColor(const QColor &light, const QColor &dark)
{
    m_lightBarColor = light;
    m_darkBarColor = dark;
    update();
}

void IndeterminateProgressRing::setCustomBackgroundColor(const QColor &light, const QColor &dark)
{
    m_lightBackgroundColor = light;
    m_darkBackgroundColor = dark;
    update();
}

void IndeterminateProgressRing::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    int cw = m_strokeWidth;
    int size = qMin(height(), width()) - cw;
    QRectF rc(cw/2.0, height()/2.0 - size/2.0, size, size);

    QColor bc = Theme::instance()->isDarkTheme() ? m_darkBackgroundColor : m_lightBackgroundColor;

    QPen pen(bc, cw, Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.drawArc(rc, 0, 360 * 16);

    QColor barColor = Theme::instance()->isDarkTheme() ? darkBarColor() : lightBarColor();

    pen.setColor(barColor);
    painter.setPen(pen);

    int startAngle = -m_startAngle + 180;
    painter.drawArc(rc, (startAngle % 360) * 16, -m_spanAngle * 16);
}
