#pragma once

#include "QFluent/Dialog/MaskDialogBase.h"

class QFrame;
class QWidget;
class QHBoxLayout;

class MaskDialogBasePrivate
{
    Q_DECLARE_PUBLIC(MaskDialogBase)

public:
    MaskDialogBase *q_ptr = nullptr;

    bool m_isClosableOnMaskClicked;
    QHBoxLayout *m_hBoxLayout;
    QWidget *m_windowMask = nullptr;
    QFrame *m_centerWidget = nullptr;
};
