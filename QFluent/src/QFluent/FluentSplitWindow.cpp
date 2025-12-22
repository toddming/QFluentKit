#include "FluentSplitWindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QFile>

#include "Theme.h"
#include "StyleSheet.h"
#include "StackedWidget.h"
#include "FluentTitleBar.h"
#include "navigation/NavigationPanel.h"
#include "Private/FluentSplitWindowPrivate.h"
#include "QWKWidgets/widgetwindowagent.h"

FluentSplitWindow::FluentSplitWindow(QWidget *parent)
    : QMainWindow(parent)
    , d_ptr(new FluentSplitWindowPrivate)
{
    Q_D(FluentSplitWindow);
    d->q_ptr = this;

    setObjectName("FluentWindow");

    setAttribute(Qt::WA_DontCreateNativeAncestors);


    QWK::WidgetWindowAgent *agent = new QWK::WidgetWindowAgent(this);
    agent->setup(this);

    d->_windowBar = new FluentTitleBar(this);
    d->_windowBar->setHostWidget(this);

    setWindowButtonHints(Fluent::WindowButtonHint::Icon | Fluent::WindowButtonHint::Title |
                         Fluent::WindowButtonHint::Minimize | Fluent::WindowButtonHint::Maximize |
                         Fluent::WindowButtonHint::Close | Fluent::WindowButtonHint::ThemeToggle);


    agent->setTitleBar(d->_windowBar);
    agent->setHitTestVisible(d->_windowBar->themeButton(), true);
    agent->setHitTestVisible(d->_windowBar->backButton(), true);
    agent->setSystemButton(QWK::WindowAgentBase::Minimize, d->_windowBar->minButton());
    agent->setSystemButton(QWK::WindowAgentBase::Maximize, d->_windowBar->maxButton());
    agent->setSystemButton(QWK::WindowAgentBase::Close, d->_windowBar->closeButton());

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

    d->_userWidget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(d->_userWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    d->_navPanel = new NavigationPanel(this);
    d->_stacked = new StackedWidget(d->_userWidget);
    d->_stacked->setProperty("noRadius", true);

    auto hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(d->_navPanel);
    hBoxLayout->setSpacing(0);
    hBoxLayout->setContentsMargins(0, 48, 0, 0);

    layout->addLayout(hBoxLayout, 0);
    layout->addWidget(d->_stacked, 1);

    setCentralWidget(d->_userWidget);

    d->setDarkTheme(Theme::instance()->isDarkTheme());
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::FLUENT_WINDOW);

    connect(Theme::instance(), &Theme::themeModeChanged, this, [agent](Fluent::ThemeMode theme) {
        agent->setWindowAttribute("dark-mode", theme == Fluent::ThemeMode::DARK);
    });
}

FluentSplitWindow::~FluentSplitWindow()
{
}


bool FluentSplitWindow::event(QEvent *event) {
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


void FluentSplitWindow::setWindowButtonHint(Fluent::WindowButtonHint hint, bool isEnable)
{
    Q_D(FluentSplitWindow);
    d->_windowBar->setWindowButtonHint(hint, isEnable);
}

void FluentSplitWindow::setWindowButtonHints(Fluent::WindowButtonHints hints)
{
    Q_D(FluentSplitWindow);
    d->_windowBar->setWindowButtonHints(hints);
}

Fluent::WindowButtonHints FluentSplitWindow::windowButtonHints() const
{
    Q_D(const FluentSplitWindow);
    return d->_windowBar->windowButtonHints();
}


void FluentSplitWindow::setWindowEffect(Fluent::WindowEffect effect)
{
    Q_D(FluentSplitWindow);

    QWK::WidgetWindowAgent *agent = qobject_cast<QWK::WidgetWindowAgent *>(d->_windowAgent);
    if (agent == nullptr) {
        return;
    }
    d->_windowDisplayMode = effect;

    bool dark = Theme::instance()->isDarkTheme();
    d->_windowBar->themeButton()->setChecked(!dark);

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
}

Fluent::WindowEffect FluentSplitWindow::windowEffect() const
{
    Q_D(const FluentSplitWindow);
    return d->_windowDisplayMode;
}

void FluentSplitWindow::setCustomWindowIcon(const QPixmap &pixmap, const QSize &size)
{
    Q_D(FluentSplitWindow);

    d->_windowBar->iconLabel()->setPixmap(pixmap);
    d->_windowBar->iconLabel()->setFixedSize(size);
}

NavigationPanel *FluentSplitWindow::navigationInterface() const
{
    Q_D(const FluentSplitWindow);
    return d->_navPanel;
}

void FluentSplitWindow::addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                                   QWidget* widget, bool selectable,
                                   Fluent::NavigationItemPosition position, const QString& tooltip,
                                   const QString& parentRouteKey)
{
    Q_D(FluentSplitWindow);
    d->_navPanel->addItem(routeKey, icon, text, [d, widget](){d->_stacked->setCurrentWidget(widget, false);}, selectable, position, tooltip, parentRouteKey);
    d->_stacked->addWidget(widget);
}

void FluentSplitWindow::resizeEvent(QResizeEvent *e)
{
    Q_D(FluentSplitWindow);
    d->_windowBar->setGeometry(0, 0, window()->width(), d->_windowBar->height());
    d->_windowBar->raise();
    QMainWindow::resizeEvent(e);
}
