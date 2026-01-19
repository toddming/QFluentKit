#include "FluentWidget.h"
#include <QStyle>

#include "Theme.h"
#include "StyleSheet.h"

#if USE_QWINDOWKIT
#include <QEvent>
#include <QLabel>
#include "QWKWidgets/widgetwindowagent.h"
#else
#ifdef _WIN32
#include <dwmapi.h>
#endif
#endif

FluentWidget::FluentWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("FluentWindow");

    setAttribute(Qt::WA_DontCreateNativeAncestors);

#if USE_QWINDOWKIT
    QWK::WidgetWindowAgent *agent = new QWK::WidgetWindowAgent(this);
    agent->setup(this);

    _windowBar = new FluentTitleBar(this);
    _windowBar->setHostWidget(this);

    agent->setTitleBar(_windowBar);
    agent->setHitTestVisible(_windowBar->themeButton(), true);
    agent->setHitTestVisible(_windowBar->backButton(), true);
    agent->setSystemButton(QWK::WindowAgentBase::Minimize, _windowBar->minButton());
    agent->setSystemButton(QWK::WindowAgentBase::Maximize, _windowBar->maxButton());
    agent->setSystemButton(QWK::WindowAgentBase::Close, _windowBar->closeButton());

    connect(_windowBar, &FluentTitleBar::themeRequested, this, [=](bool checked){
        _windowBar->themeButton()->setChecked(checked);
        setDarkTheme(!checked);
    });
    connect(_windowBar, &FluentTitleBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(_windowBar, &FluentTitleBar::maximizeRequested, this, [this](bool max) {
        if (max) {
            showMaximized();
        } else {
            showNormal();
        }
    });
    connect(_windowBar, &FluentTitleBar::closeRequested, this, &QWidget::close);
    _windowAgent = agent;

    _windowBar->themeButton()->setChecked(Theme::instance()->isDarkTheme());

    connect(Theme::instance(), &Theme::themeModeChanged, this, [agent](Fluent::ThemeMode theme) {
        agent->setWindowAttribute("dark-mode", theme == Fluent::ThemeMode::DARK);
    });
#endif
    connect(Theme::instance(), &Theme::themeModeChanged, this, [=](Fluent::ThemeMode theme) {
        setWindowEffect(windowEffect());
    });

    auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/fluent_window.qss");
    StyleSheetManager::instance()->registerWidget(styleSource, this);
}


void FluentWidget::setWindowButtonHint(WindowButtonHint hint, bool isEnable)
{
#if USE_QWINDOWKIT
    _windowBar->setWindowButtonHint(hint, isEnable);
#endif
}

void FluentWidget::setWindowButtonHints(WindowButtonHints hints)
{
#if USE_QWINDOWKIT
    _windowBar->setWindowButtonHints(hints);
#endif
}

WindowButtonHints FluentWidget::windowButtonHints() const
{
#if USE_QWINDOWKIT
    return _windowBar->windowButtonHints();
#else
    return WindowButtonHint::None;
#endif
}

void FluentWidget::setWindowEffect(WindowEffect effect)
{
    _windowDisplayMode = effect;

#if USE_QWINDOWKIT
    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(_windowAgent);
    if (agent == nullptr) {
        return;
    }
    bool dark = Theme::instance()->isDarkTheme();
    _windowBar->themeButton()->setChecked(!dark);

    QStringList names = {"none", "dwm-blur", "acrylic-material", "mica", "mica-alt"};
    foreach (QString name, names) {
        agent->setWindowAttribute(name, false);
    }
    const QString data = names.at(static_cast<int>(effect) % names.size());

    agent->setWindowAttribute("dark-mode", dark);
    if (data == QStringLiteral("none")) {
        setProperty("custom-style", false);
    } else if (!data.isEmpty()) {
        agent->setWindowAttribute(data, true);
        setProperty("custom-style", true);
    }
    style()->polish(this);
#else
#ifdef _WIN32
    QStringList names = {"none", "dwm-blur", "acrylic-material", "mica", "mica-alt"};
    const QString data = names.at(static_cast<int>(effect) % names.size());
    if (data == QStringLiteral("none") || data == QStringLiteral("acrylic-material")) {
        setProperty("custom-style", false);
    } else if (!data.isEmpty()) {
        setProperty("custom-style", true);
    }
    style()->polish(this);

    int themeMode = Theme::instance()->isDarkTheme() ? 1 : 0;

    HWND hwnd = reinterpret_cast<HWND>(this->winId());

    int useImmersiveDarkMode = themeMode; // 0 for light theme, 1 for dark theme
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useImmersiveDarkMode, sizeof(useImmersiveDarkMode));

    switch (effect) {
    case WindowEffect::Normal:
    {
        int backdropType = 0; // DWMSBT_DISABLE 或 1 (DWMSBT_AUTO)
        DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));

        // 设置主题颜色: 0 - 浅色, 1 - 深色


        // 取消边框扩展,恢复正常客户区
        MARGINS margins = {0, 0, 0, 0};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
        return;
    }
    case WindowEffect::Acrylic:
    {
        DWM_SYSTEMBACKDROP_TYPE backdropType = DWMSBT_NONE;  // 设置为NONE禁用效果

        // 取消边框扩展,恢复正常客户区
        MARGINS margins = {0, 0, 0, 0};
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));
        return;

    }

    case WindowEffect::DWMBlur:
    {
        int backdropType = 3;
        DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        break;
    }

    case WindowEffect::Mica:
    {
        int backdropType = 2;
        DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        break;
    }

    case WindowEffect::MicaAlt:
    {
        int backdropType = 4;
        DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        break;
    }

    default:
        return;
    }

    // 对于其他效果,启用边框扩展
    MARGINS margins = {-1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

#endif
#endif
}

WindowEffect FluentWidget::windowEffect() const
{
    return _windowDisplayMode;
}

FluentTitleBar *FluentWidget::titleBar() const
{
#if USE_QWINDOWKIT
    return _windowBar;
#else
    return nullptr;
#endif
}

void FluentWidget::setHitTestVisible(QWidget *w, bool visible)
{
#if USE_QWINDOWKIT
    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(_windowAgent);
    agent->setHitTestVisible(w, visible);
#endif
}


void FluentWidget::resizeEvent(QResizeEvent *e)
{
#if USE_QWINDOWKIT
    _windowBar->setGeometry(0, 0, window()->width(), _windowBar->height());
    _windowBar->raise();
#endif
    QWidget::resizeEvent(e);
}

void FluentWidget::setDarkTheme(bool dark) {
#if USE_QWINDOWKIT
    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(_windowAgent);
    if (agent == nullptr) {
        return;
    }
    agent->setWindowAttribute("dark-mode", dark);
#endif

    Theme::instance()->setTheme(dark ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);
}
