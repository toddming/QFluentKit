#ifndef FLUENTWINDOW_H
#define FLUENTWINDOW_H

#include <QMainWindow>

#include "Define.h"
#include "FluentIcon.h"

class NavigationPanel;
class FluentWindowPrivate;
class QFLUENT_EXPORT FluentWindow : public QMainWindow
{
    Q_OBJECT
    Q_Q_CREATE(FluentWindow)

public:
    FluentWindow(QMainWindow *parent = nullptr);
    ~FluentWindow();

    void setWindowButtonFlag(AppBarType::ButtonType buttonFlag, bool isEnable = true);
    void setWindowButtonFlags(AppBarType::ButtonFlags buttonFlags);
    AppBarType::ButtonFlags getWindowButtonFlags() const;

    void setWindowDisplayMode(ApplicationType::WindowDisplayMode windowDisplayType);
    void setCustomWindowIcon(const QPixmap &pixmap, const QSize &size);

    void addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                         QWidget* widget, bool selectable = true,
                         NavigationType::NavigationItemPosition position = NavigationType::NavigationItemPosition::TOP,
                         const QString& tooltip = QString(), const QString& parentRouteKey = QString());

    NavigationPanel *navigationInterface() const;

protected:
    bool event(QEvent *event) override;

};
#endif // FLUENTWINDOW_H
