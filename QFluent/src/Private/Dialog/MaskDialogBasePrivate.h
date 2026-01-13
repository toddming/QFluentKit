#pragma once

#include "QFluent/Dialog/MaskDialogBase.h"

class QFrame;
class QWidget;
class QHBoxLayout;
class MaskDialogBasePrivate
{
    Q_DECLARE_PUBLIC(MaskDialogBase)

public:
    MaskDialogBase *q_ptr{nullptr};

    bool _isClosableOnMaskClicked;
    QHBoxLayout *_hBoxLayout;
    QWidget* _windowMask = nullptr;
    QFrame* _centerWidget = nullptr;
};
