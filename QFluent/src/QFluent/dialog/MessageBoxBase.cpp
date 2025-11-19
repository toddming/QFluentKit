#include "MessageBoxBase.h"

#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "StyleSheet.h"

#include "QFluent/PushButton.h"
#include "Private/dialog/MessageBoxBasePrivate.h"

MessageBoxBase::MessageBoxBase(QWidget *parent)
    : MaskDialogBase(*new MessageBoxBasePrivate, parent)
{
    Q_D(MessageBoxBase);

    d->_buttonGroup = new QFrame(centerWidget());
    d->_yesButton = new PrimaryPushButton(QObject::tr("OK"), d->_buttonGroup);
    d->_cancelButton = new QPushButton(QObject::tr("Cancel"), d->_buttonGroup);
    d->_yesButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    d->_cancelButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);

    auto *vBoxLayout = new QVBoxLayout(centerWidget());
    d->_viewLayout = new QVBoxLayout();
    d->_buttonLayout = new QHBoxLayout(d->_buttonGroup);

    d->_buttonGroup->setObjectName("buttonGroup");
    d->_cancelButton->setObjectName("cancelButton");
    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::DIALOG);

    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addLayout(d->_viewLayout, 1);
    vBoxLayout->addWidget(d->_buttonGroup, 0, Qt::AlignBottom);

    d->_viewLayout->setSpacing(12);
    d->_viewLayout->setContentsMargins(24, 24, 24, 24);

    d->_buttonLayout->setSpacing(12);
    d->_buttonLayout->setContentsMargins(24, 24, 24, 24);
    d->_buttonLayout->addWidget(d->_yesButton, 1, Qt::AlignVCenter);
    d->_buttonLayout->addWidget(d->_cancelButton, 1, Qt::AlignVCenter);

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 50));
    setMaskColor(QColor(0, 0, 0, 76));

    d->_yesButton->setFocus();
    d->_buttonGroup->setFixedHeight(81);

    connect(d->_yesButton, &QPushButton::clicked, this, [this]() {
        accept();
    });

    connect(d->_cancelButton, &QPushButton::clicked, this, [this]() {
        reject();
    });

}

MessageBoxBase::~MessageBoxBase()
{

}

void MessageBoxBase::hideYesButton()
{
    Q_D(MessageBoxBase);
    d->_yesButton->hide();
    d->_buttonLayout->insertStretch(0, 1);
}

void MessageBoxBase::hideCancelButton()
{
    Q_D(MessageBoxBase);
    d->_cancelButton->hide();
    d->_buttonLayout->insertStretch(0, 1);
}

QPushButton* MessageBoxBase::yesButton()
{
    Q_D(MessageBoxBase);
    return d->_yesButton;
}

QPushButton* MessageBoxBase::cancelbutton()
{
    Q_D(MessageBoxBase);
    return d->_cancelButton;
}


QVBoxLayout* MessageBoxBase::viewLayout()
{
    Q_D(MessageBoxBase);
    return d->_viewLayout;
}
