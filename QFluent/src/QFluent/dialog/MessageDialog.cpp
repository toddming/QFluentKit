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

    d->_dialog = this->centerWidget();
    d->_content = content;

    // 创建控件
    d->titleLabel = new QLabel(title, centerWidget());
    d->contentLabel = new BodyLabel(centerWidget());

    d->buttonGroup = new QFrame(centerWidget());
    d->yesButton = new PrimaryPushButton(QObject::tr("OK"), d->buttonGroup);
    d->cancelButton = new QPushButton(QObject::tr("Cancel"), d->buttonGroup);
    d->yesButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    d->cancelButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);

    // 布局
    auto *vBoxLayout = new QVBoxLayout(centerWidget());
    auto *textLayout = new QVBoxLayout();
    auto *buttonLayout = new QHBoxLayout(d->buttonGroup);

    textLayout->setSpacing(12);
    textLayout->setContentsMargins(24, 24, 24, 24);
    textLayout->addWidget(d->titleLabel, 0, Qt::AlignTop);
    textLayout->addWidget(d->contentLabel, 0, Qt::AlignTop);

    buttonLayout->setSpacing(12);
    buttonLayout->setContentsMargins(24, 24, 24, 24);
    buttonLayout->addWidget(d->yesButton, 1, Qt::AlignTop);
    buttonLayout->addWidget(d->cancelButton, 1, Qt::AlignTop);
    d->buttonGroup->setFixedHeight(81);

    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addLayout(textLayout, 1);
    vBoxLayout->addWidget(d->buttonGroup, 0, Qt::AlignBottom);
    vBoxLayout->setSizeConstraint(QVBoxLayout::SetMinimumSize);

    d->yesButton->setFocus();
    //
    d->titleLabel->setObjectName("titleLabel");
    d->contentLabel->setObjectName("contentLabel");
    d->buttonGroup->setObjectName("buttonGroup");
    d->cancelButton->setObjectName("cancelButton");

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::DIALOG);

    d->yesButton->adjustSize();

    d->adjustText();

    centerWidget()->installEventFilter(centerWidget());

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 50));
    setMaskColor(QColor(0, 0, 0, 76));

    connect(d->yesButton, &QPushButton::clicked, this, [this]() {
        accept();
        emit yesClicked();
    });

    connect(d->cancelButton, &QPushButton::clicked, this, [this]() {
        reject();
        emit cancelClicked();
    });

    centerWidget()->setFixedSize(qMax(d->contentLabel->width(), d->titleLabel->width()) + 48,
                                 d->contentLabel->y() + d->contentLabel->height() + 105);

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
