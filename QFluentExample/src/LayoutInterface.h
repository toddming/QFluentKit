#pragma once

#include "GalleryInterface.h"

class LayoutInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit LayoutInterface(QWidget *parent = nullptr);

private:
    QWidget *createWidget(bool animation = false);

};
