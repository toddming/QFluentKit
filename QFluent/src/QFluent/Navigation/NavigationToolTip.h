#pragma once

#include "QFluent/ToolTip.h"

#include "FluentGlobal.h"

class QFLUENT_EXPORT NavigationToolTipFilter : public ToolTipFilter
{
public:
    using ToolTipFilter::ToolTipFilter;

protected:
    bool canShowToolTip() const override;
};
