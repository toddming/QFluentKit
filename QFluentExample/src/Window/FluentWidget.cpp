#include "FluentWidget.h"

#include "Theme.h"
#include "StyleSheet.h"

#if USE_QWINDOWKIT
#include <QEvent>
#include <QStyle>
#include <QLabel>
#include "QWKWidgets/widgetwindowagent.h"
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
#endif
}

WindowEffect FluentWidget::windowEffect() const
{
#if USE_QWINDOWKIT
    return _windowDisplayMode;
#else
    return WindowEffect::Normal;
#endif
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
