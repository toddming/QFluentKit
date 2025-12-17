#pragma once

#include "QFluent/FluentWindow.h"


class MainWindow : public FluentWindow {
    Q_OBJECT
public:
    explicit MainWindow();

    void setCurrentInterface(const QString &routeKey, int index);

private:
    void showDialog();
};
