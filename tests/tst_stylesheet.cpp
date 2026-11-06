#include <QtTest>
#include <QWidget>
#include <QPushButton>
#include <memory>

#include "Theme.h"
#include "StyleSheet.h"
#include "FluentGlobal.h"

class TestStyleSheet : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void themeColorVariablesAreReplaced();
    void resourceFileIsLoadable();
    void registerAndDeregister();
    void destroyedWidgetIsDeregistered();
    void lightAndDarkStyleSheetsDiffer();
    void repeatedRegistrationDoesNotGrowRegistry();
    void customStyleSheetOverrides();
};

void TestStyleSheet::init()
{
    Theme::setThemeColor(QColor(0x00, 0x66, 0xb4), true);
    Theme::setThemeMode(Fluent::ThemeMode::LIGHT, true);
    StyleSheet::clearThemeColorCache();
}

void TestStyleSheet::themeColorVariablesAreReplaced()
{
    const QString qss = QStringLiteral(
        "QPushButton { color: --ThemeColorPrimary; border-color: --ThemeColorDark2; }");
    const QString result = StyleSheet::applyThemeColor(qss);

    QVERIFY(!result.contains(QStringLiteral("--ThemeColor")));
    QVERIFY(result.contains(Theme::themeColor(Fluent::ThemeColor::PRIMARY).name()));

    // 空输入必须安全返回
    QVERIFY(StyleSheet::applyThemeColor(QString()).isEmpty());
    // 无变量时原样返回
    const QString plain = QStringLiteral("QWidget { margin: 0px; }");
    QCOMPARE(StyleSheet::applyThemeColor(plain), plain);
}

void TestStyleSheet::resourceFileIsLoadable()
{
    const QString content = StyleSheet::styleSheetFromFile(
        QStringLiteral(":/qfluent/style/light/button.qss"));
    QVERIFY2(!content.isEmpty(), "资源未编译进库，或路径错误");

    // 不存在的路径应返回空串而不是崩溃
    QVERIFY(StyleSheet::styleSheetFromFile(QStringLiteral(":/qfluent/style/nope.qss")).isEmpty());
}

void TestStyleSheet::registerAndDeregister()
{
    QWidget widget;
    QVERIFY(!StyleSheet::isRegistered(&widget));

    StyleSheet::registerWidget(&widget, Fluent::ThemeStyle::BUTTON);
    QVERIFY(StyleSheet::isRegistered(&widget));
    QVERIFY(!widget.styleSheet().isEmpty());

    StyleSheet::deregisterWidget(&widget);
    QVERIFY(!StyleSheet::isRegistered(&widget));

    // 重复反注册不应崩溃
    StyleSheet::deregisterWidget(&widget);
    StyleSheet::deregisterWidget(nullptr);
}

void TestStyleSheet::destroyedWidgetIsDeregistered()
{
    auto* manager = StyleSheetManager::instance();
    QVERIFY(manager != nullptr);
    const int baseline = manager->widgets().size();

    auto* widget = new QWidget;
    StyleSheet::registerWidget(widget, Fluent::ThemeStyle::BUTTON);
    QCOMPARE(manager->widgets().size(), baseline + 1);

    delete widget;
    // 销毁后注册表必须回收，否则就是泄漏
    QCOMPARE(manager->widgets().size(), baseline);
}

void TestStyleSheet::lightAndDarkStyleSheetsDiffer()
{
    auto source = std::make_shared<FluentStyleSheet>(Fluent::ThemeStyle::BUTTON);

    const QString light = StyleSheet::styleSheet(source, Fluent::ThemeMode::LIGHT);
    const QString dark = StyleSheet::styleSheet(source, Fluent::ThemeMode::DARK);

    QVERIFY(!light.isEmpty());
    QVERIFY(!dark.isEmpty());
    QVERIFY2(light != dark, "亮/暗两套样式表内容相同，主题切换将无效");
}

void TestStyleSheet::repeatedRegistrationDoesNotGrowRegistry()
{
    auto* manager = StyleSheetManager::instance();
    const int baseline = manager->widgets().size();

    QWidget widget;
    // 同一个控件反复注册（reset=false 会不断叠加 source）
    for (int i = 0; i < 50; ++i) {
        StyleSheet::registerWidget(&widget, Fluent::ThemeStyle::BUTTON, false);
    }
    QCOMPARE(manager->widgets().size(), baseline + 1);

    // 叠加后的样式表体积不应无限膨胀
    const auto compose = manager->source(&widget);
    QVERIFY(compose != nullptr);
    QVERIFY2(compose->size() <= 51, "样式表 source 数量异常增长");

    StyleSheet::deregisterWidget(&widget);
    QCOMPARE(manager->widgets().size(), baseline);
}

void TestStyleSheet::customStyleSheetOverrides()
{
    QWidget widget;
    StyleSheet::registerWidget(&widget, Fluent::ThemeStyle::BUTTON);
    StyleSheet::setCustomStyleSheet(&widget,
                                    QStringLiteral("QWidget { color: red; }"),
                                    QStringLiteral("QWidget { color: blue; }"));

    Theme::setThemeMode(Fluent::ThemeMode::LIGHT, true);
    StyleSheet::updateStyleSheet();
    QVERIFY(widget.styleSheet().contains(QStringLiteral("red")));

    Theme::setThemeMode(Fluent::ThemeMode::DARK, true);
    StyleSheet::updateStyleSheet();
    QVERIFY(widget.styleSheet().contains(QStringLiteral("blue")));
}

QTEST_MAIN(TestStyleSheet)
#include "tst_stylesheet.moc"
