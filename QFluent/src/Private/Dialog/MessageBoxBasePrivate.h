#pragma once

#include "MaskDialogBasePrivate.h"
#include "QFluent/Dialog/MaskDialogBase.h"
#include "QFluent/Dialog/MessageBoxBase.h"

class QFrame;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class PrimaryPushButton;

class MessageBoxBasePrivate : public MaskDialogBasePrivate
{
    Q_DECLARE_PUBLIC(MessageBoxBase)

public:
    QFrame *m_buttonGroup;

    QVBoxLayout *m_viewLayout;
    QHBoxLayout *m_buttonLayout;
    PrimaryPushButton *m_yesButton;
    QPushButton *m_cancelButton;
};
