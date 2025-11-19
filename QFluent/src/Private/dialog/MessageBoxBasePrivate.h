#pragma once
#include "MaskDialogBasePrivate.h"
#include "QFluent/dialog/MaskDialogBase.h"
#include <QString>

class QFrame;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class PrimaryPushButton;
class MessageBoxBasePrivate : public MaskDialogBasePrivate
{
    Q_DECLARE_PUBLIC(MaskDialogBase)
public:
    MaskDialogBase *q_ptr{nullptr};

    QFrame *_buttonGroup;

    QVBoxLayout *_viewLayout;
    QHBoxLayout *_buttonLayout;
    PrimaryPushButton *_yesButton;
    QPushButton *_cancelButton;
};
