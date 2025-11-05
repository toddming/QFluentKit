#pragma once

#include <QProgressBar>
#include "GalleryInterface.h"

class StatusInfoInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit StatusInfoInterface(QWidget *parent = nullptr);

private:
    QWidget* createProgressWidget(QProgressBar *widget);
};
