#include "FluentTitleBar.h"

#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtGui/QtEvents>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCoreApplication>
#include <QGuiApplication>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"

#include "Private/FluentTitleBarPrivate.h"

static inline void emulateLeaveEvent(QWidget *widget) {
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }

    QMetaObject::invokeMethod(widget, [widget]() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QScreen *screen = widget->screen();
#else
        const QScreen *screen = widget->windowHandle() ? widget->windowHandle()->screen() : nullptr;
#endif
        if (!screen) {
            return;
        }

        const QPoint globalPos = QCursor::pos(screen);
        const QRect widgetRect(widget->mapToGlobal(QPoint{0, 0}), widget->size());

        if (!widgetRect.contains(globalPos)) {
            QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));

            if (widget->testAttribute(Qt::WA_Hover)) {
                const QPoint localPos = widget->mapFromGlobal(globalPos);
                const QPoint scenePos = widget->window() ? widget->window()->mapFromGlobal(globalPos) : localPos;
                static constexpr const auto oldPos = QPoint{};
                const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();

#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
                auto event = new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
                Q_UNUSED(localPos);
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
                auto event = new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#else
                auto event = new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#endif
                QCoreApplication::postEvent(widget, event);
            }
        }
    }, Qt::QueuedConnection);
}

FluentTitleBar::FluentTitleBar(QWidget *parent)
    : QFrame(parent)
    , d_ptr(new FluentTitleBarPrivate)
{
    Q_D(FluentTitleBar);
    d->q_ptr = this;

    setFixedHeight(48);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);

    auto leftLayout = new QHBoxLayout();
    auto rightLayout = new QHBoxLayout();
    leftLayout->setSpacing(5);
    rightLayout->setSpacing(0);
    leftLayout->setContentsMargins(5, 0, 0, 0);

    d->_centerWidget = new QWidget(this);

    hBoxLayout->addLayout(leftLayout, 0);
    hBoxLayout->addWidget(d->_centerWidget, 1);
    hBoxLayout->addLayout(rightLayout, 0);

    d->_titleLabel  = new QLabel(this);
    d->_backButton  = new QPushButton(this);
    d->_iconButton  = new QPushButton(this);
    d->_themeButton = new QPushButton(this);
    d->_minButton   = new QPushButton(this);
    d->_maxButton   = new QPushButton(this);
    d->_closeButton = new QPushButton(this);
    d->_maxButton->setCheckable(true);
    d->_themeButton->setCheckable(true);
    d->_backButton->hide();

    d->_backButton->setFixedSize(40, 36);
    d->_iconButton->setFixedSize(40, 36);
    d->_themeButton->setFixedSize(40, 36);
    d->_minButton->setFixedSize(40, 36);
    d->_maxButton->setFixedSize(40, 36);
    d->_closeButton->setFixedSize(40, 36);

    d->_backButton->setProperty("system-button", true);
    d->_themeButton->setProperty("system-button", true);
    d->_minButton->setProperty("system-button", true);
    d->_maxButton->setProperty("system-button", true);
    d->_closeButton->setProperty("system-button", true);
    d->_iconButton->setProperty("system-button", true);

    d->_backButton->setObjectName("back-button");
    d->_themeButton->setObjectName("theme-button");
    d->_minButton->setObjectName("min-button");
    d->_maxButton->setObjectName("max-button");
    d->_closeButton->setObjectName("close-button");
    d->_titleLabel->setObjectName("win-title-label");
    d->_iconButton->setObjectName("icon-button");
    d->_iconButton->setIconSize(QSize(18, 18));
    d->_closeButton->setIconSize(QSize(12, 12));
    d->_maxButton->setIconSize(QSize(12, 12));
    d->_minButton->setIconSize(QSize(12, 12));
    d->_themeButton->setIconSize(QSize(12, 12));

    leftLayout->addWidget(d->_backButton, 0, Qt::AlignLeft | Qt::AlignHCenter);
    leftLayout->addWidget(d->_iconButton, 0, Qt::AlignLeft | Qt::AlignHCenter);
    leftLayout->addWidget(d->_titleLabel, 0, Qt::AlignLeft | Qt::AlignHCenter);

    rightLayout->addWidget(d->_themeButton, 0, Qt::AlignRight | Qt::AlignTop);
    rightLayout->addWidget(d->_minButton, 0, Qt::AlignRight | Qt::AlignTop);
    rightLayout->addWidget(d->_maxButton, 0, Qt::AlignRight | Qt::AlignTop);
    rightLayout->addWidget(d->_closeButton, 0, Qt::AlignRight | Qt::AlignTop);

    d->_titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    d->_backButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    d->_iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    d->_themeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    d->_minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    d->_maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    d->_closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    connect(d->_backButton,  &QAbstractButton::clicked, this, &FluentTitleBar::backRequested);
    connect(d->_themeButton, &QAbstractButton::clicked, this, &FluentTitleBar::themeRequested);
    connect(d->_minButton,   &QAbstractButton::clicked, this, &FluentTitleBar::minimizeRequested);
    connect(d->_maxButton,   &QAbstractButton::clicked, this, &FluentTitleBar::maximizeRequested);
    connect(d->_closeButton, &QAbstractButton::clicked, this, &FluentTitleBar::closeRequested);

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::TITLE_BAR);

    const QString fillPath = ":/res/images/window_bar/%1_{color}.svg";
    d->_backButton->setIcon(FluentIcon(Fluent::IconType::LEFT_ARROW).qicon());
    d->_themeButton->setIcon(FluentIcon(fillPath.arg("theme")).qicon());
    d->_minButton->setIcon(FluentIcon(fillPath.arg("minimize")).qicon());
    d->_maxButton->setIcon(FluentIcon(fillPath.arg("maximize")).qicon());
    d->_closeButton->setIcon(FluentIcon(fillPath.arg("close")).qicon());

    connect(d->_maxButton, &QAbstractButton::clicked, this, [=](bool max) {
        d->_maxButton->setIcon(FluentIcon(fillPath.arg(max ? "restore" : "maximize")).qicon());
        emulateLeaveEvent(d->_maxButton);
    });
}


FluentTitleBar::~FluentTitleBar()
{

}


QLabel *FluentTitleBar::titleLabel() const {
    Q_D(const FluentTitleBar);
    return d->_titleLabel;
}

QAbstractButton *FluentTitleBar::iconButton() const {
    Q_D(const FluentTitleBar);
    return d->_iconButton;
}

QAbstractButton *FluentTitleBar::backButton() const {
    Q_D(const FluentTitleBar);
    return d->_backButton;
}

QAbstractButton *FluentTitleBar::themeButton() const {
    Q_D(const FluentTitleBar);
    return d->_themeButton;
}

QAbstractButton *FluentTitleBar::minButton() const {
    Q_D(const FluentTitleBar);
    return d->_minButton;
}

QAbstractButton *FluentTitleBar::maxButton() const {
    Q_D(const FluentTitleBar);
    return d->_maxButton;
}

QAbstractButton *FluentTitleBar::closeButton() const {
    Q_D(const FluentTitleBar);
    return d->_closeButton;
}


QWidget *FluentTitleBar::hostWidget() const {
    Q_D(const FluentTitleBar);
    return d->_hostWidget;
}

void FluentTitleBar::setHostWidget(QWidget *w) {
    Q_D(FluentTitleBar);

    if (!d->_hostWidget.isNull()) {
        d->_hostWidget->removeEventFilter(this);
    }

    d->_hostWidget = w;

    if (!d->_hostWidget.isNull()) {
        d->_hostWidget->installEventFilter(this);
    }
}

bool FluentTitleBar::titleFollowWindow() const {
    Q_D(const FluentTitleBar);
    return d->_autoTitle;
}

void FluentTitleBar::setTitleFollowWindow(bool value) {
    Q_D(FluentTitleBar);
    d->_autoTitle = value;
}

bool FluentTitleBar::iconFollowWindow() const {
    Q_D(const FluentTitleBar);
    return d->_autoIcon;
}

void FluentTitleBar::setIconFollowWindow(bool value) {
    Q_D(FluentTitleBar);
    d->_autoIcon = value;
}


bool FluentTitleBar::eventFilter(QObject *obj, QEvent *event) {
    Q_D(const FluentTitleBar);

    auto w = d->_hostWidget;
    if (obj == w) {
        QAbstractButton *iconBtn = iconButton();
        QLabel *label = titleLabel();
        QAbstractButton *maxBtn = maxButton();
        switch (event->type()) {
        case QEvent::WindowIconChange: {
            if (d->_autoIcon && iconBtn) {
                iconBtn->setIcon(w->windowIcon());
                iconChanged(w->windowIcon());
            }
            break;
        }
        case QEvent::WindowTitleChange: {
            if (d->_autoTitle && label) {
                label->setText(w->windowTitle());
                titleChanged(w->windowTitle());
            }
            break;
        }
        case QEvent::WindowStateChange: {
            if (maxBtn) {
                maxBtn->setChecked(w->isMaximized());
                const QString fillPath = ":/res/images/window_bar/%1_{color}.svg";
                maxBtn->setIcon(FluentIcon(fillPath.arg(w->isMaximized() ? "restore" : "maximize")).qicon());
            }
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void FluentTitleBar::titleChanged(const QString &text) {
    Q_UNUSED(text)
}

void FluentTitleBar::iconChanged(const QIcon &icon){Q_UNUSED(icon)}

void FluentTitleBar::setWindowButtonHint(Fluent::WindowButtonHint hint, bool isEnable)
{
    Q_D(FluentTitleBar);
    if (isEnable) {
        setWindowButtonHints(d->_buttonFlags | hint);
    } else {
        setWindowButtonHints(d->_buttonFlags & ~hint);
    }
}

void FluentTitleBar::setWindowButtonHints(Fluent::WindowButtonHints hints)
{
    Q_D(FluentTitleBar);
    d->_buttonFlags = hints;
    if (d->_buttonFlags.testFlag(Fluent::WindowButtonHint::None)) {
        d->_backButton->setVisible(false);
        d->_iconButton->setVisible(false);
        d->_titleLabel->setVisible(false);
        d->_themeButton->setVisible(false);
        d->_minButton->setVisible(false);
        d->_maxButton->setVisible(false);
        d->_closeButton->setVisible(false);
    }
    else
    {
        d->_backButton->setVisible(d->_buttonFlags.testFlag(Fluent::WindowButtonHint::RouteBack));
        d->_iconButton->setVisible(d->_buttonFlags.testFlag(Fluent::WindowButtonHint::Icon));
        d->_titleLabel->setVisible(d->_buttonFlags.testFlag(Fluent::WindowButtonHint::Title));
        d->_themeButton->setVisible(d->_buttonFlags.testFlag(Fluent::WindowButtonHint::ThemeToggle));
        d->_minButton->setVisible(d->_buttonFlags.testFlag(Fluent::WindowButtonHint::Minimize));
        d->_maxButton->setVisible(d->_buttonFlags.testFlag(Fluent::WindowButtonHint::Maximize));
        d->_closeButton->setVisible(d->_buttonFlags.testFlag(Fluent::WindowButtonHint::Close));
    }
}

Fluent::WindowButtonHints FluentTitleBar::windowButtonHints() const
{
    return d_ptr->_buttonFlags;
}

QWidget *FluentTitleBar::centerWidget() const
{
    Q_D(const FluentTitleBar);
    return d->_centerWidget;
}
