#include "MessageBoxBase.h"

#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "StyleSheet.h"

#include "QFluent/PushButton.h"
#include "Private/Dialog/MessageBoxBasePrivate.h"

MessageBoxBase::MessageBoxBase(QWidget *parent)
    : MaskDialogBase(*new MessageBoxBasePrivate, parent)
{
    Q_D(MessageBoxBase);

    d->buttonGroup = new QFrame(centerWidget());
    d->yesButton = new PrimaryPushButton(QObject::tr("OK"), d->buttonGroup);
    d->cancelButton = new QPushButton(QObject::tr("Cancel"), d->buttonGroup);
    d->yesButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    d->cancelButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);

    auto *vBoxLayout = new QVBoxLayout(centerWidget());
    d->viewLayout = new QVBoxLayout();
    d->buttonLayout = new QHBoxLayout(d->buttonGroup);

    d->buttonGroup->setObjectName("buttonGroup");
    d->cancelButton->setObjectName("cancelButton");
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::DIALOG);

    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addLayout(d->viewLayout, 1);
    vBoxLayout->addWidget(d->buttonGroup, 0, Qt::AlignBottom);

    d->viewLayout->setSpacing(12);
    d->viewLayout->setContentsMargins(24, 24, 24, 24);

    d->buttonLayout->setSpacing(12);
    d->buttonLayout->setContentsMargins(24, 24, 24, 24);
    d->buttonLayout->addWidget(d->yesButton, 1, Qt::AlignVCenter);
    d->buttonLayout->addWidget(d->cancelButton, 1, Qt::AlignVCenter);

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 50));
    setMaskColor(QColor(0, 0, 0, 76));

    d->yesButton->setFocus();
    d->buttonGroup->setFixedHeight(81);

    connect(d->yesButton, &QPushButton::clicked, this, [this]() {
        accept();
    });

    connect(d->cancelButton, &QPushButton::clicked, this, [this]() {
        reject();
    });

}

MessageBoxBase::~MessageBoxBase()
{

}

void MessageBoxBase::hideYesButton()
{
    Q_D(MessageBoxBase);
    d->yesButton->hide();
    d->buttonLayout->insertStretch(0, 1);
}

void MessageBoxBase::hideCancelButton()
{
    Q_D(MessageBoxBase);
    d->cancelButton->hide();
    d->buttonLayout->insertStretch(0, 1);
}

QPushButton* MessageBoxBase::yesButton()
{
    Q_D(MessageBoxBase);
    return d->yesButton;
}

QPushButton* MessageBoxBase::cancelbutton()
{
    Q_D(MessageBoxBase);
    return d->cancelButton;
}


QVBoxLayout* MessageBoxBase::viewLayout()
{
    Q_D(MessageBoxBase);
    return d->viewLayout;
}
