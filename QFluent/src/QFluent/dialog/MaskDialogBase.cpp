#include "MaskDialogBase.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QFrame>
#include <QResizeEvent>
#include <QDebug>

#include "Theme.h"
#include "Private/dialog/MaskDialogBasePrivate.h"


Q_PROPERTY_CREATE_Q_CPP(MaskDialogBase, bool, IsClosableOnMaskClicked)
MaskDialogBase::MaskDialogBase(QWidget* parent)
    : QDialog(parent)
, d_ptr(new MaskDialogBasePrivate())
{
    Q_D(MaskDialogBase);
    d->q_ptr = this;

    d->_pIsClosableOnMaskClicked = false;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    if (parent) {
        setGeometry(0, 0, parent->width(), parent->height());
    }
    d->_hBoxLayout = new QHBoxLayout(this);
    d->_hBoxLayout->setContentsMargins(0, 0, 0, 0);

    d->_windowMask = new QWidget(this);
    d->_windowMask->installEventFilter(this);

    d->_centerWidget = new QFrame(this);
    d->_centerWidget->setObjectName("centerWidget");
    d->_hBoxLayout->addWidget(d->_centerWidget, 1, Qt::AlignCenter);

    d->_windowMask->resize(size());

    int c = Theme::instance()->isDarkMode() ? 0 : 255;
    d->_windowMask->setStyleSheet(QString("background: rgba(%1, %1, %1, 153);").arg(c));

    setShadowEffect();

    if (parent) {
        parent->installEventFilter(this);
    }
    window()->installEventFilter(this);
}

// 在MaskDialogBase.cpp中
MaskDialogBase::MaskDialogBase(MaskDialogBasePrivate& dd, QWidget* parent)
    : QDialog(parent)
    , d_ptr(&dd)
{
    Q_D(MaskDialogBase);
    d->q_ptr = this;

    d->_pIsClosableOnMaskClicked = false;

    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    if (parent) {
        setGeometry(0, 0, parent->width(), parent->height());
    }
    d->_hBoxLayout = new QHBoxLayout(this);
    d->_hBoxLayout->setContentsMargins(0, 0, 0, 0);

    d->_windowMask = new QWidget(this);
    d->_windowMask->installEventFilter(this);

    d->_centerWidget = new QFrame(this);
    d->_centerWidget->setObjectName("centerWidget");
    d->_hBoxLayout->addWidget(d->_centerWidget, 1, Qt::AlignCenter);

    d->_windowMask->resize(size());

    int c = Theme::instance()->isDarkMode() ? 0 : 255;
    d->_windowMask->setStyleSheet(QString("background: rgba(%1, %1, %1, 153);").arg(c));

    setShadowEffect();

    if (parent) {
        parent->installEventFilter(this);
    }
    window()->installEventFilter(this);

    qDebug() << "MaskDialogBase constructor: d_ptr.data() =222" << d_ptr.data();

}

MaskDialogBase::~MaskDialogBase()
{

}

QHBoxLayout* MaskDialogBase::hBoxLayout()
{
    Q_D(MaskDialogBase);
    return d->_hBoxLayout;
}

QWidget* MaskDialogBase::centerWidget() const
{
    Q_D_CONST(MaskDialogBase);

    return d->_centerWidget;
}

void MaskDialogBase::setMaskColor(const QColor& color)
{
    Q_D(MaskDialogBase);
    d->_windowMask->setStyleSheet(QString(
                                    "background: rgba(%1, %2, %3, %4);"
                                    ).arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
    update();
}

void MaskDialogBase::setShadowEffect(int blurRadius, const QPoint& offset, const QColor& color)
{
    Q_D(MaskDialogBase);

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(d->_centerWidget);
    shadowEffect->setBlurRadius(blurRadius);
    shadowEffect->setOffset(offset);
    shadowEffect->setColor(color);
    d->_centerWidget->setGraphicsEffect(nullptr);
    d->_centerWidget->setGraphicsEffect(shadowEffect);
}


void MaskDialogBase::showEvent(QShowEvent* event)
{
    auto* opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);

    auto* opacityAni = new QPropertyAnimation(opacityEffect, "opacity", this);
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

    d->_centerWidget->setGraphicsEffect(nullptr);

    auto* opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);

    auto* opacityAni = new QPropertyAnimation(opacityEffect, "opacity", this);
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

void MaskDialogBase::resizeEvent(QResizeEvent* event)
{
    Q_D(MaskDialogBase);

    d->_windowMask->resize(size());
    QDialog::resizeEvent(event);
}

bool MaskDialogBase::eventFilter(QObject* obj, QEvent* event)
{
    Q_D(MaskDialogBase);
    if (obj == window() && event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        resize(resizeEvent->size());
    } else if (obj == window() && event->type() == QEvent::ScreenChangeInternal) {
        QTimer::singleShot(0, this, [=](){
            QWidget *w = qobject_cast<QWidget*>(this->parent());
            setGeometry(0, 0, w->width(), w->height());
        });
    } else if (obj == d->_windowMask && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && d->_pIsClosableOnMaskClicked) {
            reject();
        }
    }

    return QDialog::eventFilter(obj, event);
}
