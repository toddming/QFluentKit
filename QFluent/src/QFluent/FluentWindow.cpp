#include "FluentWindow.h"

#include <QVBoxLayout>
#include <QStyle>
#include <QFile>

#include "Theme.h"
#include "StyleSheet.h"
#include "StackedWidget.h"
#include "FluentTitleBar.h"
#include "QWKWidgets/widgetwindowagent.h"

FluentWindow::FluentWindow(QMainWindow *parent)
    : QMainWindow(parent)
{
    setObjectName("FluentWindow");

    setAttribute(Qt::WA_DontCreateNativeAncestors);

    QWK::WidgetWindowAgent *agent = new QWK::WidgetWindowAgent(this);
    agent->setup(this);

    auto windowBar = new FluentTitleBar(this);
    windowBar->setHostWidget(this);

    agent->setTitleBar(windowBar);
    agent->setHitTestVisible(windowBar->themeButton(), true);
    agent->setHitTestVisible(windowBar->backButton(), true);
    agent->setSystemButton(QWK::WindowAgentBase::Minimize, windowBar->minButton());
    agent->setSystemButton(QWK::WindowAgentBase::Maximize, windowBar->maxButton());
    agent->setSystemButton(QWK::WindowAgentBase::Close, windowBar->closeButton());

    setMenuWidget(windowBar);

    connect(windowBar, &FluentTitleBar::themeRequested, this, [this, windowBar](bool checked){
        windowBar->themeButton()->setChecked(checked);
        setDarkTheme(!checked);
    });
    connect(windowBar, &FluentTitleBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(windowBar, &FluentTitleBar::maximizeRequested, this, [this](bool max) {
        if (max) {
            showMaximized();
        } else {
            showNormal();
        }
    });
    connect(windowBar, &FluentTitleBar::closeRequested, this, &QWidget::close);
    windowAgent = agent;

    initUI();
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


void FluentWindow::setDarkTheme(bool dark) {

    Theme::instance()->setTheme(dark ? ThemeType::ThemeMode::DARK : ThemeType::ThemeMode::LIGHT);
}

void FluentWindow::initUI()
{
    QWidget *client = new QWidget(this);
    client->setFocusPolicy(Qt::ClickFocus);

    stacked = new StackedWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(client);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->addWidget(stacked);

    setCentralWidget(client);

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::FLUENT_WINDOW);

    setDarkTheme(true);

    resize(800, 600);
}

void FluentWindow::switchTo(QWidget *w)
{
    stacked->setCurrentWidget(w);
}
