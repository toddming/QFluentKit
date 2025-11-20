#pragma once

#include <QMainWindow>

#include "FluentGlobal.h"
#include "FluentIcon.h"

class NavigationPanel;
class FluentSplitWindowPrivate;
class NavigationAvatarWidget;
class QFLUENT_EXPORT FluentSplitWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, FluentSplitWindow)

public:
    FluentSplitWindow(QMainWindow *parent = nullptr);
    ~FluentSplitWindow();

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
    void resizeEvent(QResizeEvent *e) override;

private:
    QScopedPointer<FluentSplitWindowPrivate> d_ptr;

};
