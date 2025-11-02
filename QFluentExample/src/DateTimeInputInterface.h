#pragma once

#include "GalleryInterface.h"

class DateTimeInputInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit DateTimeInputInterface(QWidget *parent = nullptr);

};
