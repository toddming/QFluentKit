#include "MessageDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

#include "StyleSheet.h"
#include "QFluent/Label.h"
#include "QFluent/PushButton.h"
#include "Private/Dialog/MessageDialogPrivate.h"

MessageDialog::MessageDialog(const QString &title, const QString &content, QWidget *parent)
    : MaskDialogBase(*new MessageDialogPrivate(), parent)
{
    Q_D(MessageDialog);

    setAttribute(Qt::WA_DeleteOnClose);

    d->m_dialog = this->centerWidget();
    d->m_content = content;

    // 创建控件
    d->m_titleLabel = new QLabel(title, centerWidget());
    d->m_contentLabel = new BodyLabel(centerWidget());

    d->m_buttonGroup = new QFrame(centerWidget());
    d->m_yesButton = new PrimaryPushButton(QObject::tr("OK"), d->m_buttonGroup);
    d->m_cancelButton = new QPushButton(QObject::tr("Cancel"), d->m_buttonGroup);
    d->m_yesButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    d->m_cancelButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);

    // 布局
    auto *vBoxLayout = new QVBoxLayout(centerWidget());
    auto *textLayout = new QVBoxLayout();
    auto *buttonLayout = new QHBoxLayout(d->m_buttonGroup);

    textLayout->setSpacing(12);
    textLayout->setContentsMargins(24, 24, 24, 24);
    textLayout->addWidget(d->m_titleLabel, 0, Qt::AlignTop);
    textLayout->addWidget(d->m_contentLabel, 0, Qt::AlignTop);

    buttonLayout->setSpacing(12);
    buttonLayout->setContentsMargins(24, 24, 24, 24);
    buttonLayout->addWidget(d->m_yesButton, 1, Qt::AlignTop);
    buttonLayout->addWidget(d->m_cancelButton, 1, Qt::AlignTop);
    d->m_buttonGroup->setFixedHeight(81);

    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addLayout(textLayout, 1);
    vBoxLayout->addWidget(d->m_buttonGroup, 0, Qt::AlignBottom);
    vBoxLayout->setSizeConstraint(QVBoxLayout::SetMinimumSize);

    d->m_yesButton->setFocus();
    //
    d->m_titleLabel->setObjectName("titleLabel");
    d->m_contentLabel->setObjectName("contentLabel");
    d->m_buttonGroup->setObjectName("buttonGroup");
    d->m_cancelButton->setObjectName("cancelButton");

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::DIALOG);

    d->m_yesButton->adjustSize();

    d->adjustText();

    centerWidget()->installEventFilter(centerWidget());

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 50));
    setMaskColor(QColor(0, 0, 0, 76));

    connect(d->m_yesButton, &QPushButton::clicked, this, [this]() {
        accept();
        emit yesClicked();
    });

    connect(d->m_cancelButton, &QPushButton::clicked, this, [this]() {
        reject();
        emit cancelClicked();
    });

    centerWidget()->setFixedSize(qMax(d->m_contentLabel->width(), d->m_titleLabel->width()) + 48,
                                 d->m_contentLabel->y() + d->m_contentLabel->height() + 105);

    d->setContentCopyable(true);
}

MessageDialog::~MessageDialog()
{

}


bool MessageDialog::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(MessageDialog);
    if (watched == this && event->type() == QEvent::Resize) {
        d->adjustText();
        return true;
    }
    return MaskDialogBase::eventFilter(watched, event);
}
