#pragma once

#include "GalleryInterface.h"

class BasicInputInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit BasicInputInterface(QWidget *parent = nullptr);

};
