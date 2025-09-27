#include "ScrollBar.h"

#include <QDebug>
#include <QPainter>
#include <QPointer>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWheelEvent>

#include "ScrollBarStyle.h"
#include "ScrollBarPrivate.h"
Q_PROPERTY_CREATE_Q_CPP(ScrollBar, bool, IsAnimation)
Q_PROPERTY_CREATE_Q_CPP(ScrollBar, qreal, SpeedLimit)
ScrollBar::ScrollBar(QWidget* parent)
    : QScrollBar(parent), d_ptr(new ScrollBarPrivate())
{
    Q_D(ScrollBar);
    d->q_ptr = this;
    setSingleStep(1);
    setObjectName("ScrollBar");
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    d->_pSpeedLimit = 20;
    d->_pTargetMaximum = 0;
    d->_pIsAnimation = false;
    connect(this, &ScrollBar::rangeChanged, d, &ScrollBarPrivate::onRangeChanged);
    ScrollBarStyle* scrollBarStyle = new ScrollBarStyle(style());
    scrollBarStyle->setScrollBar(this);
    setStyle(scrollBarStyle);
    d->_slideSmoothAnimation = new QPropertyAnimation(this, "value");
    d->_slideSmoothAnimation->setEasingCurve(QEasingCurve::OutSine);
    d->_slideSmoothAnimation->setDuration(300);
    connect(d->_slideSmoothAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->_scrollValue = value();
    });

    d->_expandTimer = new QTimer(this);
    connect(d->_expandTimer, &QTimer::timeout, this, [=]() {
        d->_expandTimer->stop();
        d->_isExpand = underMouse();
        scrollBarStyle->startExpandAnimation(d->_isExpand);
    });
}

ScrollBar::ScrollBar(Qt::Orientation orientation, QWidget* parent)
    : ScrollBar(parent)
{
    setOrientation(orientation);
}

ScrollBar::ScrollBar(QScrollBar* originScrollBar, QAbstractScrollArea* parent)
    : ScrollBar(parent)
{
    Q_D(ScrollBar);
    if (!originScrollBar || !parent) {
        qCritical() << "Invalid origin or parent!";
        return;
    }
    d->_originScrollArea = parent;
    Qt::Orientation orientation = originScrollBar->orientation();
    setOrientation(orientation);
    orientation == Qt::Horizontal ? parent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff) : parent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    parent->installEventFilter(this);

    d->_originScrollBar = originScrollBar;
    d->_initAllConfig();

    connect(d->_originScrollBar, &QScrollBar::valueChanged, this, [=](int value) {
        d->_handleScrollBarValueChanged(this, value);
    });
    connect(this, &QScrollBar::valueChanged, this, [=](int value) {
        d->_handleScrollBarValueChanged(d->_originScrollBar, value);
    });
    connect(d->_originScrollBar, &QScrollBar::rangeChanged, this, [=](int min, int max) {
        d->_handleScrollBarRangeChanged(min, max);
    });
}

ScrollBar::~ScrollBar()
{
    delete this->style();
}

bool ScrollBar::event(QEvent* event)
{
    Q_D(ScrollBar);
    switch (event->type())
    {
    case QEvent::Enter:
    {
        d->_expandTimer->stop();
        if (!d->_isExpand)
        {
            d->_expandTimer->start(350);
        }
        break;
    }
    case QEvent::Leave:
    {
        d->_expandTimer->stop();
        if (d->_isExpand)
        {
            d->_expandTimer->start(350);
        }
        break;
    }
    default:
    {
        break;
    }
    }
    return QScrollBar::event(event);
}

bool ScrollBar::eventFilter(QObject* watched, QEvent* event)
{
    Q_D(ScrollBar);
    switch (event->type())
    {
    case QEvent::Show:
    case QEvent::Resize:
    case QEvent::LayoutRequest:
    {
        d->_handleScrollBarGeometry();
        break;
    }
    default:
    {
        break;
    }
    }
    return QScrollBar::eventFilter(watched, event);
}

void ScrollBar::mousePressEvent(QMouseEvent* event)
{
    Q_D(ScrollBar);
    d->_slideSmoothAnimation->stop();
    QScrollBar::mousePressEvent(event);
    d->_scrollValue = value();
}

void ScrollBar::mouseReleaseEvent(QMouseEvent* event)
{
    Q_D(ScrollBar);
    d->_slideSmoothAnimation->stop();
    QScrollBar::mouseReleaseEvent(event);
    d->_scrollValue = value();
}

void ScrollBar::mouseMoveEvent(QMouseEvent* event)
{
    Q_D(ScrollBar);
    d->_slideSmoothAnimation->stop();
    QScrollBar::mouseMoveEvent(event);
    d->_scrollValue = value();
}

void ScrollBar::wheelEvent(QWheelEvent* event)
{
    Q_D(ScrollBar);
    int verticalDelta = event->angleDelta().y();
    if (d->_slideSmoothAnimation->state() == QAbstractAnimation::Stopped) {
        d->_scrollValue = value();
    }
    if (verticalDelta != 0) {
        if ((value() == minimum() && verticalDelta > 0) || (value() == maximum() && verticalDelta < 0))
        {
            QScrollBar::wheelEvent(event);
            return;
        }
        d->_scroll(event->modifiers(), verticalDelta);
    } else {
        int horizontalDelta = event->angleDelta().x();
        if ((value() == minimum() && horizontalDelta > 0) || (value() == maximum() && horizontalDelta < 0))
        {
            QScrollBar::wheelEvent(event);
            return;
        }
        d->_scroll(event->modifiers(), horizontalDelta);
    }
    event->accept();
}

