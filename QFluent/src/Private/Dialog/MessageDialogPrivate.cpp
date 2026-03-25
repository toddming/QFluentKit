#include "MessageDialogPrivate.h"

#include <QLabel>
#include <QPushButton>
#include <QFrame>

#include "StyleSheet.h"
#include "TextWrap.h"
#include "QFluent/Label.h"
#include "QFluent/PushButton.h"


void MessageDialogPrivate::setQss()
{
    titleLabel->setObjectName("titleLabel");
    contentLabel->setObjectName("contentLabel");
    buttonGroup->setObjectName("buttonGroup");
    cancelButton->setObjectName("cancelButton");

    StyleSheetManager::instance()->registerWidget(dialog, Fluent::ThemeStyle::DIALOG);
    StyleSheetManager::instance()->registerWidget(contentLabel, Fluent::ThemeStyle::DIALOG);

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
    if (!dialog || !titleLabel || !contentLabel) {
        return;
    }

    int chars = 100;

    bool isTopLevelWindow = dialog->isWindow();

    if (isTopLevelWindow) {
        QWidget *parentWidget = dialog->parentWidget();
        if (parentWidget) {
            int w = qMax(titleLabel->width(), parentWidget->width());
            chars = qMax(qMin(w / 9, 140), 30);
        } else {
            chars = 100;
        }
    } else {
        QWidget *topWindow = dialog->window();
        if (topWindow) {
            int w = qMax(titleLabel->width(), topWindow->width());
            chars = qMax(qMin(w / 9, 100), 30);
        } else {
            chars = 100;
        }
    }

    contentLabel->setText(TextWrap::wrap(content, chars, false).first);
}
