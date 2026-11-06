#include <QtTest>
#include <QSignalSpy>
#include <QColor>
#include <QFont>
#include <QWidget>

#include "Theme.h"
#include "FluentGlobal.h"

class TestTheme : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void themeModeSwitch();
    void lazyModeDoesNotEmit();
    void toggleThemeFlipsMode();
    void themeColorRoundTrip();
    void derivedColorsAreOrdered();
    void derivedColorsValidForAchromaticThemeColor();
    void fontCacheIsConsistent();
    void signalHelperReceivesModeChange();
};

void TestTheme::init()
{
    // 每个用例从确定的亮色 + 默认主色开始，避免用例间互相污染
    Theme::setThemeColor(QColor(0x00, 0x66, 0xb4), true);
    Theme::setThemeMode(Fluent::ThemeMode::LIGHT, true);
}

void TestTheme::themeModeSwitch()
{
    QCOMPARE(Theme::themeMode(), Fluent::ThemeMode::LIGHT);
    QVERIFY(!Theme::isDark());

    Theme::setThemeMode(Fluent::ThemeMode::DARK);
    QCOMPARE(Theme::themeMode(), Fluent::ThemeMode::DARK);
    QVERIFY(Theme::isDark());
}

void TestTheme::lazyModeDoesNotEmit()
{
    // 用一个短生命周期的接收者，测试结束即自动断开连接，
    // 避免捕获的局部变量在后续用例中变成悬空引用
    QObject receiver;
    int calls = 0;
    Theme::onThemeModeChanged(&receiver, [&calls](Fluent::ThemeMode) { ++calls; });

    Theme::setThemeMode(Fluent::ThemeMode::DARK, true);
    QCOMPARE(calls, 0);
    // 状态已切换，只是没有广播
    QCOMPARE(Theme::themeMode(), Fluent::ThemeMode::DARK);

    // 非 lazy 调用才广播
    Theme::setThemeMode(Fluent::ThemeMode::LIGHT);
    QCOMPARE(calls, 1);
}

void TestTheme::toggleThemeFlipsMode()
{
    QCOMPARE(Theme::themeMode(), Fluent::ThemeMode::LIGHT);
    Theme::toggleTheme();
    QCOMPARE(Theme::themeMode(), Fluent::ThemeMode::DARK);
    Theme::toggleTheme();
    QCOMPARE(Theme::themeMode(), Fluent::ThemeMode::LIGHT);
}

void TestTheme::themeColorRoundTrip()
{
    const QColor custom(0x12, 0x34, 0x56);
    Theme::setThemeColor(custom);
    QCOMPARE(Theme::themeColor(), custom);
    QCOMPARE(Theme::themeColor(Fluent::ThemeColor::PRIMARY), custom);
}

void TestTheme::derivedColorsAreOrdered()
{
    const QColor primary = Theme::themeColor(Fluent::ThemeColor::PRIMARY);
    const QColor dark1 = Theme::themeColor(Fluent::ThemeColor::DARK_1);
    const QColor dark3 = Theme::themeColor(Fluent::ThemeColor::DARK_3);

    QVERIFY2(dark1.value() <= primary.value(), "DARK_1 应不亮于主色");
    QVERIFY2(dark3.value() <= dark1.value(), "DARK_3 应不亮于 DARK_1");

    // 暗色主题下 DARK_1 仍然必须可用
    Theme::setThemeMode(Fluent::ThemeMode::DARK);
    QVERIFY(Theme::themeColor(Fluent::ThemeColor::DARK_1).isValid());
}

void TestTheme::derivedColorsValidForAchromaticThemeColor()
{
    // 灰色没有色相，QColor::getHsvF 会把 hue 置为 -1，
    // 派生色必须仍然有效，不能返回非法 QColor
    Theme::setThemeColor(QColor(128, 128, 128));

    const Fluent::ThemeColor types[] = {
        Fluent::ThemeColor::PRIMARY, Fluent::ThemeColor::DARK_1,
        Fluent::ThemeColor::DARK_2, Fluent::ThemeColor::DARK_3,
        Fluent::ThemeColor::LIGHT_1, Fluent::ThemeColor::LIGHT_2,
        Fluent::ThemeColor::LIGHT_3
    };
    for (auto type : types) {
        QVERIFY2(Theme::themeColor(type).isValid(),
                 qPrintable(QString("灰色主题色派生的颜色无效，type=%1").arg(int(type))));
    }

    // 纯白与纯黑同样是无彩色
    Theme::setThemeColor(QColor(255, 255, 255));
    QVERIFY(Theme::themeColor(Fluent::ThemeColor::DARK_2).isValid());
    Theme::setThemeColor(QColor(0, 0, 0));
    QVERIFY(Theme::themeColor(Fluent::ThemeColor::LIGHT_2).isValid());
}

void TestTheme::fontCacheIsConsistent()
{
    const QFont a = Theme::font(16);
    const QFont b = Theme::font(16);
    QCOMPARE(a.pixelSize(), 16);
    QCOMPARE(a.pixelSize(), b.pixelSize());
    QCOMPARE(a.weight(), QFont::Normal);

    const QFont bold = Theme::font(20, QFont::Bold);
    QCOMPARE(bold.pixelSize(), 20);
    QCOMPARE(bold.weight(), QFont::Bold);
}

void TestTheme::signalHelperReceivesModeChange()
{
    QObject receiver;
    int calls = 0;
    Fluent::ThemeMode last = Fluent::ThemeMode::AUTO;
    Theme::onThemeModeChanged(&receiver, [&calls, &last](Fluent::ThemeMode mode) {
        ++calls;
        last = mode;
    });

    Theme::setThemeMode(Fluent::ThemeMode::DARK);
    QCOMPARE(calls, 1);
    QCOMPARE(last, Fluent::ThemeMode::DARK);
}

QTEST_MAIN(TestTheme)
#include "tst_theme.moc"
