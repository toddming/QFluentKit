#include "StackedWidget.h"
#include <QAbstractAnimation>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QAbstractScrollArea>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPauseAnimation>
#include <QTimer>
#include <QCoreApplication>

// 辅助函数：创建自定义贝塞尔曲线
static QEasingCurve createBezierCurve(qreal c1x, qreal c1y, qreal c2x, qreal c2y)
{
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(c1x, c1y), QPointF(c2x, c2y), QPointF(1.0, 1.0));
    return curve;
}


// ==================== OpacityAniStackedWidget ====================

OpacityAniStackedWidget::OpacityAniStackedWidget(QWidget *parent)
    : QStackedWidget(parent)
    , m_nextIndex(0)
{
}

void OpacityAniStackedWidget::addWidget(QWidget *widget)
{
    if (!widget) {
        return;
    }

    QStackedWidget::addWidget(widget);

    // 创建透明度效果
    auto effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);
    widget->setGraphicsEffect(effect);

    // 创建动画
    auto animation = new QPropertyAnimation(effect, "opacity", this);
    animation->setDuration(220);
    connect(animation, &QPropertyAnimation::finished,
            this, &OpacityAniStackedWidget::onAnimationFinished);

    m_effects.append(effect);
    m_animations.append(animation);
}

void OpacityAniStackedWidget::setCurrentIndex(int index)
{
    int currentIdx = currentIndex();
    if (index == currentIdx || index < 0 || index >= count()) {
        return;
    }

    m_nextIndex = index;
    QPropertyAnimation *animation = nullptr;

    if (index > currentIdx) {
        // 前进导航：淡入下一个控件
        animation = m_animations[index];
        animation->setStartValue(0.0);
        animation->setEndValue(1.0);

        // 设置目标控件透明度为0,然后立即切换
        m_effects[index]->setOpacity(0.0);
        QStackedWidget::setCurrentIndex(index);
    } else {
        // 后退导航：淡出当前控件
        animation = m_animations[currentIdx];
        animation->setStartValue(1.0);
        animation->setEndValue(0.0);

        // 保持当前控件可见以执行淡出动画
        if (QWidget *w = widget(currentIdx)) {
            w->show();
        }
    }

    animation->start();
}

void OpacityAniStackedWidget::setCurrentWidget(QWidget *widget)
{
    if (widget) {
        setCurrentIndex(indexOf(widget));
    }
}

void OpacityAniStackedWidget::onAnimationFinished()
{
    // 如果是后退导航,现在切换到目标索引
    if (m_nextIndex < currentIndex()) {
        // 恢复目标控件的透明度
        if (m_nextIndex >= 0 && m_nextIndex < m_effects.size()) {
            m_effects[m_nextIndex]->setOpacity(1.0);
        }
        QStackedWidget::setCurrentIndex(m_nextIndex);
    }
}


// ==================== PopUpAniStackedWidget ====================

PopUpAniStackedWidget::PopUpAniStackedWidget(QWidget *parent)
    : QStackedWidget(parent)
    , m_isAnimationEnabled(true)
    , m_nextIndex(-1)
    , m_currentAnimation(nullptr)
{
}

void PopUpAniStackedWidget::addWidget(QWidget *widget, int deltaX, int deltaY)
{
    if (!widget) {
        return;
    }

    QStackedWidget::addWidget(widget);

    // 延迟创建动画,避免在添加时造成开销
    auto animation = new QPropertyAnimation(widget, "pos", this);
    m_animationInfos.append(PopUpAniInfo(widget, deltaX, deltaY, animation));
}

void PopUpAniStackedWidget::removeWidget(QWidget *widget)
{
    int index = indexOf(widget);
    if (index == -1) {
        return;
    }

    // 安全删除动画对象（先停止，再延迟删除避免与 Qt parent 管理冲突）
    if (index >= 0 && index < m_animationInfos.size()) {
        m_animationInfos[index].animation->stop();
        m_animationInfos[index].animation->deleteLater();
        m_animationInfos.removeAt(index);
    }

    QStackedWidget::removeWidget(widget);
}

void PopUpAniStackedWidget::setAnimationEnabled(bool isEnabled)
{
    m_isAnimationEnabled = isEnabled;
}

void PopUpAniStackedWidget::setCurrentIndex(int index, bool needPopOut,
                                            bool showNextWidgetDirectly,
                                            int duration, QEasingCurve easingCurve)
{
    if (index < 0 || index >= count()) {
        return;
    }

    if (index == currentIndex()) {
        return;
    }

    if (!m_isAnimationEnabled) {
        QStackedWidget::setCurrentIndex(index);
        return;
    }

    // 停止正在运行的动画
    if (m_currentAnimation && m_currentAnimation->state() == QAbstractAnimation::Running) {
        // 断开旧的信号连接
        disconnect(m_currentAnimation, &QPropertyAnimation::finished,
                   this, &PopUpAniStackedWidget::onAnimationFinished);
        m_currentAnimation->stop();
        // 立即完成切换
        QStackedWidget::setCurrentIndex(m_nextIndex);
    }

    m_nextIndex = index;

    PopUpAniInfo &nextAniInfo = m_animationInfos[index];
    PopUpAniInfo &currentAniInfo = m_animationInfos[currentIndex()];

    QWidget *currentWidget = this->currentWidget();
    QWidget *nextWidget = nextAniInfo.widget;

    if (!currentWidget || !nextWidget) {
        QStackedWidget::setCurrentIndex(index);
        return;
    }

    m_currentAnimation = needPopOut ? currentAniInfo.animation : nextAniInfo.animation;

    if (needPopOut) {
        // 弹出当前控件
        QPoint startPos = currentWidget->pos();
        QPoint endPos = startPos + QPoint(currentAniInfo.deltaX, currentAniInfo.deltaY);
        setAnimation(m_currentAnimation, startPos, endPos, duration, easingCurve);
        nextWidget->setVisible(showNextWidgetDirectly);
    } else {
        // 弹入下一个控件
        QPoint startPos = nextWidget->pos() + QPoint(nextAniInfo.deltaX, nextAniInfo.deltaY);
        QPoint endPos(nextWidget->x(), 0);
        setAnimation(m_currentAnimation, startPos, endPos, duration, easingCurve);
        QStackedWidget::setCurrentIndex(index); // 先切换,再执行动画
    }

    // 使用 UniqueConnection 避免重复连接
    connect(m_currentAnimation, &QPropertyAnimation::finished,
            this, &PopUpAniStackedWidget::onAnimationFinished,
            Qt::UniqueConnection);

    emit aniStart();
    m_currentAnimation->start();
}

void PopUpAniStackedWidget::setCurrentWidget(QWidget *widget, bool needPopOut,
                                             bool showNextWidgetDirectly,
                                             int duration, QEasingCurve easingCurve)
{
    int index = indexOf(widget);
    if (index != -1) {
        setCurrentIndex(index, needPopOut, showNextWidgetDirectly, duration, easingCurve);
    }
}

void PopUpAniStackedWidget::setAnimation(QPropertyAnimation *animation,
                                         const QPoint &startValue,
                                         const QPoint &endValue,
                                         int duration,
                                         QEasingCurve easingCurve)
{
    if (!animation) {
        return;
    }

    animation->setEasingCurve(easingCurve);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->setDuration(duration);
}

void PopUpAniStackedWidget::onAnimationFinished()
{
    if (m_currentAnimation) {
        disconnect(m_currentAnimation, &QPropertyAnimation::finished,
                   this, &PopUpAniStackedWidget::onAnimationFinished);
    }

    QStackedWidget::setCurrentIndex(m_nextIndex);
    emit aniFinished();
}


// ==================== TransitionStackedWidget ====================

TransitionStackedWidget::TransitionStackedWidget(QWidget *parent)
    : QStackedWidget(parent)
    , m_animationGroup(new QParallelAnimationGroup(this))
    , m_currentSnapshot(nullptr)
    , m_nextSnapshot(nullptr)
    , m_nextIndex(-1)
    , m_isAnimationEnabled(true)
{
    // 延迟创建快照标签,只在首次需要时创建
    // m_currentSnapshot 和 m_nextSnapshot 将在 createSnapshotLabel() 中按需创建

    connect(m_animationGroup, &QParallelAnimationGroup::finished,
            this, &TransitionStackedWidget::onAnimationFinished);
}

void TransitionStackedWidget::setAnimationEnabled(bool isEnabled)
{
    m_isAnimationEnabled = isEnabled;
}

void TransitionStackedWidget::setCurrentWidget(QWidget *widget, int duration, bool isBack)
{
    setCurrentIndex(indexOf(widget), duration, isBack);
}

void TransitionStackedWidget::setCurrentIndex(int index, int duration, bool isBack)
{
    if (index < 0 || index >= count()) {
        return;
    }

    if (index == currentIndex()) {
        return;
    }

    if (!m_isAnimationEnabled) {
        QStackedWidget::setCurrentIndex(index);
        return;
    }

    stopAnimation();

    m_nextIndex = index;

    // 延迟创建快照标签
    if (!m_currentSnapshot) {
        m_currentSnapshot = createSnapshotLabel();
    }
    if (!m_nextSnapshot) {
        m_nextSnapshot = createSnapshotLabel();
    }

    // 设置过渡动画（由子类实现）
    setUpTransitionAnimation(index, duration, isBack);

    // 开始过渡动画
    m_animationGroup->start();
    emit aniStart();
}

void TransitionStackedWidget::stopAnimation()
{
    if (m_animationGroup->state() != QAbstractAnimation::Running) {
        return;
    }

    m_animationGroup->stop();
    onAnimationFinished();
}

void TransitionStackedWidget::hideSnapshots()
{
    if (m_currentSnapshot) {
        m_currentSnapshot->hide();
    }
    if (m_nextSnapshot) {
        m_nextSnapshot->hide();
    }
}

void TransitionStackedWidget::onAnimationFinished()
{
    hideSnapshots();
    m_animationGroup->clear();
    QStackedWidget::setCurrentIndex(m_nextIndex);
    emit aniFinished();
}

QLabel* TransitionStackedWidget::createSnapshotLabel()
{
    auto label = new QLabel(this);
    label->setAttribute(Qt::WA_TranslucentBackground);

    auto effect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(effect);
    label->hide();

    return label;
}

void TransitionStackedWidget::renderSnapshot(QWidget *widget, QLabel *label)
{
    if (!widget || !label) {
        return;
    }

    // 确保控件已完成样式计算和布局
    widget->ensurePolished();

    // 只在尺寸不匹配时才调整
    if (widget->size() != size()) {
        widget->resize(size());
        // 处理待处理的布局事件,但不处理用户输入
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    // 使用 grab() 捕获控件内容
    QPixmap pixmap = widget->grab();

    // 如果捕获失败,使用备用方案
    if (pixmap.isNull() || pixmap.size().isEmpty()) {
        pixmap = QPixmap(widget->size());
        pixmap.fill(Qt::transparent);

        // 使用 QPainter 进行渲染
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing, false); // 禁用抗锯齿提高速度
        widget->render(&painter, QPoint(), QRegion(),
                       QWidget::DrawWindowBackground | QWidget::DrawChildren);
        painter.end();
    }

    label->setPixmap(pixmap);
    label->setGeometry(rect());
    label->show();
    label->raise();
}


// ==================== EntranceTransitionStackedWidget ====================

EntranceTransitionStackedWidget::EntranceTransitionStackedWidget(QWidget *parent)
    : TransitionStackedWidget(parent)
{
}

void EntranceTransitionStackedWidget::setUpTransitionAnimation(int nextIndex,
                                                               int duration,
                                                               bool isBack)
{
    const int offset = 140;
    const int outDuration = 150;
    const int inDuration = (duration > 0) ? duration : 300;

    QEasingCurve inCurve = createBezierCurve(0.1, 0.9, 0.2, 1.0);
    QEasingCurve outCurve = createBezierCurve(0.7, 0.0, 1.0, 0.5);

    QWidget *currentWidget = this->currentWidget();
    QWidget *nextWidget = widget(nextIndex);

    if (!nextWidget) {
        return;
    }

    // 淡出并滑出当前控件
    if (currentWidget) {
        renderSnapshot(currentWidget, m_currentSnapshot);
        currentWidget->hide();

        // 淡出动画
        auto fadeOutAni = new QPropertyAnimation(
                    m_currentSnapshot->graphicsEffect(), "opacity", this);
        fadeOutAni->setDuration(outDuration);
        fadeOutAni->setStartValue(1.0);
        fadeOutAni->setEndValue(0.0);
        fadeOutAni->setEasingCurve(outCurve);
        m_animationGroup->addAnimation(fadeOutAni);

        // 滑出动画（仅在后退时）
        if (isBack) {
            auto slideOutAni = new QPropertyAnimation(m_currentSnapshot, "pos", this);
            slideOutAni->setDuration(outDuration);
            slideOutAni->setStartValue(QPoint(0, 0));
            slideOutAni->setEndValue(QPoint(0, offset));
            slideOutAni->setEasingCurve(outCurve);
            m_animationGroup->addAnimation(slideOutAni);
        }
    }

    nextWidget->hide();

    // 在 outDuration 后显示下一个控件
    auto nextWidgetAniGroup = new QSequentialAnimationGroup(this);
    nextWidgetAniGroup->addPause(outDuration);

    m_animationGroup->addAnimation(nextWidgetAniGroup);

    if (!isBack) {
        // 确保控件尺寸正确
        nextWidget->resize(size());

        // 使用 move() 而不是 setGeometry(),避免与布局管理冲突
        nextWidget->move(0, offset);
        nextWidget->show(); // 提前显示以便动画

        auto slideInAni = new QPropertyAnimation(nextWidget, "pos", this);
        slideInAni->setDuration(inDuration);
        slideInAni->setStartValue(QPoint(0, offset));
        slideInAni->setEndValue(QPoint(0, 0));
        slideInAni->setEasingCurve(inCurve);
        nextWidgetAniGroup->addAnimation(slideInAni);
    } else {
        // 直接显示下一个控件
        nextWidget->resize(size());
        nextWidget->move(0, 0);

        // 在暂停后显示
        connect(nextWidgetAniGroup, &QSequentialAnimationGroup::currentAnimationChanged,
                this, [nextWidget](QAbstractAnimation*) {
            if (nextWidget && !nextWidget->isVisible()) {
                nextWidget->show();
            }
        }, Qt::UniqueConnection);
    }
}


// ==================== DrillInTransitionStackedWidget ====================

DrillInTransitionStackedWidget::DrillInTransitionStackedWidget(QWidget *parent)
    : TransitionStackedWidget(parent)
{
}

void DrillInTransitionStackedWidget::setUpTransitionAnimation(int nextIndex,
                                                              int duration,
                                                              bool isBack)
{
    QEasingCurve scaleCurve = createBezierCurve(0.1, 0.9, 0.2, 1.0);
    QEasingCurve opacityCurve = createBezierCurve(0.17, 0.17, 0.0, 1.0);
    QEasingCurve backScaleCurve = createBezierCurve(0.12, 0.0, 0.0, 1.0);

    qreal inScale, outScale;
    int inDuration, outDuration;
    QEasingCurve inScaleCurve;

    if (isBack) {
        inScale = 1.06;
        outScale = 0.96;
        inDuration = (duration > 0) ? duration : 333;
        outDuration = 100;
        inScaleCurve = backScaleCurve;
    } else {
        inScale = 0.94;
        outScale = 1.04;
        inDuration = (duration > 0) ? duration : 333;
        outDuration = 100;
        inScaleCurve = scaleCurve;
    }

    QWidget *currentWidget = this->currentWidget();
    QWidget *nextWidget = widget(nextIndex);
    QRect widgetRect = rect();

    if (!nextWidget) {
        return;
    }

    // 缩放并淡出当前控件
    if (currentWidget) {
        renderSnapshot(currentWidget, m_currentSnapshot);
        m_currentSnapshot->setScaledContents(true);
        currentWidget->hide();

        // 计算缩放后的矩形
        int outW = static_cast<int>(widgetRect.width() * outScale);
        int outH = static_cast<int>(widgetRect.height() * outScale);
        int outX = (widgetRect.width() - outW) / 2;
        int outY = (widgetRect.height() - outH) / 2;
        QRect outRect(outX, outY, outW, outH);

        // 缩放动画
        auto scaleOutAni = new QPropertyAnimation(m_currentSnapshot, "geometry", this);
        scaleOutAni->setDuration(outDuration);
        scaleOutAni->setStartValue(widgetRect);
        scaleOutAni->setEndValue(outRect);
        scaleOutAni->setEasingCurve(scaleCurve);
        m_animationGroup->addAnimation(scaleOutAni);

        // 淡出动画
        auto fadeOutAni = new QPropertyAnimation(
                    m_currentSnapshot->graphicsEffect(), "opacity", this);
        fadeOutAni->setDuration(outDuration);
        fadeOutAni->setStartValue(1.0);
        fadeOutAni->setEndValue(0.0);
        fadeOutAni->setEasingCurve(opacityCurve);
        m_animationGroup->addAnimation(fadeOutAni);
    }

    // 缩放并淡入下一个控件
    renderSnapshot(nextWidget, m_nextSnapshot);
    m_nextSnapshot->setScaledContents(true);
    nextWidget->hide();

    // 计算初始缩放矩形
    int inW = static_cast<int>(widgetRect.width() * inScale);
    int inH = static_cast<int>(widgetRect.height() * inScale);
    int inX = (widgetRect.width() - inW) / 2;
    int inY = (widgetRect.height() - inH) / 2;
    QRect inRect(inX, inY, inW, inH);

    m_nextSnapshot->setGeometry(inRect);

    // 缩放动画
    auto scaleInAni = new QPropertyAnimation(m_nextSnapshot, "geometry", this);
    scaleInAni->setDuration(inDuration);
    scaleInAni->setStartValue(inRect);
    scaleInAni->setEndValue(widgetRect);
    scaleInAni->setEasingCurve(inScaleCurve);
    m_animationGroup->addAnimation(scaleInAni);

    // 淡入动画
    auto fadeInAni = new QPropertyAnimation(
                m_nextSnapshot->graphicsEffect(), "opacity", this);
    fadeInAni->setDuration(inDuration);
    fadeInAni->setStartValue(0.0);
    fadeInAni->setEndValue(1.0);
    fadeInAni->setEasingCurve(opacityCurve);
    m_animationGroup->addAnimation(fadeInAni);
}


// ==================== StackedWidget ====================

StackedWidget::StackedWidget(QWidget *parent, AnimationType type)
    : QFrame(parent)
    , m_hBoxLayout(new QHBoxLayout(this))
    , m_view(nullptr)
    , m_animationType(type)
{
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);

    // 创建指定类型的堆叠控件
    m_view = createStackedWidget(type);
    m_hBoxLayout->addWidget(m_view);

    // 连接信号
    connectSignals();

    setAttribute(Qt::WA_StyledBackground);
}

void StackedWidget::setAnimationType(AnimationType type)
{
    if (m_animationType == type) {
        return;
    }

    // 断开旧的信号连接
    disconnectSignals();

    // 移除旧的堆叠控件
    if (m_view) {
        m_hBoxLayout->removeWidget(m_view);
        m_view->deleteLater();
    }

    // 创建新的堆叠控件
    m_animationType = type;
    m_view = createStackedWidget(type);
    m_hBoxLayout->addWidget(m_view);

    // 连接新的信号
    connectSignals();
}

bool StackedWidget::isAnimationEnabled() const
{
    if (!m_view) {
        return false;
    }

    // 根据类型调用相应的方法
    switch (m_animationType) {
    case AnimationType::Opacity:
        // OpacityAniStackedWidget 没有动画开关
        return true;

    case AnimationType::PopUp: {
        auto* view = qobject_cast<PopUpAniStackedWidget*>(m_view);
        return view ? view->isAnimationEnabled() : true;
    }

    case AnimationType::EntranceTransition:
    case AnimationType::DrillInTransition: {
        auto* view = qobject_cast<TransitionStackedWidget*>(m_view);
        return view ? view->isAnimationEnabled() : true;
    }
    }

    return true;
}

void StackedWidget::setAnimationEnabled(bool isEnabled)
{
    if (!m_view) {
        return;
    }

    // 根据类型调用相应的方法
    switch (m_animationType) {
    case AnimationType::Opacity:
        // OpacityAniStackedWidget 没有动画开关
        break;

    case AnimationType::PopUp: {
        auto* view = qobject_cast<PopUpAniStackedWidget*>(m_view);
        if (view) view->setAnimationEnabled(isEnabled);
        break;
    }

    case AnimationType::EntranceTransition:
    case AnimationType::DrillInTransition: {
        auto* view = qobject_cast<TransitionStackedWidget*>(m_view);
        if (view) view->setAnimationEnabled(isEnabled);
        break;
    }
    }
}

void StackedWidget::addWidget(QWidget *widget, int deltaX, int deltaY)
{
    if (!widget || !m_view) {
        return;
    }

    // 根据类型调用相应的方法
    switch (m_animationType) {
    case AnimationType::Opacity: {
        auto* view = qobject_cast<OpacityAniStackedWidget*>(m_view);
        if (view) view->addWidget(widget);
        break;
    }

    case AnimationType::PopUp: {
        auto* view = qobject_cast<PopUpAniStackedWidget*>(m_view);
        if (view) view->addWidget(widget, deltaX, deltaY);
        break;
    }

    case AnimationType::EntranceTransition:
    case AnimationType::DrillInTransition:
        m_view->addWidget(widget);
        break;
    }
}

void StackedWidget::removeWidget(QWidget *widget)
{
    if (!widget || !m_view) {
        return;
    }

    // 根据类型调用相应的方法
    switch (m_animationType) {
    case AnimationType::PopUp: {
        auto* view = qobject_cast<PopUpAniStackedWidget*>(m_view);
        if (view) view->removeWidget(widget);
        break;
    }

    default:
        m_view->removeWidget(widget);
        break;
    }
}

QWidget* StackedWidget::widget(int index) const
{
    return m_view ? m_view->widget(index) : nullptr;
}

void StackedWidget::setCurrentWidget(QWidget *widget, bool popOut,
                                     int duration, bool isBack)
{
    if (!widget || !m_view) {
        return;
    }

    // *** 关键优化：延迟重置滚动条,避免触发强制布局 ***
    // 根据类型调用相应的方法
    switch (m_animationType) {
    case AnimationType::Opacity: {
        auto* view = qobject_cast<OpacityAniStackedWidget*>(m_view);
        if (view) view->setCurrentWidget(widget);
        // 延迟重置滚动条,在切换完成后执行
        QTimer::singleShot(0, this, [this, widget]() {
            resetScrollBars(widget);
        });
        break;
    }

    case AnimationType::PopUp: {
        auto popUpView = qobject_cast<PopUpAniStackedWidget*>(m_view);
        if (!popUpView) break;
        if (!popOut) {
            popUpView->setCurrentWidget(widget, false, true,
                                        duration > 0 ? duration : 300);
        } else {
            popUpView->setCurrentWidget(widget, true, false,
                                        duration > 0 ? duration : 200,
                                        QEasingCurve::InQuad);
        }

        // 延迟重置滚动条,在切换完成后执行
        QTimer::singleShot(0, this, [this, widget]() {
            resetScrollBars(widget);
        });
        break;
    }

    case AnimationType::EntranceTransition:
    case AnimationType::DrillInTransition: {
        auto transitionView = qobject_cast<TransitionStackedWidget*>(m_view);
        if (!transitionView) break;
        transitionView->setCurrentWidget(widget, duration, isBack);

        // 延迟重置滚动条,在切换完成后执行
        QTimer::singleShot(0, this, [this, widget]() {
            resetScrollBars(widget);
        });
        break;
    }
    }
}

void StackedWidget::setCurrentIndex(int index, bool popOut,
                                    int duration, bool isBack)
{
    QWidget *w = widget(index);
    if (w) {
        setCurrentWidget(w, popOut, duration, isBack);
    }
}

int StackedWidget::currentIndex() const
{
    return m_view ? m_view->currentIndex() : -1;
}

QWidget* StackedWidget::currentWidget() const
{
    return m_view ? m_view->currentWidget() : nullptr;
}

int StackedWidget::indexOf(QWidget *widget) const
{
    return m_view ? m_view->indexOf(widget) : -1;
}

int StackedWidget::count() const
{
    return m_view ? m_view->count() : 0;
}

QStackedWidget* StackedWidget::createStackedWidget(AnimationType type)
{
    switch (type) {
    case AnimationType::Opacity:
        return new OpacityAniStackedWidget(this);

    case AnimationType::PopUp:
        return new PopUpAniStackedWidget(this);

    case AnimationType::EntranceTransition:
        return new EntranceTransitionStackedWidget(this);

    case AnimationType::DrillInTransition:
        return new DrillInTransitionStackedWidget(this);
    }

    // 默认返回弹出动画类型
    return new PopUpAniStackedWidget(this);
}

void StackedWidget::connectSignals()
{
    if (!m_view) {
        return;
    }

    // 连接 currentChanged 信号（所有类型都有）
    connect(m_view, &QStackedWidget::currentChanged,
            this, &StackedWidget::currentChanged);

    // 根据类型连接动画相关信号
    switch (m_animationType) {
    case AnimationType::PopUp: {
        auto popUpView = qobject_cast<PopUpAniStackedWidget*>(m_view);
        connect(popUpView, &PopUpAniStackedWidget::aniStart,
                this, &StackedWidget::aniStart);
        connect(popUpView, &PopUpAniStackedWidget::aniFinished,
                this, &StackedWidget::aniFinished);
        break;
    }

    case AnimationType::EntranceTransition:
    case AnimationType::DrillInTransition: {
        auto transitionView = qobject_cast<TransitionStackedWidget*>(m_view);
        connect(transitionView, &TransitionStackedWidget::aniStart,
                this, &StackedWidget::aniStart);
        connect(transitionView, &TransitionStackedWidget::aniFinished,
                this, &StackedWidget::aniFinished);
        break;
    }

    default:
        break;
    }
}

void StackedWidget::disconnectSignals()
{
    if (!m_view) {
        return;
    }

    // 断开所有信号连接
    disconnect(m_view, nullptr, this, nullptr);
}

void StackedWidget::resetScrollBars(QWidget *widget)
{
    if (!widget) {
        return;
    }

    // 如果是滚动区域,自动滚动到顶部
    if (auto scrollArea = qobject_cast<QAbstractScrollArea*>(widget)) {
        if (QScrollBar *vbar = scrollArea->verticalScrollBar()) {
            vbar->setValue(0);
        }
        if (QScrollBar *hbar = scrollArea->horizontalScrollBar()) {
            hbar->setValue(0);
        }
    }
}
