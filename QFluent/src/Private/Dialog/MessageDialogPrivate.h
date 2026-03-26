#pragma once

#include "MaskDialogBasePrivate.h"
#include "QFluent/Dialog/MessageDialog.h"

class QLabel;
class QFrame;
class BodyLabel;
class QPushButton;
class PrimaryPushButton;

class MessageDialogPrivate : public MaskDialogBasePrivate
{
    Q_DECLARE_PUBLIC(MessageDialog)

public:
    QLabel *m_titleLabel;
    BodyLabel *m_contentLabel;
    QFrame *m_buttonGroup;
    PrimaryPushButton *m_yesButton;
    QPushButton *m_cancelButton;

    QString m_content;
    QWidget *m_dialog;

    void setQss();
    void adjustText();
    void setContentCopyable(bool isCopyable);
};
