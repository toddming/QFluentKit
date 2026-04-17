#include "FluentWidget.h"
#include <QStyle>
#include <QVariant>

#include "Theme.h"
#include "StyleSheet.h"

#if USE_QWINDOWKIT
#include <QEvent>
#include <QLabel>
#include "QWKWidgets/widgetwindowagent.h"
#else
#ifdef _WIN32
#include <dwmapi.h>
#include <QOperatingSystemVersion>
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

    _windowBar->themeButton()->setChecked(Theme::isDark());

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
    bool dark = Theme::isDark();
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
    auto current = QOperatingSystemVersion::current();
    bool isWin11 = (current >= QOperatingSystemVersion(QOperatingSystemVersion::Windows, 10, 0, 22000));
    if (!isWin11) {
        effect = WindowEffect::Normal;
    }
    QStringList names = {"none", "dwm-blur", "acrylic-material", "mica", "mica-alt"};
    const QString data = names.at(static_cast<int>(effect) % names.size());
    if (data == QStringLiteral("none")) { //  || data == QStringLiteral("acrylic-material")
        setProperty("custom-style", false);
    } else if (!data.isEmpty()) {
        setProperty("custom-style", true);
    }
    style()->polish(this);

    HWND hwnd = reinterpret_cast<HWND>(this->winId());

    if (!hwnd) {
        qWarning("Invalid window handle");
        return;
    }

    // 设置暗色模式 (Windows 11, 属性 20: DWMWA_USE_IMMERSIVE_DARK_MODE)
    BOOL darkMode = Theme::isDark() ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttribute(hwnd, 20, &darkMode, sizeof(darkMode));
    if (FAILED(hr)) {
        qDebug("Failed to set dark mode: 0x%08X", hr);
    }

    bool needExtendFrame = true;
    bool needResetColor = true;

    switch (effect) {
    case WindowEffect::Acrylic:
    {
        // Windows 11: Type 3 = 真正的Acrylic效果
        int backdropType = 3; // DWMSBT_TRANSIENTWINDOW
        HRESULT hr = DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        if (FAILED(hr)) {
            qWarning("Failed to set Acrylic: 0x%08X", hr);
        }
        needExtendFrame = true;
        needResetColor = true;
        break;
    }

    case WindowEffect::DWMBlur:
    {
        // Windows 11: 使用 Acrylic (type 3)，因为它最接近模糊效果
        int backdropType = 3; // DWMSBT_TRANSIENTWINDOW
        HRESULT hr = DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        if (FAILED(hr)) {
            qWarning("Failed to set blur: 0x%08X", hr);
        }
        needExtendFrame = true;
        needResetColor = true;
        break;
    }

    case WindowEffect::Mica:
    {
        // 背景类型 2 = Mica (DWMSBT_MAINWINDOW)
        int backdropType = 2;
        HRESULT hr = DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        if (FAILED(hr)) {
            qWarning("Failed to set Mica: 0x%08X", hr);
        }
        needExtendFrame = true;
        needResetColor = true;
        break;
    }

    case WindowEffect::MicaAlt:
    {
        // 背景类型 4 = Mica Alt (DWMSBT_TABBEDWINDOW)
        int backdropType = 4;
        HRESULT hr = DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        if (FAILED(hr)) {
            qWarning("Failed to set Mica Alt: 0x%08X", hr);
        }
        needExtendFrame = true;
        needResetColor = true;
        break;
    }

    case WindowEffect::Normal:
    {
        // 背景类型 1 = None (DWMSBT_NONE - 禁用效果)
        int backdropType = 1;
        HRESULT hr = DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
        if (FAILED(hr)) {
            qWarning("Failed to reset backdrop: 0x%08X", hr);
        }

        // 重置边框
        MARGINS margins = {0, 0, 0, 0};
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        // 设置标题栏颜色 (属性 35: DWMWA_CAPTION_COLOR)
        BOOL isDark = Theme::isDark();
        QColor color(isDark ? "#202020" : "#F0F4F9");
        COLORREF captionColor = RGB(color.red(), color.green(), color.blue());
        hr = DwmSetWindowAttribute(hwnd, 35, &captionColor, sizeof(captionColor));
        if (FAILED(hr)) {
            qDebug("Failed to set caption color: 0x%08X", hr);
        }

        needExtendFrame = false;
        needResetColor = false;
        break;
    }

    default:
        return;
    }

    // 重置标题栏颜色为默认值 (属性 35: DWMWA_CAPTION_COLOR)
    if (needResetColor) {
        COLORREF defaultColor = 0xFFFFFFFF;  // 使用系统默认颜色
        HRESULT hr = DwmSetWindowAttribute(hwnd, 35, &defaultColor, sizeof(defaultColor));
        if (FAILED(hr)) {
            qDebug("Failed to reset caption color: 0x%08X", hr);
        }
    }

    // 扩展边框到客户区
    if (needExtendFrame) {
        MARGINS margins = {-1, -1, -1, -1};
        HRESULT hr = DwmExtendFrameIntoClientArea(hwnd, &margins);
        if (FAILED(hr)) {
            qWarning("Failed to extend frame: 0x%08X", hr);
        }
    }

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
