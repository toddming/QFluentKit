#pragma once

#include "GalleryInterface.h"

class Frame : public QFrame
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = nullptr);

    void addWidget(QWidget *widget);

private:
    QHBoxLayout *hBoxLayout;
    void setupUI();
    void applyStyleSheet();
};


class ViewInterface : public GalleryInterface
{
    Q_OBJECT
public:
    explicit ViewInterface(QWidget *parent = nullptr);


};
