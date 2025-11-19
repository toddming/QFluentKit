#include "FluentWidgetPrivate.h"
#include "QWKWidgets/widgetwindowagent.h"

#include "Theme.h"

void FluentWidgetPrivate::setDarkTheme(bool dark) {
    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(_windowAgent);
    if (agent == nullptr) {
        return;
    }
    agent->setWindowAttribute("dark-mode", dark);
    Theme::instance()->setTheme(dark ? ThemeType::ThemeMode::DARK : ThemeType::ThemeMode::LIGHT);
}
