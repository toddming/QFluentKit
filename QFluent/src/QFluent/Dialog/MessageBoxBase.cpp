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

    d->m_buttonGroup = new QFrame(centerWidget());
    d->m_yesButton = new PrimaryPushButton(QObject::tr("OK"), d->m_buttonGroup);
    d->m_cancelButton = new QPushButton(QObject::tr("Cancel"), d->m_buttonGroup);
    d->m_yesButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    d->m_cancelButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);

    auto *vBoxLayout = new QVBoxLayout(centerWidget());
    d->m_viewLayout = new QVBoxLayout();
    d->m_buttonLayout = new QHBoxLayout(d->m_buttonGroup);

    d->m_buttonGroup->setObjectName("buttonGroup");
    d->m_cancelButton->setObjectName("cancelButton");
    StyleSheet::registerWidget(this, Fluent::ThemeStyle::DIALOG);

    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addLayout(d->m_viewLayout, 1);
    vBoxLayout->addWidget(d->m_buttonGroup, 0, Qt::AlignBottom);

    d->m_viewLayout->setSpacing(12);
    d->m_viewLayout->setContentsMargins(24, 24, 24, 24);

    d->m_buttonLayout->setSpacing(12);
    d->m_buttonLayout->setContentsMargins(24, 24, 24, 24);
    d->m_buttonLayout->addWidget(d->m_yesButton, 1, Qt::AlignVCenter);
    d->m_buttonLayout->addWidget(d->m_cancelButton, 1, Qt::AlignVCenter);

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 50));
    setMaskColor(QColor(0, 0, 0, 76));

    d->m_yesButton->setFocus();
    d->m_buttonGroup->setFixedHeight(81);

    connect(d->m_yesButton, &QPushButton::clicked, this, [this]() {
        accept();
    });

    connect(d->m_cancelButton, &QPushButton::clicked, this, [this]() {
        reject();
    });

}

MessageBoxBase::~MessageBoxBase()
{

}

void MessageBoxBase::hideYesButton()
{
    Q_D(MessageBoxBase);
    d->m_yesButton->hide();
    d->m_buttonLayout->insertStretch(0, 1);
}

void MessageBoxBase::hideCancelButton()
{
    Q_D(MessageBoxBase);
    d->m_cancelButton->hide();
    d->m_buttonLayout->insertStretch(0, 1);
}

QPushButton* MessageBoxBase::yesButton() const
{
    Q_D(const MessageBoxBase);
    return d->m_yesButton;
}

QPushButton* MessageBoxBase::cancelButton() const
{
    Q_D(const MessageBoxBase);
    return d->m_cancelButton;
}


QVBoxLayout* MessageBoxBase::viewLayout() const
{
    Q_D(const MessageBoxBase);
    return d->m_viewLayout;
}
