#pragma once
#include "MaskDialogBasePrivate.h"
#include "QFluent/Dialog/MessageDialog.h"
#include <QString>

class QLabel;
class QFrame;
class BodyLabel;
class QPushButton;
class PrimaryPushButton;
class MessageDialogPrivate : public MaskDialogBasePrivate
{
    Q_DECLARE_PUBLIC(MessageDialog)

public:
    QLabel *titleLabel;
    BodyLabel *contentLabel;
    QFrame *buttonGroup;
    PrimaryPushButton *yesButton;
    QPushButton *cancelButton;

    QString content;
    QWidget *dialog;

    void setQss();

    void adjustText();
    void setContentCopyable(bool isCopyable);

};
