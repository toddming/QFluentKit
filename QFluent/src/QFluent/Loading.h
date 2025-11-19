#pragma once

#include "dialog/MaskDialogBase.h"
#include "FluentGlobal.h"

class QFLUENT_EXPORT Loading : public MaskDialogBase
{
    Q_OBJECT
public:
    explicit Loading(const QString &content = QString(), QWidget *parent = nullptr);

};
