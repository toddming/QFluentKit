#include "ScrollBarPrivate.h"

#include <QApplication>
#include <QPropertyAnimation>
#include <QStyleOption>
#include <QPointer>

#include "ScrollBar.h"
ScrollBarPrivate::ScrollBarPrivate(QObject* parent)
    : QObject{parent}
{
}

ScrollBarPrivate::~ScrollBarPrivate()
{
}

void ScrollBarPrivate::onRangeChanged(int min, int max)
{
    Q_UNUSED(min);
    Q_Q(ScrollBar);
    if (q->isVisible() && _pIsAnimation && max != 0) {
        QPointer<ScrollBarPrivate> self = this;
        QPropertyAnimation* rangeSmoothAnimation = new QPropertyAnimation(this, "pTargetMaximum");
        connect(rangeSmoothAnimation, &QPropertyAnimation::finished, this, [=]() {
            if (!self.isNull())
                Q_EMIT q->rangeAnimationFinished();
        });
        connect(rangeSmoothAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
            if (!self.isNull()) {
                q->blockSignals(true);
                q->setMaximum(value.toUInt());
                q->blockSignals(false);
                q->update();
            }
        });
        rangeSmoothAnimation->setEasingCurve(QEasingCurve::OutSine);
        rangeSmoothAnimation->setDuration(250);
        rangeSmoothAnimation->setStartValue(_pTargetMaximum);
        rangeSmoothAnimation->setEndValue(max);
        rangeSmoothAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        if (max == 0) {
            _scrollValue = -1;
        }
        _pTargetMaximum = max;
    }
}

void ScrollBarPrivate::_scroll(Qt::KeyboardModifiers modifiers, int delta)
{
    Q_Q(ScrollBar);
    int stepsToScroll = 0;
    qreal offset = qreal(delta) / 120;
    int pageStep = 10;
    int singleStep = q->singleStep();
    if ((modifiers & Qt::ControlModifier) || (modifiers & Qt::ShiftModifier)) {
        stepsToScroll = qBound(-pageStep, int(offset * pageStep), pageStep);
    } else {
        stepsToScroll = QApplication::wheelScrollLines() * offset * singleStep;
    }
    if (abs(_scrollValue - q->value()) > abs(stepsToScroll * _pSpeedLimit)) {
        _scrollValue = q->value();
    }
    _scrollValue -= stepsToScroll;
    _slideSmoothAnimation->stop();
    _slideSmoothAnimation->setStartValue(q->value());
    _slideSmoothAnimation->setEndValue(_scrollValue);
    _slideSmoothAnimation->start();
}

int ScrollBarPrivate::_pixelPosToRangeValue(int pos) const
{
    Q_Q(const ScrollBar);
    QStyleOptionSlider opt;
    q->initStyleOption(&opt);
    QRect gr = q->style()->subControlRect(QStyle::CC_ScrollBar, &opt,
                                          QStyle::SC_ScrollBarGroove, q);
    QRect sr = q->style()->subControlRect(QStyle::CC_ScrollBar, &opt,
                                          QStyle::SC_ScrollBarSlider, q);
    int sliderMin, sliderMax, sliderLength;
    if (q->orientation() == Qt::Horizontal) {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
        if (q->layoutDirection() == Qt::RightToLeft) {
            opt.upsideDown = !opt.upsideDown;
        }
    } else {
        sliderLength = sr.height();
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }
    return QStyle::sliderValueFromPosition(q->minimum(), q->maximum(), pos - sliderMin,
                                           sliderMax - sliderMin, opt.upsideDown);
}

void ScrollBarPrivate::_initAllConfig()
{
    Q_Q(ScrollBar);
    _handleScrollBarRangeChanged(_originScrollBar->minimum(), _originScrollBar->maximum());
    q->setSingleStep(_originScrollBar->singleStep());
    q->setPageStep(_originScrollBar->pageStep());
}

void ScrollBarPrivate::_handleScrollBarValueChanged(QScrollBar* scrollBar, int value)
{
    scrollBar->setValue(value);
}

void ScrollBarPrivate::_handleScrollBarRangeChanged(int min, int max)
{
    Q_Q(ScrollBar);
    q->setRange(min, max);
    if (max <= 0) {
        q->setVisible(false);
    } else {
        q->setVisible(true);
    }
}

void ScrollBarPrivate::_handleScrollBarGeometry()
{
    Q_Q(ScrollBar);
    q->raise();
    q->setSingleStep(_originScrollBar->singleStep());
    q->setPageStep(_originScrollBar->pageStep());
    if (q->orientation() == Qt::Horizontal) {
        q->setGeometry(0, _originScrollArea->height() - 10, _originScrollArea->width(), 10);
    } else {
        q->setGeometry(_originScrollArea->width() - 10, 0, 10, _originScrollArea->height());
    }
}
