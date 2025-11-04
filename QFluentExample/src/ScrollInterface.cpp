#include "ScrollInterface.h"

#include "QFluent/PagiNation.h"

ScrollInterface::ScrollInterface(QWidget *parent)
    : GalleryInterface("滚动", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("ScrollInterface");

    auto pagiNation = new PagiNation(this);
    pagiNation->setAlign(PagiNationType::Align_Left);
    pagiNation->setPageSize(10);
    pagiNation->setTotal(500);
    addExampleCard("页码", pagiNation);
}


