#include "FluentWindowPrivate.h"
#include "QWKWidgets/widgetwindowagent.h"
#include "QFluent/FluentWindow.h"
#include "Theme.h"

void FluentWindowPrivate::setDarkTheme(bool dark) {
    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(windowAgent);
    if (agent == nullptr) {
        return;
    }
    agent->setWindowAttribute("dark-mode", dark);
    Theme::instance()->setTheme(dark ? ThemeType::ThemeMode::DARK : ThemeType::ThemeMode::LIGHT);
}
