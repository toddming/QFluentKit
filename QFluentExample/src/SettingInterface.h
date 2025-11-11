#pragma once

#include "QFluent/ScrollArea.h"
#include "QFluent/layout/ExpandLayout.h"

class SettingInterface : public ScrollArea
{
    Q_OBJECT
public:
    explicit SettingInterface(QWidget *parent = nullptr);

private:
    QWidget *m_scrollWidget;
    ExpandLayout *m_expandLayout;

    void initConf();
};
