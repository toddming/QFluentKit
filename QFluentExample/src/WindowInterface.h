#pragma once

#include "GalleryInterface.h"

class WindowInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit WindowInterface(QWidget *parent = nullptr);


};
