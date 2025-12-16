#pragma once
#include "MaskDialogBasePrivate.h"
#include "QFluent/Dialog/MaskDialogBase.h"
#include "QFluent/Dialog/MessageBoxBase.h"
#include <QString>

class QFrame;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class PrimaryPushButton;
class MessageBoxBasePrivate : public MaskDialogBasePrivate
{
    Q_DECLARE_PUBLIC(MessageBoxBase)

public:
    QFrame *_buttonGroup;

    QVBoxLayout *_viewLayout;
    QHBoxLayout *_buttonLayout;
    PrimaryPushButton *_yesButton;
    QPushButton *_cancelButton;
};
