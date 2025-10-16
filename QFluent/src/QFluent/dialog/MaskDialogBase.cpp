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

#include "Private/dialog/MaskDialogBasePrivate.h"


Q_PROPERTY_CREATE_Q_CPP(MaskDialogBase, bool, IsClosableOnMaskClicked)
MaskDialogBase::MaskDialogBase(QWidget* parent)
    : QDialog(parent)
, d_ptr(new MaskDialogBasePrivate())
{
    Q_D(MaskDialogBase);
    d->q_ptr = this;

    d->init(parent);
}

// 在MaskDialogBase.cpp中
MaskDialogBase::MaskDialogBase(MaskDialogBasePrivate& dd, QWidget* parent)
    : QDialog(parent)
    , d_ptr(&dd)
{
    Q_D(MaskDialogBase);
    d->q_ptr = this;

    d->init(parent);
}

MaskDialogBase::~MaskDialogBase()
{

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
        _onDone(code);
    });

    opacityAni->start(QPropertyAnimation::DeleteWhenStopped);
}

void MaskDialogBase::_onDone(int code)
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
