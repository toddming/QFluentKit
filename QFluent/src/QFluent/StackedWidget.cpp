#include "StackedWidget.h"
#include <QAbstractAnimation>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QAbstractScrollArea>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

// ==================== OpacityAniStackedWidget ====================

OpacityAniStackedWidget::OpacityAniStackedWidget(QWidget *parent)
    : QStackedWidget(parent), __nextIndex(0)
{
}

void OpacityAniStackedWidget::addWidget(QWidget *w)
{
    QStackedWidget::addWidget(w);

    auto effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);
    auto ani = new QPropertyAnimation(effect, "opacity", this);
    ani->setDuration(220);
    connect(ani, &QPropertyAnimation::finished, this, &OpacityAniStackedWidget::__onAniFinished);

    __effects.append(effect);
    __anis.append(ani);
    w->setGraphicsEffect(effect);
}

void OpacityAniStackedWidget::setCurrentIndex(int index)
{
    int currentIndex = this->currentIndex();
    if (index == currentIndex) return;

    QPropertyAnimation *ani;
    if (index > currentIndex) {
        ani = __anis[index];
        ani->setStartValue(0.0);
        ani->setEndValue(1.0);
        QStackedWidget::setCurrentIndex(index); // 先切换，再动画显示
    } else {
        ani = __anis[currentIndex];
        ani->setStartValue(1.0);
        ani->setEndValue(0.0);
    }

    widget(currentIndex)->show(); // 确保当前页可见以执行淡出
    __nextIndex = index;
    ani->start();
}

void OpacityAniStackedWidget::setCurrentWidget(QWidget *w)
{
    setCurrentIndex(indexOf(w));
}

void OpacityAniStackedWidget::__onAniFinished()
{
    QStackedWidget::setCurrentIndex(__nextIndex);
}


// ==================== PopUpAniStackedWidget ====================

PopUpAniStackedWidget::PopUpAniStackedWidget(QWidget *parent)
    : QStackedWidget(parent), m_isAnimationEnabled(true), m_nextIndex(-1), m_ani(nullptr)
{
}

void PopUpAniStackedWidget::addWidget(QWidget *widget, int deltaX, int deltaY)
{
    QStackedWidget::addWidget(widget);

    auto ani = new QPropertyAnimation(widget, "pos", this);
    m_aniInfos.append(PopUpAniInfo(widget, deltaX, deltaY, ani));
}

void PopUpAniStackedWidget::removeWidget(QWidget *widget)
{
    int index = indexOf(widget);
    if (index == -1) return;

    m_aniInfos.removeAt(index);
    QStackedWidget::removeWidget(widget);
}

void PopUpAniStackedWidget::setAnimationEnabled(bool isEnabled)
{
    m_isAnimationEnabled = isEnabled;
}

void PopUpAniStackedWidget::setCurrentIndex(int index, bool needPopOut, bool showNextWidgetDirectly,
                                            int duration, QEasingCurve easingCurve)
{
    if (index < 0 || index >= count()) return;
    if (index == currentIndex()) return;
    if (!m_isAnimationEnabled) {
        QStackedWidget::setCurrentIndex(index);
        return;
    }

    if (m_ani && m_ani->state() == QAbstractAnimation::Running) {
        m_ani->stop();
        __onAniFinished();
    }

    m_nextIndex = index;

    PopUpAniInfo &nextAniInfo = m_aniInfos[index];
    PopUpAniInfo &currentAniInfo = m_aniInfos[currentIndex()];

    QWidget *currentWidget = this->currentWidget();
    QWidget *nextWidget = nextAniInfo.widget;
    m_ani = needPopOut ? currentAniInfo.ani : nextAniInfo.ani;

    if (needPopOut) {
        QPoint startPos = currentWidget->pos();
        QPoint endPos = startPos + QPoint(currentAniInfo.deltaX, currentAniInfo.deltaY);
        __setAnimation(m_ani, startPos, endPos, duration, easingCurve);
        nextWidget->setVisible(showNextWidgetDirectly);
    } else {
        QPoint startPos = nextWidget->pos() + QPoint(nextAniInfo.deltaX, nextAniInfo.deltaY);
        QPoint endPos(nextWidget->x(), 0);
        __setAnimation(m_ani, startPos, endPos, duration, easingCurve);
        QStackedWidget::setCurrentIndex(index); // 先切换再动画
    }

    connect(m_ani, &QPropertyAnimation::finished, this, &PopUpAniStackedWidget::__onAniFinished);
    m_ani->start();
    emit aniStart();
}

void PopUpAniStackedWidget::setCurrentWidget(QWidget *widget, bool needPopOut, bool showNextWidgetDirectly,
                                             int duration, QEasingCurve easingCurve)
{
    int index = indexOf(widget);
    if (index != -1) {
        setCurrentIndex(index, needPopOut, showNextWidgetDirectly, duration, easingCurve);
    }
}

void PopUpAniStackedWidget::__setAnimation(QPropertyAnimation *ani, const QPoint &startValue,
                                           const QPoint &endValue, int duration, QEasingCurve easingCurve)
{
    ani->setEasingCurve(easingCurve);
    ani->setStartValue(startValue);
    ani->setEndValue(endValue);
    ani->setDuration(duration);
}

void PopUpAniStackedWidget::__onAniFinished()
{
    disconnect(m_ani, &QPropertyAnimation::finished, this, &PopUpAniStackedWidget::__onAniFinished);
    QStackedWidget::setCurrentIndex(m_nextIndex);
    emit aniFinished();
}


// ==================== StackedWidget ====================

StackedWidget::StackedWidget(QWidget *parent)
    : QFrame(parent), view(new PopUpAniStackedWidget(this))
{
    hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->addWidget(view);

    connect(view, &PopUpAniStackedWidget::currentChanged, this, &StackedWidget::currentChanged);
    setAttribute(Qt::WA_StyledBackground);
}

bool StackedWidget::isAnimationEnabled() const
{
    return view->isAnimationEnabled();
}

void StackedWidget::setAnimationEnabled(bool isEnabled)
{
    view->setAnimationEnabled(isEnabled);
}

void StackedWidget::addWidget(QWidget *widget)
{
    view->addWidget(widget);
}

void StackedWidget::removeWidget(QWidget *widget)
{
    view->removeWidget(widget);
}

QWidget* StackedWidget::widget(int index) const
{
    return view->widget(index);
}

void StackedWidget::setCurrentWidget(QWidget *widget, bool popOut)
{
    // 如果是滚动区域，自动滚到顶部
    if (auto sa = qobject_cast<QAbstractScrollArea*>(widget)) {
        if (QScrollBar* vbar = sa->verticalScrollBar()) {
            vbar->setValue(0);
        }
        if (QScrollBar* hbar = sa->horizontalScrollBar()) {
            hbar->setValue(0);
        }
    }

    if (!popOut) {
        view->setCurrentWidget(widget, false, true, 300);
    } else {
        view->setCurrentWidget(widget, true, false, 200, QEasingCurve::InQuad);
    }
}

void StackedWidget::setCurrentIndex(int index, bool popOut)
{
    setCurrentWidget(view->widget(index), popOut);
}

int StackedWidget::currentIndex() const
{
    return view->currentIndex();
}

QWidget* StackedWidget::currentWidget() const
{
    return view->currentWidget();
}

int StackedWidget::indexOf(QWidget *widget) const
{
    return view->indexOf(widget);
}

int StackedWidget::count() const
{
    return view->count();
}
