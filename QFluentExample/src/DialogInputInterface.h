#pragma once

#include "GalleryInterface.h"

class DialogInputInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit DialogInputInterface(QWidget *parent = nullptr);

private slots:
    void showSimpleFlyout(QWidget* target);
    void showComplexFlyout(QWidget* target);
    void showBottomTeachingTip(QWidget* target);
    void showLeftBottomTeachingTip(QWidget* target);
};
