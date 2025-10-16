#include "MaskDialogBasePrivate.h"

#include <QFrame>
#include <QWidget>
#include <QHBoxLayout>
#include "Theme.h"

#include "QFluent/dialog/MaskDialogBase.h"

MaskDialogBasePrivate::MaskDialogBasePrivate()
{

}

void MaskDialogBasePrivate::init(QWidget* parent)
{
    Q_Q(MaskDialogBase);

    _pIsClosableOnMaskClicked = false;

    q->setWindowFlags(Qt::FramelessWindowHint);
    q->setAttribute(Qt::WA_TranslucentBackground);
    if (parent) {
        q->setGeometry(0, 0, parent->width(), parent->height());
    }
    QHBoxLayout *hBoxLayout = new QHBoxLayout(q);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);

    _windowMask = new QWidget(q);
    _windowMask->installEventFilter(q);

    _centerWidget = new QFrame(q);
    _centerWidget->setObjectName("centerWidget");
    hBoxLayout->addWidget(_centerWidget, 1, Qt::AlignCenter);

    _windowMask->resize(q->size());

    int c = Theme::instance()->isDarkMode() ? 0 : 255;
    _windowMask->setStyleSheet(QString("background: rgba(%1, %1, %1, 153);").arg(c));

    q->setShadowEffect();

    if (parent) {
        parent->installEventFilter(q);
    }
    q->window()->installEventFilter(q);
}
