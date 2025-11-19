#include "ScrollInterface.h"

#include "QFluent/PagiNation.h"

ScrollInterface::ScrollInterface(QWidget *parent)
    : GalleryInterface("滚动", "", parent)
{
    setObjectName("ScrollInterface");

    auto pagiNation = new PagiNation(this);
    pagiNation->setAlign(Fluent::Alignment::Align_Left);
    pagiNation->setPageSize(10);
    pagiNation->setTotal(500);
    addExampleCard("页码", pagiNation);
}


