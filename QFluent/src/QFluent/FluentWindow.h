#ifndef FLUENTWINDOW_H
#define FLUENTWINDOW_H

#include <QMainWindow>

#include "FluentGlobal.h"
#include "FluentIcon.h"

class NavigationPanel;
class FluentWindowPrivate;
class NavigationAvatarWidget;
class QFLUENT_EXPORT FluentWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, FluentWindow)

public:
    FluentWindow(QMainWindow *parent = nullptr);
    ~FluentWindow();

    void setWindowButtonHint(Fluent::WindowButtonHint hint, bool isEnable = true);
    void setWindowButtonHints(Fluent::WindowButtonHints hints);
    Fluent::WindowButtonHints windowButtonHints() const;

    void setWindowEffect(Fluent::WindowEffect effect);
    Fluent::WindowEffect windowEffect() const;

    void setCustomWindowIcon(const QPixmap &pixmap, const QSize &size);

    void addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                         QWidget* widget, bool selectable = true,
                         Fluent::NavigationItemPosition position = Fluent::NavigationItemPosition::TOP,
                         const QString& tooltip = QString(), const QString& parentRouteKey = QString());

    NavigationPanel *navigationInterface() const;


protected:
    bool event(QEvent *event) override;

private:
    QScopedPointer<FluentWindowPrivate> d_ptr;

};
#endif // FLUENTWINDOW_H
