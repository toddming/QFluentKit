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
    m_titleLabel->setObjectName("titleLabel");
    m_contentLabel->setObjectName("contentLabel");
    m_buttonGroup->setObjectName("buttonGroup");
    m_cancelButton->setObjectName("cancelButton");

    StyleSheet::registerWidget(m_dialog, Fluent::ThemeStyle::DIALOG);
    StyleSheet::registerWidget(m_contentLabel, Fluent::ThemeStyle::DIALOG);

    m_yesButton->adjustSize();
    m_cancelButton->adjustSize();
}

void MessageDialogPrivate::setContentCopyable(bool isCopyable)
{
    m_contentLabel->setTextInteractionFlags(
        isCopyable ?
            (Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard) :
            Qt::NoTextInteraction
    );
}

void MessageDialogPrivate::adjustText()
{
    if (!m_dialog || !m_titleLabel || !m_contentLabel) {
        return;
    }

    int chars = 100;

    bool isTopLevelWindow = m_dialog->isWindow();

    if (isTopLevelWindow) {
        QWidget *parentWidget = m_dialog->parentWidget();
        if (parentWidget) {
            int w = qMax(m_titleLabel->width(), parentWidget->width());
            chars = qMax(qMin(w / 9, 140), 30);
        } else {
            chars = 100;
        }
    } else {
        QWidget *topWindow = m_dialog->window();
        if (topWindow) {
            int w = qMax(m_titleLabel->width(), topWindow->width());
            chars = qMax(qMin(w / 9, 100), 30);
        } else {
            chars = 100;
        }
    }

    m_contentLabel->setText(TextWrap::wrap(m_content, chars, false).first);
}
