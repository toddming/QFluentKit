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
    lightBackgroundColor(QColor(0, 0, 0, 0)),
    darkBackgroundColor(QColor(255, 255, 255, 0)),
    m_strokeWidth(6),
    m_startAngle(-180),
    m_spanAngle(0),
    startAngleAni1(new QPropertyAnimation(this, "startAngle")),
    startAngleAni2(new QPropertyAnimation(this, "startAngle")),
    spanAngleAni1(new QPropertyAnimation(this, "spanAngle")),
    spanAngleAni2(new QPropertyAnimation(this, "spanAngle")),
    startAngleAniGroup(new QSequentialAnimationGroup(this)),
    spanAngleAniGroup(new QSequentialAnimationGroup(this)),
    aniGroup(new QParallelAnimationGroup(this))
{
    // Initialize start angle animation
    startAngleAni1->setDuration(1000);
    startAngleAni1->setStartValue(0);
    startAngleAni1->setEndValue(450);

    startAngleAni2->setDuration(1000);
    startAngleAni2->setStartValue(450);
    startAngleAni2->setEndValue(1080);

    startAngleAniGroup->addAnimation(startAngleAni1);
    startAngleAniGroup->addAnimation(startAngleAni2);

    // Initialize span angle animation
    spanAngleAni1->setDuration(1000);
    spanAngleAni1->setStartValue(0);
    spanAngleAni1->setEndValue(180);

    spanAngleAni2->setDuration(1000);
    spanAngleAni2->setStartValue(180);
    spanAngleAni2->setEndValue(0);

    spanAngleAniGroup->addAnimation(spanAngleAni1);
    spanAngleAniGroup->addAnimation(spanAngleAni2);

    aniGroup->addAnimation(startAngleAniGroup);
    aniGroup->addAnimation(spanAngleAniGroup);
    aniGroup->setLoopCount(-1);

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
    aniGroup->start();
}

void IndeterminateProgressRing::stop()
{
    aniGroup->stop();
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
    lightBackgroundColor = light;
    darkBackgroundColor = dark;
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

    QColor bc = Theme::instance()->isDarkTheme() ? darkBackgroundColor : lightBackgroundColor;

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
