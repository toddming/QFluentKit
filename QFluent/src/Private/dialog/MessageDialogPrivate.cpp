#include "MessageDialogPrivate.h"

#include <QLabel>
#include <QPushButton>
#include <QFrame>

#include "TextWrap.h"
#include "QFluent/Label.h"
#include "QFluent/PushButton.h"

MessageDialogPrivate::MessageDialogPrivate()
    : MaskDialogBasePrivate()
{
    // 特有初始化
}

void MessageDialogPrivate::setQss()
{
    titleLabel->setObjectName("titleLabel");
    contentLabel->setObjectName("contentLabel");
    buttonGroup->setObjectName("buttonGroup");
    cancelButton->setObjectName("cancelButton");

    Theme::instance()->registerWidget(_dialog, ThemeType::ThemeStyle::DIALOG);
    Theme::instance()->registerWidget(contentLabel, ThemeType::ThemeStyle::DIALOG);

    yesButton->adjustSize();
    cancelButton->adjustSize();
}

void MessageDialogPrivate::setContentCopyable(bool isCopyable)
{
    contentLabel->setTextInteractionFlags(
        isCopyable ?
            (Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard) :
            Qt::NoTextInteraction
    );
}

void MessageDialogPrivate::adjustText()
{
    if (!_dialog || !titleLabel || !contentLabel) {
        return;
    }

    int chars = 100;

    bool isTopLevelWindow = _dialog->isWindow();

    if (isTopLevelWindow) {
        QWidget *parentWidget = _dialog->parentWidget();
        if (parentWidget) {
            int w = qMax(titleLabel->width(), parentWidget->width());
            chars = qMax(qMin(w / 9, 140), 30);
        } else {
            chars = 100;
        }
    } else {
        QWidget *topWindow = _dialog->window();
        if (topWindow) {
            int w = qMax(titleLabel->width(), topWindow->width());
            chars = qMax(qMin(w / 9, 100), 30);
        } else {
            chars = 100;
        }
    }

    contentLabel->setText(TextWrap::wrap(_content, chars, false).first);
}
