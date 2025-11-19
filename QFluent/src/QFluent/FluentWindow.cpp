#include "FluentWindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QFile>

#include "Theme.h"
#include "StyleSheet.h"
#include "StackedWidget.h"
#include "FluentTitleBar.h"
#include "navigation/NavigationPanel.h"
#include "Private/FluentWindowPrivate.h"
#include "QWKWidgets/widgetwindowagent.h"

FluentWindow::FluentWindow(QMainWindow *parent)
    : QMainWindow(parent)
    , d_ptr(new FluentWindowPrivate)
{
    Q_D(FluentWindow);
    d->q_ptr = this;

    setObjectName("FluentWindow");

    setAttribute(Qt::WA_DontCreateNativeAncestors);


    QWK::WidgetWindowAgent *agent = new QWK::WidgetWindowAgent(this);
    agent->setup(this);

    d->_windowBar = new FluentTitleBar(this);
    d->_windowBar->setHostWidget(this);

    setWindowButtonFlags(AppBarType::IconButtonHint | AppBarType::WindowTitleHint |
                         AppBarType::MinimizeButtonHint | AppBarType::MaximizeButtonHint |
                         AppBarType::CloseButtonHint | AppBarType::ThemeChangeButtonHint);


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

    QWidget *w = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    d->_navPanel = new NavigationPanel(w);
    d->_stacked = new StackedWidget(w);
    layout->addWidget(d->_navPanel, 0);
    layout->addWidget(d->_stacked, 1);
    setCentralWidget(w);

    d->setDarkTheme(Theme::instance()->isDarkTheme());
    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::FLUENT_WINDOW);

    connect(Theme::instance(), &Theme::themeModeChanged, this, [agent](ThemeType::ThemeMode theme) {
        agent->setWindowAttribute("dark-mode", theme == ThemeType::DARK);
    });
}

FluentWindow::~FluentWindow()
{
}


bool FluentWindow::event(QEvent *event) {
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


void FluentWindow::setWindowButtonFlag(AppBarType::ButtonType buttonFlag, bool isEnable)
{
    Q_D(FluentWindow);
    d->_windowBar->setWindowButtonFlag(buttonFlag, isEnable);
}

void FluentWindow::setWindowButtonFlags(AppBarType::ButtonFlags buttonFlags)
{
    Q_D(FluentWindow);
    d->_windowBar->setWindowButtonFlags(buttonFlags);
}

AppBarType::ButtonFlags FluentWindow::getWindowButtonFlags() const
{
    Q_D(const FluentWindow);
    return d->_windowBar->getWindowButtonFlags();
}


void FluentWindow::setWindowDisplayMode(ApplicationType::WindowDisplayMode windowDisplayType)
{
    Q_D(FluentWindow);

    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(d->_windowAgent);
    if (agent == nullptr) {
        return;
    }
    d->_windowDisplayMode = windowDisplayType;

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

ApplicationType::WindowDisplayMode FluentWindow::windowDisplayMode() const
{
    Q_D(const FluentWindow);
    return d->_windowDisplayMode;
}

void FluentWindow::setCustomWindowIcon(const QPixmap &pixmap, const QSize &size)
{
    Q_D(FluentWindow);

    d->_windowBar->iconLabel()->setPixmap(pixmap);
    d->_windowBar->iconLabel()->setFixedSize(size);
}

NavigationPanel *FluentWindow::navigationInterface() const
{
    Q_D(const FluentWindow);
    return d->_navPanel;
}

void FluentWindow::addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                                   QWidget* widget, bool selectable,
                                   NavigationType::NavigationItemPosition position, const QString& tooltip,
                                   const QString& parentRouteKey)
{
    Q_D(FluentWindow);
    d->_navPanel->addItem(routeKey, icon, text, [d, widget](){d->_stacked->setCurrentWidget(widget, false);}, selectable, position, tooltip, parentRouteKey);
    d->_stacked->addWidget(widget);
}

