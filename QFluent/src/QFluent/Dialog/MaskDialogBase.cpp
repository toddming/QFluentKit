#include "MaskDialogBase.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QResizeEvent>

#include "Theme.h"
#include "Private/Dialog/MaskDialogBasePrivate.h"


MaskDialogBase::MaskDialogBase(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new MaskDialogBasePrivate)
{
    Q_D(MaskDialogBase);
    d->q_ptr = this;

    d->m_isClosableOnMaskClicked = false;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    d->m_hBoxLayout = new QHBoxLayout(this);
    d->m_hBoxLayout->setContentsMargins(0, 0, 0, 0);

    d->m_windowMask = new QWidget(this);
    d->m_windowMask->installEventFilter(this);

    d->m_centerWidget = new QFrame(this);
    d->m_centerWidget->setObjectName("centerWidget");
    d->m_hBoxLayout->addWidget(d->m_centerWidget, 1, Qt::AlignCenter);

    int c = Theme::instance()->isDarkTheme() ? 0 : 255;
    d->m_windowMask->setStyleSheet(QString("background: rgba(%1, %1, %1, 153);").arg(c));

    setShadowEffect();

    setupEventFilters();
}

MaskDialogBase::MaskDialogBase(MaskDialogBasePrivate &dd, QWidget *parent)
    : QDialog(parent)
    , d_ptr(&dd)
{
    Q_D(MaskDialogBase);
    d->q_ptr = this;

    d->m_isClosableOnMaskClicked = false;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    d->m_hBoxLayout = new QHBoxLayout(this);
    d->m_hBoxLayout->setContentsMargins(0, 0, 0, 0);

    d->m_windowMask = new QWidget(this);
    d->m_windowMask->installEventFilter(this);

    d->m_centerWidget = new QFrame(this);
    d->m_centerWidget->setObjectName("centerWidget");
    d->m_hBoxLayout->addWidget(d->m_centerWidget, 1, Qt::AlignCenter);

    int c = Theme::instance()->isDarkTheme() ? 0 : 255;
    d->m_windowMask->setStyleSheet(QString("background: rgba(%1, %1, %1, 153);").arg(c));

    setShadowEffect();

    setupEventFilters();
}

MaskDialogBase::~MaskDialogBase()
{
    QWidget *tw = targetWidget();
    if (tw) {
        tw->removeEventFilter(this);
    }
}

void MaskDialogBase::setupEventFilters()
{
    QWidget *tw = targetWidget();
    if (tw) {
        tw->installEventFilter(this);
    }
}

QWidget* MaskDialogBase::targetWidget() const
{
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) {
        return nullptr;
    }

    // 判断parent是否是顶级窗口
    // 如果parent就是顶级窗口,则覆盖parent
    // 如果parent不是顶级窗口,也覆盖parent(局部遮罩)
    return parentWidget;
}

QHBoxLayout* MaskDialogBase::hBoxLayout()
{
    Q_D(MaskDialogBase);
    return d->m_hBoxLayout;
}

QWidget* MaskDialogBase::centerWidget() const
{
    Q_D(const MaskDialogBase);

    return d->m_centerWidget;
}

void MaskDialogBase::setMaskColor(const QColor &color)
{
    Q_D(MaskDialogBase);
    d->m_windowMask->setStyleSheet(QString(
                                      "background: rgba(%1, %2, %3, %4);"
                                      ).arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
    update();
}

void MaskDialogBase::setShadowEffect(int blurRadius, const QPoint &offset, const QColor &color)
{
    Q_D(MaskDialogBase);

    // Qt 的 setGraphicsEffect 会自动删除旧效果，直接设置新效果即可
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(d->m_centerWidget);
    shadowEffect->setBlurRadius(blurRadius);
    shadowEffect->setOffset(offset);
    shadowEffect->setColor(color);
    d->m_centerWidget->setGraphicsEffect(shadowEffect);
}


void MaskDialogBase::showEvent(QShowEvent *event)
{
    // 显示时同步目标窗口尺寸
    syncGeometryWithParent();

    auto *opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);

    auto *opacityAni = new QPropertyAnimation(opacityEffect, "opacity", this);
    opacityAni->setStartValue(0.0);
    opacityAni->setEndValue(1.0);
    opacityAni->setDuration(200);
    opacityAni->setEasingCurve(QEasingCurve::InSine);

    connect(opacityAni, &QPropertyAnimation::finished, this, [this, opacityAni]() {
        opacityAni->deleteLater();
        setGraphicsEffect(nullptr);
    });

    opacityAni->start(QPropertyAnimation::DeleteWhenStopped);

    QDialog::showEvent(event);
}

void MaskDialogBase::done(int code)
{
    Q_D(MaskDialogBase);

    d->m_centerWidget->setGraphicsEffect(nullptr);

    auto *opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);

    auto *opacityAni = new QPropertyAnimation(opacityEffect, "opacity", this);
    opacityAni->setStartValue(1.0);
    opacityAni->setEndValue(0.0);
    opacityAni->setDuration(100);

    connect(opacityAni, &QPropertyAnimation::finished, this, [this, opacityAni, code]() {
        opacityAni->deleteLater();
        onDone(code);
    });

    opacityAni->start(QPropertyAnimation::DeleteWhenStopped);
}

void MaskDialogBase::onDone(int code)
{
    setGraphicsEffect(nullptr);
    QDialog::done(code);
}

void MaskDialogBase::resizeEvent(QResizeEvent *event)
{
    Q_D(MaskDialogBase);

    d->m_windowMask->resize(size());
    QDialog::resizeEvent(event);
}

bool MaskDialogBase::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(MaskDialogBase);

    QWidget *tw = targetWidget();

    // 处理目标窗口的resize和move事件
    if (watched == tw) {
        if (event->type() == QEvent::Resize) {
            syncGeometryWithParent();
        } else if (event->type() == QEvent::Move) {
            // 如果目标窗口移动,确保dialog位置正确
            syncGeometryWithParent();
        } else if (event->type() == QEvent::ScreenChangeInternal) {
            // 屏幕改变时也需要同步
            QMetaObject::invokeMethod(this, [this]() {
                syncGeometryWithParent();
            }, Qt::QueuedConnection);
        }
    } else if (watched == d->m_windowMask && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && d->m_isClosableOnMaskClicked) {
            reject();
        }
    }

    return QDialog::eventFilter(watched, event);
}

void MaskDialogBase::syncGeometryWithParent()
{
    QWidget *tw = targetWidget();
    if (!tw) {
        return;
    }

    // 获取目标窗口的尺寸
    QSize targetSize = tw->size();

    // 如果目标窗口尺寸有效,则调整dialog大小
    if (targetSize.width() > 0 && targetSize.height() > 0) {
        // 将dialog的geometry设置为相对于父窗口的坐标
        // 由于dialog的parent已经设置,这里的坐标是相对于parent的
        QWidget *parentWidget = qobject_cast<QWidget*>(parent());
        if (parentWidget) {
            // 如果parent就是targetWidget,直接覆盖
            if (parentWidget == tw) {
                setGeometry(0, 0, targetSize.width(), targetSize.height());
            } else {
                // 如果parent不是targetWidget(例如parent是子控件,target是顶级窗口)
                // 需要计算相对位置
                QPoint targetPosInParent = parentWidget->mapFrom(tw, QPoint(0, 0));
                setGeometry(targetPosInParent.x(), targetPosInParent.y(),
                           targetSize.width(), targetSize.height());
            }
        }
    }
}

void MaskDialogBase::setIsClosableOnMaskClicked(bool enable)
{
    Q_D(MaskDialogBase);
    d->m_isClosableOnMaskClicked = enable;
}

bool MaskDialogBase::isClosableOnMaskClicked() const
{
    Q_D(const MaskDialogBase);
    return d->m_isClosableOnMaskClicked;
}
