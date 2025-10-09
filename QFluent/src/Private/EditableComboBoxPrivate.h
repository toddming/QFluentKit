#pragma once

#include "Define.h"

class CheckBox;
class CheckBoxPrivate
{
public:
    Q_D_CREATE(CheckBox)

private:
    QColor borderColor();
    QColor backgroundColor();
    CheckBoxType::CheckBoxState state();

    bool _isPressed = false;
    bool _isHover = false;
};
