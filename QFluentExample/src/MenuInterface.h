#pragma once

#include "GalleryInterface.h"

class MenuInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit MenuInterface(QWidget *parent = nullptr);

private:
    Action* createTimeAction;
    Action* shootTimeAction;
    Action* modifiedTimeAction;
    Action* nameAction;
    Action* ascendAction;
    Action* descendAction;

    void createMenu(QPoint pos);
    void createCustomWidgetMenu(QPoint pos);
    void createCheckableMenu(QPoint pos);
};
