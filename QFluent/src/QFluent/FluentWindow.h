#ifndef FLUENTWINDOW_H
#define FLUENTWINDOW_H

#include <QMainWindow>

#include "Property.h"

class StackedWidget;
class QFLUENT_EXPORT FluentWindow : public QMainWindow
{
    Q_OBJECT

public:
    FluentWindow(QMainWindow *parent = nullptr);
    ~FluentWindow();

protected:
    bool event(QEvent *event) override;

private:
    void initUI();

    void setDarkTheme(bool dark);

    QObject *windowAgent{nullptr};

    void switchTo(QWidget *w);

    StackedWidget *stacked;

};
#endif // FLUENTWINDOW_H
