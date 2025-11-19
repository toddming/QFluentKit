#include "FluentWidget.h"

#include <QEvent>
#include <QStyle>
#include <QLabel>

#include "Theme.h"
#include "StyleSheet.h"
#include "FluentTitleBar.h"
#include "Private/FluentWidgetPrivate.h"
#include "QWKWidgets/widgetwindowagent.h"


FluentWidget::FluentWidget(QMainWindow *parent)
    : QMainWindow(parent)
    , d_ptr(new FluentWidgetPrivate)
{
    Q_D(FluentWidget);
    d->q_ptr = this;

    setObjectName("FluentWindow");

    setAttribute(Qt::WA_DontCreateNativeAncestors);

    QWK::WidgetWindowAgent *agent = new QWK::WidgetWindowAgent(this);
    agent->setup(this);

    d->_windowBar = new FluentTitleBar(this);
    d->_windowBar->setHostWidget(this);

    agent->setTitleBar(d->_windowBar);
    agent->setHitTestVisible(d->_windowBar->themeButton(), true);
    agent->setHitTestVisible(d->_windowBar->backButton(), true);
    agent->setSystemButton(QWK::WindowAgentBase::Minimize, d->_windowBar->minButton());
    agent->setSystemButton(QWK::WindowAgentBase::Maximize, d->_windowBar->maxButton());
    agent->setSystemButton(QWK::WindowAgentBase::Close, d->_windowBar->closeButton());

    setMenuWidget(d->_windowBar);

    connect(d->_windowBar, &FluentTitleBar::themeRequested, this, [d](bool checked){
        d->_windowBar->themeButton()->setChecked(checked);
        d->setDarkTheme(!checked);
    });
    connect(d->_windowBar, &FluentTitleBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(d->_windowBar, &FluentTitleBar::maximizeRequested, this, [this](bool max) {
        if (max) {
            showMaximized();
        } else {
            showNormal();
        }
    });
    connect(d->_windowBar, &FluentTitleBar::closeRequested, this, &QWidget::close);
    d->_windowAgent = agent;

    d->setDarkTheme(Theme::instance()->isDarkTheme());
    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::FLUENT_WINDOW);

    connect(Theme::instance(), &Theme::themeModeChanged, this, [agent](ThemeType::ThemeMode theme) {
        agent->setWindowAttribute("dark-mode", theme == ThemeType::DARK);
    });
}

FluentWidget::~FluentWidget()
{
}

bool FluentWidget::event(QEvent *event) {
    switch (event->type()) {
    case QEvent::WindowActivate: {
        auto menu = menuWidget();
        if (menu) {
            menu->setProperty("bar-active", true);
            style()->polish(menu);
        }
        break;
    }

    case QEvent::WindowDeactivate: {
        auto menu = menuWidget();
        if (menu) {
            menu->setProperty("bar-active", false);
            style()->polish(menu);
        }
        break;
    }

    default:
        break;
    }
    return QMainWindow::event(event);
}


void FluentWidget::setWindowButtonFlag(AppBarType::ButtonType buttonFlag, bool isEnable)
{
    Q_D(FluentWidget);
    d->_windowBar->setWindowButtonFlag(buttonFlag, isEnable);
}

void FluentWidget::setWindowButtonFlags(AppBarType::ButtonFlags buttonFlags)
{
    Q_D(FluentWidget);
    d->_windowBar->setWindowButtonFlags(buttonFlags);
}

AppBarType::ButtonFlags FluentWidget::getWindowButtonFlags() const
{
    Q_D(const FluentWidget);
    return d->_windowBar->getWindowButtonFlags();
}


void FluentWidget::setWindowDisplayMode(ApplicationType::WindowDisplayMode windowDisplayType)
{
    Q_D(FluentWidget);

    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(d->_windowAgent);
    if (agent == nullptr) {
        return;
    }
    bool dark = Theme::instance()->isDarkTheme();
    d->_windowBar->themeButton()->setChecked(!dark);

    QStringList names = {"none", "dwm-blur", "acrylic-material", "mica", "mica-alt"};
    foreach (QString name, names) {
        agent->setWindowAttribute(name, false);
    }
    const QString data = names.at(static_cast<int>(windowDisplayType) % names.size());

    agent->setWindowAttribute("dark-mode", dark);
    if (data == QStringLiteral("none")) {
        setProperty("custom-style", false);
    } else if (!data.isEmpty()) {
        agent->setWindowAttribute(data, true);
        setProperty("custom-style", true);
    }
    style()->polish(this);
}

ApplicationType::WindowDisplayMode FluentWidget::windowDisplayMode() const
{
    Q_D(const FluentWidget);
    return d->_windowDisplayMode;
}

void FluentWidget::setCustomWindowIcon(const QPixmap &pixmap, const QSize &size)
{
    Q_D(FluentWidget);

    d->_windowBar->iconLabel()->setPixmap(pixmap);
    d->_windowBar->iconLabel()->setFixedSize(size);
}
