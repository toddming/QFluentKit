#pragma once
#include "MaskDialogBasePrivate.h"

#include <QString>

class QLabel;
class QFrame;
class BodyLabel;
class QPushButton;
class PrimaryPushButton;
class MessageDialog;
class MessageDialogPrivate : public MaskDialogBasePrivate
{
    Q_D_CREATE(MessageDialog)
public:
    explicit MessageDialogPrivate();
    // 添加MessageDialog特有的私有成员

private:
    QLabel *titleLabel;
    BodyLabel *contentLabel;
    QFrame *buttonGroup;
    PrimaryPushButton *yesButton;
    QPushButton *cancelButton;

    QString _content;
    QWidget *_dialog;

    void setQss();

    void adjustText();
    void setContentCopyable(bool isCopyable);

};
