#include "FluentWidgetPrivate.h"
#include "QWKWidgets/widgetwindowagent.h"
#include "QFluent/FluentWidget.h"
#include "Theme.h"
#include "StyleSheet.h"

#include <QFile>
#include <QTextStream>

void FluentWidgetPrivate::loadStyleSheet(bool dark) {
    Q_Q(FluentWidget);

    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(windowAgent);
    if (agent == nullptr) {
        return;
    }

    agent->setWindowAttribute("dark-mode", dark);

    QFile file(dark ? ":/res/style/dark-style.qss" : ":/res/style/light-style.qss");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Open Style File Error:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    q->setStyleSheet(content);
    file.close();

    Theme::instance()->setTheme(!dark ? ThemeType::ThemeMode::LIGHT : ThemeType::ThemeMode::DARK);
}
