#pragma once

#include <QContextMenuEvent>
#include "GalleryInterface.h"
#include "QFluent/ImageLabel.h"
#include "QFluent/Material/AcrylicLabel.h"

class MenuLabel;
class MenuAcrylicLabel;
class MaterialInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit MaterialInterface(QWidget *parent = nullptr);

private:
    Action* createTimeAction;
    Action* shootTimeAction;
    Action* modifiedTimeAction;
    Action* nameAction;
    Action* ascendAction;
    Action* descendAction;

    MenuAcrylicLabel *menuAcrylicLabel;

    void createMenu(QPoint pos);
    void createCustomWidgetMenu(QPoint pos);
    void createCheckableMenu(QPoint pos);
    void createSliderMenu(QPoint pos);
};

class MenuLabel : public ImageLabel
{
    Q_OBJECT
public:
    explicit MenuLabel(QWidget *parent = nullptr);

signals:
    void mouseRightClicked(const QPoint &pos);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

class MenuAcrylicLabel : public AcrylicLabel
{
  Q_OBJECT
public:
    explicit MenuAcrylicLabel(QWidget *parent = nullptr);
signals:
    void mouseRightClicked(const QPoint &pos);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

};
