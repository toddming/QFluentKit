#include "FluentWidget.h"

#include <QEvent>
#include <QStyle>
#include <QLabel>

#include "Theme.h"
#include "StyleSheet.h"
#include "FluentTitleBar.h"
#include "Private/FluentWidgetPrivate.h"
#include "QWKWidgets/widgetwindowagent.h"


FluentWidget::FluentWidget(QWidget *parent)
    : QWidget(parent)
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
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::FLUENT_WINDOW);

    connect(Theme::instance(), &Theme::themeModeChanged, this, [agent](Fluent::ThemeMode theme) {
        agent->setWindowAttribute("dark-mode", theme == Fluent::ThemeMode::DARK);
    });
}

FluentWidget::~FluentWidget()
{

}

void FluentWidget::setWindowButtonHint(Fluent::WindowButtonHint hint, bool isEnable)
{
    Q_D(FluentWidget);
    d->_windowBar->setWindowButtonHint(hint, isEnable);
}

void FluentWidget::setWindowButtonHints(Fluent::WindowButtonHints hints)
{
    Q_D(FluentWidget);
    d->_windowBar->setWindowButtonHints(hints);
}

Fluent::WindowButtonHints FluentWidget::windowButtonHints() const
{
    Q_D(const FluentWidget);
    return d->_windowBar->windowButtonHints();
}

void FluentWidget::setWindowEffect(Fluent::WindowEffect effect)
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

Fluent::WindowEffect FluentWidget::windowEffect() const
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

void FluentWidget::resizeEvent(QResizeEvent *e)
{
    Q_D(FluentWidget);
    d->_windowBar->setGeometry(0, 0, window()->width(), d->_windowBar->height());
    d->_windowBar->raise();
    QWidget::resizeEvent(e);
}
