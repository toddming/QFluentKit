#include "NavigationToolTip.h"

bool NavigationToolTipFilter::canShowToolTip() const
{
    bool isVisible = ToolTipFilter::canShowToolTip();
    if (!parent())
        return isVisible;
    bool isCompacted = parent()->property("isCompacted").toBool();
    return isVisible && isCompacted;
}
