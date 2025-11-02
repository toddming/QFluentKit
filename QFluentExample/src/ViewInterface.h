#pragma once

#include "GalleryInterface.h"

class ViewInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit ViewInterface(QWidget *parent = nullptr);


};
