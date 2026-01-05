#include "ScrollBarPrivate.h"
#include "QFluent/ScrollBar.h"

#include <QAbstractScrollArea>
#include <QApplication>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QStyle>
#include <QStyleOption>
#include <QTimer>
#include <QtMath>

ScrollBarPrivate::ScrollBarPrivate(ScrollBar* parent)
    : QObject(parent)
    , q_ptr(parent)
{
}

ScrollBarPrivate::~ScrollBarPrivate()
{
    // Qt父子关系会自动清理子对象
}

void ScrollBarPrivate::setTargetMaximum(int maximum)
{
    if (m_targetMaximum != maximum)
    {
        m_targetMaximum = maximum;
        Q_EMIT targetMaximumChanged();
    }
}

int ScrollBarPrivate::targetMaximum() const
{
    return m_targetMaximum;
}

void ScrollBarPrivate::onRangeChanged(int min, int max)
{
    Q_Q(ScrollBar);

    if (q->isVisible() && m_isAnimationEnabled && max != 0)
    {
        QPropertyAnimation* rangeSmoothAnimation = new QPropertyAnimation(this, "targetMaximum", this);
        rangeSmoothAnimation->setEasingCurve(QEasingCurve::OutSine);
        rangeSmoothAnimation->setDuration(250);
        rangeSmoothAnimation->setStartValue(m_targetMaximum);
        rangeSmoothAnimation->setEndValue(max);

        connect(rangeSmoothAnimation, &QPropertyAnimation::valueChanged, q, [q](const QVariant& value) {
            q->blockSignals(true);
            q->setMaximum(value.toInt());
            q->blockSignals(false);
            q->update();
        });

        rangeSmoothAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else
    {
        if (max == 0)
        {
            m_scrollValue = -1;
        }
        m_targetMaximum = max;
    }
}

void ScrollBarPrivate::scroll(Qt::KeyboardModifiers modifiers, int delta)
{
    Q_Q(ScrollBar);

    int stepsToScroll = 0;
    qreal offset = qreal(delta) / 120.0;
    int pageStep = 10;
    int singleStep = q->singleStep();

    if ((modifiers & Qt::ControlModifier) || (modifiers & Qt::ShiftModifier))
    {
        stepsToScroll = qBound(-pageStep, int(offset * pageStep), pageStep);
    }
    else
    {
        stepsToScroll = QApplication::wheelScrollLines() * offset * singleStep;
    }

    // 检查滚动值是否偏离太多
    if (qAbs(m_scrollValue - q->value()) > qAbs(stepsToScroll * m_speedLimit))
    {
        m_scrollValue = q->value();
    }

    m_scrollValue -= stepsToScroll;

    m_slideSmoothAnimation->stop();
    m_slideSmoothAnimation->setStartValue(q->value());
    m_slideSmoothAnimation->setEndValue(m_scrollValue);
    m_slideSmoothAnimation->start();
}

int ScrollBarPrivate::pixelPosToRangeValue(int pos) const
{
    Q_Q(const ScrollBar);

    QStyleOptionSlider opt;
    q->initStyleOption(&opt);

    QRect grooveRect = q->style()->subControlRect(QStyle::CC_ScrollBar, &opt,
                                                   QStyle::SC_ScrollBarGroove, q);
    QRect sliderRect = q->style()->subControlRect(QStyle::CC_ScrollBar, &opt,
                                                   QStyle::SC_ScrollBarSlider, q);

    int sliderMin, sliderMax, sliderLength;

    if (q->orientation() == Qt::Horizontal)
    {
        sliderLength = sliderRect.width();
        sliderMin = grooveRect.x();
        sliderMax = grooveRect.right() - sliderLength + 1;

        if (q->layoutDirection() == Qt::RightToLeft)
        {
            opt.upsideDown = !opt.upsideDown;
        }
    }
    else
    {
        sliderLength = sliderRect.height();
        sliderMin = grooveRect.y();
        sliderMax = grooveRect.bottom() - sliderLength + 1;
    }

    return QStyle::sliderValueFromPosition(q->minimum(), q->maximum(),
                                          pos - sliderMin,
                                          sliderMax - sliderMin,
                                          opt.upsideDown);
}

void ScrollBarPrivate::initAllConfig()
{
    Q_Q(ScrollBar);

    if (!m_originScrollBar)
    {
        return;
    }

    handleScrollBarRangeChanged(m_originScrollBar->minimum(), m_originScrollBar->maximum());
    q->setSingleStep(m_originScrollBar->singleStep());
    q->setPageStep(m_originScrollBar->pageStep());
}

void ScrollBarPrivate::handleScrollBarValueChanged(QScrollBar* scrollBar, int value)
{
    if (scrollBar && scrollBar->value() != value)
    {
        scrollBar->setValue(value);
    }
}

void ScrollBarPrivate::handleScrollBarRangeChanged(int min, int max)
{
    Q_Q(ScrollBar);

    q->setRange(min, max);
    q->setVisible(max > 0);
}

void ScrollBarPrivate::handleScrollBarGeometry()
{
    Q_Q(ScrollBar);

    if (!m_originScrollArea)
    {
        return;
    }

    q->raise();

    if (m_originScrollBar)
    {
        q->setSingleStep(m_originScrollBar->singleStep());
        q->setPageStep(m_originScrollBar->pageStep());
    }

    const int scrollBarWidth = 10;

    if (q->orientation() == Qt::Horizontal)
    {
        q->setGeometry(0,
                      m_originScrollArea->height() - scrollBarWidth,
                      m_originScrollArea->width(),
                      scrollBarWidth);
    }
    else
    {
        q->setGeometry(m_originScrollArea->width() - scrollBarWidth,
                      0,
                      scrollBarWidth,
                      m_originScrollArea->height());
    }
}
