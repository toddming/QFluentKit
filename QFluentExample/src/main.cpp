#include <QApplication>

#include "Theme.h"
#include "MainWindow.h"
#include "ConfigManager.h"

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    QApplication app(argc, argv);
    app.setStyle("Fusion");

    QFont font;
    font.setFamilies({"Microsoft YaHei", "PingFang SC", "Segoe UI"});
    font.setPixelSize(14);
    app.setFont(font);

    int theme = ConfigManager::instance().getValue("Window/theme", 0).toInt();
    Theme::instance()->setThemeColor(QColor(ConfigManager::instance().getValue("Window/color", "#0066b4").toString()), true);
    Theme::instance()->setTheme(theme == 0 ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT, true);

    MainWindow w;
    w.show();
    return app.exec();
}
