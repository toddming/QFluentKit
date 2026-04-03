#pragma once

#include "FluentGlobal.h"
#include "QFluent/CheckBox.h"

class CheckBoxPrivate
{
    Q_DECLARE_PUBLIC(CheckBox)

public:
    CheckBox *q_ptr{nullptr};

    QColor borderColor() const;
    QColor backgroundColor() const;
    CheckBox::State state() const;

    bool m_isPressed = false;
    bool m_isHover = false;
};
