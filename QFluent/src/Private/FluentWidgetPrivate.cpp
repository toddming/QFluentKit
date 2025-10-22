#include "FluentWidgetPrivate.h"
#include "QWKWidgets/widgetwindowagent.h"
#include "QFluent/FluentWidget.h"
#include "Theme.h"
#include "StyleSheet.h"

#include <QFile>
#include <QTextStream>

void FluentWidgetPrivate::setDarkTheme(bool dark) {
    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(windowAgent);
    if (agent == nullptr) {
        return;
    }
    agent->setWindowAttribute("dark-mode", dark);
    Theme::instance()->setTheme(dark ? ThemeType::ThemeMode::DARK : ThemeType::ThemeMode::LIGHT);
}
