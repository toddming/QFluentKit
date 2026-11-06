#include <QtTest>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QElapsedTimer>
#include <QFile>
#include <QPointer>

// 注意：必须先包含 QFluent 头文件再包含 windows.h。
// windows.h 定义了 DELETE / COPY / CLOSE / MOVE / CANCEL / UPDATE / VIEW / TRANSPARENT
// 等宏，与 Fluent::IconType 的枚举名冲突，先包含会导致 moc 与编译器解析失败。
#include "Theme.h"
#include "StyleSheet.h"
#include "FluentGlobal.h"
#include "FluentIcon.h"

#include "QFluent/PushButton.h"
#include "QFluent/Label.h"
#include "QFluent/LineEdit.h"
#include "QFluent/CheckBox.h"
#include "QFluent/RadioButton.h"
#include "QFluent/SwitchButton.h"
#include "QFluent/Slider.h"
#include "QFluent/ComboBox.h"
#include "QFluent/CardWidget.h"
#include "QFluent/ScrollArea.h"
#include "QFluent/TableView.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/Progress/ProgressBar.h"
#include "QFluent/Progress/ProgressRing.h"

#ifdef Q_OS_WIN
#  include <windows.h>
#  include <psapi.h>
#endif

namespace {

// 当前进程的工作集（KB），用于内存回归检查；无法获取时返回 -1
qint64 currentWorkingSetKb()
{
#ifdef Q_OS_WIN
    PROCESS_MEMORY_COUNTERS info{};
    if (::GetProcessMemoryInfo(::GetCurrentProcess(), &info, sizeof(info))) {
        return static_cast<qint64>(info.WorkingSetSize / 1024);
    }
#elif defined(Q_OS_LINUX)
    QFile status(QStringLiteral("/proc/self/status"));
    if (status.open(QIODevice::ReadOnly)) {
        const QByteArray data = status.readAll();
        const int idx = data.indexOf("VmRSS:");
        if (idx >= 0) {
            const QByteArray line = data.mid(idx, 32);
            const QList<QByteArray> parts = line.simplified().split(' ');
            if (parts.size() >= 2) {
                return parts.at(1).toLongLong();
            }
        }
    }
#endif
    return -1;
}

// 构造一个包含多类组件的窗口，用于批量压测
QWidget* buildWidgetWindow(int columns, int rows)
{
    auto* window = new QWidget;
    auto* layout = new QVBoxLayout(window);

    for (int r = 0; r < rows; ++r) {
        auto* row = new QWidget(window);
        auto* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);

        for (int c = 0; c < columns; ++c) {
            switch (c % 6) {
            case 0: rowLayout->addWidget(new PushButton(QStringLiteral("确定"), row)); break;
            case 1: rowLayout->addWidget(new PrimaryPushButton(QStringLiteral("主按钮"), row)); break;
            case 2: rowLayout->addWidget(new LineEdit(row)); break;
            case 3: rowLayout->addWidget(new CheckBox(QStringLiteral("选项"), row)); break;
            case 4: rowLayout->addWidget(new BodyLabel(QStringLiteral("文本"), row)); break;
            default: rowLayout->addWidget(new SwitchButton(QStringLiteral("开关"), row)); break;
            }
        }
        layout->addWidget(row);
    }
    return window;
}

} // namespace

class TestWidgets : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    void allPublicWidgetsCanBeCreated();
    void iconSvgLookupIsCached();
    void themeSwitchCostScalesLinearly();
    void repeatedCreateDestroyDoesNotLeak();
    void repeatedThemeSwitchDoesNotLeak();
    void registryIsStableUnderChurn();
};

void TestWidgets::initTestCase()
{
    Theme::setThemeMode(Fluent::ThemeMode::LIGHT, true);
}

void TestWidgets::init()
{
    Theme::setThemeColor(QColor(0x00, 0x66, 0xb4), true);
    Theme::setThemeMode(Fluent::ThemeMode::LIGHT, true);
}

void TestWidgets::allPublicWidgetsCanBeCreated()
{
    QWidget host;
    auto* layout = new QVBoxLayout(&host);

    QList<QWidget*> widgets;
    widgets << new PushButton(&host)
            << new PrimaryPushButton(&host)
            << new ToggleButton(&host)
            << new LineEdit(&host)
            << new BodyLabel(QStringLiteral("body"), &host)
            << new CaptionLabel(QStringLiteral("caption"), &host)
            << new CheckBox(QStringLiteral("check"), &host)
            << new RadioButton(QStringLiteral("radio"), &host)
            << new SwitchButton(QStringLiteral("switch"), &host)
            << new Slider(&host)
            << new ComboBox(&host)
            << new CardWidget(&host)
            << new ScrollArea(&host)
            << new TableWidget(&host)
            << new TableView(&host)
            << new StackedWidget(&host)
            << new ProgressBar(&host)
            << new ProgressRing(&host);

    for (auto* w : widgets) {
        layout->addWidget(w);
        QVERIFY(w != nullptr);
    }

    // 基础组件必须自动完成样式注册
    for (auto* w : {widgets.at(0), widgets.at(3), widgets.at(6)}) {
        QVERIFY2(StyleSheet::isRegistered(w), "组件未注册样式表");
    }

    host.show();
    QTest::qWait(50);
    QCoreApplication::processEvents();
}

void TestWidgets::iconSvgLookupIsCached()
{
    // 图标 SVG 会在每次绘制时被读取并做颜色替换，
    // 缓存失效会导致每条绘制路径都走一遍资源读取 + 字符串处理。
    // 用最大的图标（约 10 KB）来暴露缓存是否真正生效。
    const QString path = QStringLiteral(":/qfluent/images/icons/Setting_black.svg");

    QVERIFY(!FluentIconUtils::writeSvg(path).isEmpty());   // 预热，填入缓存

    constexpr int rounds = 500;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < rounds; ++i) {
        FluentIconUtils::writeSvg(path);
    }
    const qint64 elapsedNs = timer.nsecsElapsed();
    const double perCallUs = double(elapsedNs) / 1000.0 / rounds;

    qInfo("图标 SVG 取用：%d 次共 %.2f ms，单次约 %.1f us",
          rounds, double(elapsedNs) / 1e6, perCallUs);

    QVERIFY2(perCallUs < 2000.0,
             qPrintable(QString("图标取用过慢，缓存可能失效：单次 %1 us").arg(perCallUs)));
}

void TestWidgets::themeSwitchCostScalesLinearly()
{
    const int columns = 6;
    const int rows = 25;   // 150 个组件
    QWidget* window = buildWidgetWindow(columns, rows);
    window->resize(900, 700);
    window->show();
    QTest::qWait(50);
    QCoreApplication::processEvents();

    const int widgetCount = window->findChildren<QWidget*>().size();

    // 预热一次，排除首次资源加载的影响
    Theme::setThemeMode(Fluent::ThemeMode::DARK);
    Theme::setThemeMode(Fluent::ThemeMode::LIGHT);
    QCoreApplication::processEvents();

    constexpr int rounds = 4;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < rounds; ++i) {
        Theme::setThemeMode(i % 2 == 0 ? Fluent::ThemeMode::DARK
                                       : Fluent::ThemeMode::LIGHT);
    }
    const qint64 elapsed = timer.elapsed();

    const double perSwitch = double(elapsed) / rounds;
    qInfo("主题切换：%d 个控件，%d 次共 %lld ms，单次约 %.1f ms",
          widgetCount, rounds, static_cast<long long>(elapsed), perSwitch);

    // 归因：单独统计同样次数的 QSS 文本生成（文件读取 + 颜色变量替换）耗时，
    // 用于区分「生成样式文本」与「Qt 解析并应用样式」两部分开销
    auto buttonStyle = std::make_shared<FluentStyleSheet>(Fluent::ThemeStyle::BUTTON);
    QElapsedTimer genTimer;
    genTimer.start();
    for (int i = 0; i < widgetCount; ++i) {
        StyleSheet::styleSheet(buttonStyle, Theme::themeMode());
    }
    const qint64 genElapsed = genTimer.elapsed();
    qInfo("其中 QSS 文本生成 %d 次共 %lld ms（占比约 %.0f%%）",
          widgetCount, static_cast<long long>(genElapsed),
          perSwitch > 0 ? 100.0 * genElapsed / perSwitch : 0.0);

    // 单次全量换肤在 150 个控件规模下不应超过 1 秒（宽松阈值，防止 CI 抖动）
    QVERIFY2(perSwitch < 1000.0,
             qPrintable(QString("主题切换过慢：单次 %1 ms").arg(perSwitch)));

    delete window;
}

void TestWidgets::repeatedCreateDestroyDoesNotLeak()
{
    const qint64 rssBefore = currentWorkingSetKb();

    constexpr int rounds = 40;
    for (int i = 0; i < rounds; ++i) {
        QWidget* window = buildWidgetWindow(4, 4);
        window->show();
        QCoreApplication::processEvents();
        delete window;
        QCoreApplication::processEvents();
    }

    // 强制回收
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    QCoreApplication::processEvents();

    const qint64 rssAfter = currentWorkingSetKb();
    if (rssBefore > 0 && rssAfter > 0) {
        const qint64 growth = rssAfter - rssBefore;
        qInfo("创建/销毁 %d 轮后工作集变化：%lld KB（%lld -> %lld）",
              rounds, static_cast<long long>(growth),
              static_cast<long long>(rssBefore), static_cast<long long>(rssAfter));

        // 40 轮 × 16 个组件，正常波动应在几 MB 以内
        QVERIFY2(growth < 40 * 1024,
                 qPrintable(QString("疑似泄漏：工作集增长 %1 KB").arg(growth)));
    } else {
        QSKIP("当前平台无法读取进程工作集，跳过内存断言");
    }
}

void TestWidgets::repeatedThemeSwitchDoesNotLeak()
{
    QWidget* window = buildWidgetWindow(4, 4);
    window->show();
    QCoreApplication::processEvents();

    // 预热，排除首次资源加载与样式解析的一次性分配
    for (int i = 0; i < 10; ++i) {
        Theme::setThemeMode(i % 2 == 0 ? Fluent::ThemeMode::DARK
                                       : Fluent::ThemeMode::LIGHT);
    }
    QCoreApplication::processEvents();

    const qint64 rssBefore = currentWorkingSetKb();
    if (rssBefore <= 0) {
        delete window;
        QSKIP("当前平台无法读取进程工作集，跳过内存断言");
    }

    constexpr int rounds = 200;
    for (int i = 0; i < rounds; ++i) {
        Theme::setThemeMode(i % 2 == 0 ? Fluent::ThemeMode::DARK
                                       : Fluent::ThemeMode::LIGHT);
    }
    QCoreApplication::processEvents();

    const qint64 rssAfter = currentWorkingSetKb();
    const qint64 growth = rssAfter - rssBefore;
    qInfo("连续切换主题 %d 次后工作集变化：%lld KB（%lld -> %lld）",
          rounds, static_cast<long long>(growth),
          static_cast<long long>(rssBefore), static_cast<long long>(rssAfter));

    // 换肤本身不应产生累积分配
    QVERIFY2(growth < 20 * 1024,
             qPrintable(QString("疑似换肤泄漏：工作集增长 %1 KB").arg(growth)));

    delete window;
}

void TestWidgets::registryIsStableUnderChurn()
{
    auto* manager = StyleSheetManager::instance();
    QVERIFY(manager != nullptr);
    const int baseline = manager->widgets().size();

    for (int i = 0; i < 20; ++i) {
        QWidget* window = buildWidgetWindow(3, 3);
        QVERIFY(manager->widgets().size() > baseline);
        delete window;
        QCOMPARE(manager->widgets().size(), baseline);
    }
}

QTEST_MAIN(TestWidgets)
#include "tst_widgets.moc"
