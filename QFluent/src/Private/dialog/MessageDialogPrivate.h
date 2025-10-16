#pragma once
#include "MaskDialogBasePrivate.h"

class MessageDialog;
class MessageDialogPrivate : public MaskDialogBasePrivate
{
    Q_D_CREATE(MessageDialog)
public:
    explicit MessageDialogPrivate();
    // 添加MessageDialog特有的私有成员
};
