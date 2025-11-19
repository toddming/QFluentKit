#pragma once

#include "FluentGlobal.h"
#include "QFluent/CheckBox.h"

class CheckBoxPrivate
{
    Q_DECLARE_PUBLIC(CheckBox)

public:
    CheckBox *q_ptr{nullptr};

    QColor borderColor();
    QColor backgroundColor();
    Fluent::CheckBoxState state();

    bool _isPressed = false;
    bool _isHover = false;
};
