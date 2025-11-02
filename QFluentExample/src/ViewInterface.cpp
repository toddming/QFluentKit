#include "ViewInterface.h"

#include <QWidget>

ViewInterface::ViewInterface(QWidget *parent)
    : GalleryInterface("视图", "qfluentwidgets.components.widgets", parent)
{
    setObjectName("ViewInterface");

}


