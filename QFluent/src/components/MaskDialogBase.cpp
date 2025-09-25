#include "MaskDialogBase.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QTimer>

#include "../common/Theme.h"


MaskDialogBase::MaskDialogBase(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    if (parent) {
        setGeometry(0, 0, parent->width(), parent->height());
    }
    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);

    m_windowMask = new QWidget(this);
    m_windowMask->installEventFilter(this);

    m_centerWidget = new QFrame(this);
    m_centerWidget->setObjectName("centerWidget");
    hBoxLayout->addWidget(m_centerWidget, 1, Qt::AlignCenter);

    m_windowMask->resize(size());

    int c = sTheme->isDarkMode() ? 0 : 255;
    m_windowMask->setStyleSheet(QString("background: rgba(%1, %1, %1, 153);").arg(c));

    setShadowEffect();

    if (parent) {
        parent->installEventFilter(this);
    }
    window()->installEventFilter(this);
}

QWidget* MaskDialogBase::centerWidget() const
{
    return m_centerWidget;
}

void MaskDialogBase::setMaskColor(const QColor& color)
{
    m_windowMask->setStyleSheet(QString(
                                    "background: rgba(%1, %2, %3, %4);"
                                    ).arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
    update();
}

void MaskDialogBase::setShadowEffect(int blurRadius, const QPoint& offset, const QColor& color)
{
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(m_centerWidget);
    shadowEffect->setBlurRadius(blurRadius);
    shadowEffect->setOffset(offset);
    shadowEffect->setColor(color);
    m_centerWidget->setGraphicsEffect(nullptr);
    m_centerWidget->setGraphicsEffect(shadowEffect);
}

bool MaskDialogBase::isClosableOnMaskClicked() const
{
    return m_isClosableOnMaskClicked;
}

void MaskDialogBase::setClosableOnMaskClicked(bool isClosable)
{
    m_isClosableOnMaskClicked = isClosable;
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

    connect(opacityAni, &QPropertyAnimation::finished, this, [this, opacityEffect, opacityAni]() {
        opacityAni->deleteLater();
        setGraphicsEffect(nullptr);
    });

    opacityAni->start(QPropertyAnimation::DeleteWhenStopped);

    QDialog::showEvent(event);
}

void MaskDialogBase::done(int code)
{
    m_centerWidget->setGraphicsEffect(nullptr);

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
    m_windowMask->resize(size());
    QDialog::resizeEvent(event);
}

bool MaskDialogBase::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == window() && event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        resize(resizeEvent->size());
    } else if (obj == window() && event->type() == QEvent::ScreenChangeInternal) {
        QTimer::singleShot(0, this, [=](){
            QWidget *w = qobject_cast<QWidget*>(this->parent());
            setGeometry(0, 0, w->width(), w->height());
        });
    } else if (obj == m_windowMask && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && m_isClosableOnMaskClicked) {
            reject();
        }
    }

    return QDialog::eventFilter(obj, event);
}
