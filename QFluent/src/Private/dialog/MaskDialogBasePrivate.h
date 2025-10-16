#pragma once

#include "Property.h"

class QFrame;
class QWidget;
class MaskDialogBase;
class MaskDialogBasePrivate
{
    Q_D_CREATE(MaskDialogBase)
    Q_PROPERTY_CREATE_D(bool, IsClosableOnMaskClicked)

public:
    explicit MaskDialogBasePrivate();

private:
    void init(QWidget* parent);

    QWidget* _windowMask = nullptr;
    QFrame* _centerWidget = nullptr;
};
