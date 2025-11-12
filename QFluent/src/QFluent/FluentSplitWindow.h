#pragma once

#include <QMainWindow>

#include "Define.h"
#include "FluentIcon.h"

class NavigationPanel;
class FluentSplitWindowPrivate;
class NavigationAvatarWidget;
class QFLUENT_EXPORT FluentSplitWindow : public QMainWindow
{
    Q_OBJECT
    Q_Q_CREATE(FluentSplitWindow)

public:
    FluentSplitWindow(QMainWindow *parent = nullptr);
    ~FluentSplitWindow();

    void setWindowButtonFlag(AppBarType::ButtonType buttonFlag, bool isEnable = true);
    void setWindowButtonFlags(AppBarType::ButtonFlags buttonFlags);
    AppBarType::ButtonFlags getWindowButtonFlags() const;

    void setWindowDisplayMode(ApplicationType::WindowDisplayMode windowDisplayType);
    ApplicationType::WindowDisplayMode windowDisplayMode() const;

    void setCustomWindowIcon(const QPixmap &pixmap, const QSize &size);

    void addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                         QWidget* widget, bool selectable = true,
                         NavigationType::NavigationItemPosition position = NavigationType::NavigationItemPosition::TOP,
                         const QString& tooltip = QString(), const QString& parentRouteKey = QString());

    NavigationPanel *navigationInterface() const;


protected:
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;
};
