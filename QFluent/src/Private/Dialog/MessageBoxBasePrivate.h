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
    QFrame *buttonGroup;

    QVBoxLayout *viewLayout;
    QHBoxLayout *buttonLayout;
    PrimaryPushButton *yesButton;
    QPushButton *cancelButton;
};
