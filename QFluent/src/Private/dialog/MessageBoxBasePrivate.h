#pragma once
#include "MaskDialogBasePrivate.h"

#include <QString>

class QFrame;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class PrimaryPushButton;
class MessageBoxBase;
class MessageBoxBasePrivate : public MaskDialogBasePrivate
{
    Q_D_CREATE(MessageBoxBase)
public:
    explicit MessageBoxBasePrivate();

private:
    QFrame *_buttonGroup;

    QVBoxLayout *_viewLayout;
    QHBoxLayout *_buttonLayout;
    PrimaryPushButton *_yesButton;
    QPushButton *_cancelButton;
};
