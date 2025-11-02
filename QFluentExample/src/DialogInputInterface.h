#pragma once

#include "GalleryInterface.h"

class DialogInputInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit DialogInputInterface(QWidget *parent = nullptr);

};
