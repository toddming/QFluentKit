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
{
    setFixedHeight(48);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);

    auto leftLayout = new QHBoxLayout();
    auto rightLayout = new QHBoxLayout();
    leftLayout->setSpacing(5);
    rightLayout->setSpacing(0);
    leftLayout->setContentsMargins(5, 0, 0, 0);

    _centerWidget = new QWidget(this);

    hBoxLayout->addLayout(leftLayout, 0);
    hBoxLayout->addWidget(_centerWidget, 1);
    hBoxLayout->addLayout(rightLayout, 0);

    _titleLabel  = new QLabel(this);
    _backButton  = new QPushButton(this);
    _iconButton  = new QPushButton(this);
    _themeButton = new QPushButton(this);
    _minButton   = new QPushButton(this);
    _maxButton   = new QPushButton(this);
    _closeButton = new QPushButton(this);
    _maxButton->setCheckable(true);
    _themeButton->setCheckable(true);
    _backButton->hide();

    _backButton->setFixedSize(40, 36);
    _iconButton->setFixedSize(40, 36);
    _themeButton->setFixedSize(40, 36);
    _minButton->setFixedSize(40, 36);
    _maxButton->setFixedSize(40, 36);
    _closeButton->setFixedSize(40, 36);

    _backButton->setProperty("system-button", true);
    _themeButton->setProperty("system-button", true);
    _minButton->setProperty("system-button", true);
    _maxButton->setProperty("system-button", true);
    _closeButton->setProperty("system-button", true);
    _iconButton->setProperty("system-button", true);

    _backButton->setObjectName("back-button");
    _themeButton->setObjectName("theme-button");
    _minButton->setObjectName("min-button");
    _maxButton->setObjectName("max-button");
    _closeButton->setObjectName("close-button");
    _titleLabel->setObjectName("win-title-label");
    _iconButton->setObjectName("icon-button");
    _iconButton->setIconSize(QSize(18, 18));
    _closeButton->setIconSize(QSize(12, 12));
    _maxButton->setIconSize(QSize(12, 12));
    _minButton->setIconSize(QSize(12, 12));
    _themeButton->setIconSize(QSize(12, 12));

    leftLayout->addWidget(_backButton, 0, Qt::AlignLeft | Qt::AlignHCenter);
    leftLayout->addWidget(_iconButton, 0, Qt::AlignLeft | Qt::AlignHCenter);
    leftLayout->addWidget(_titleLabel, 0, Qt::AlignLeft | Qt::AlignHCenter);

    rightLayout->addWidget(_themeButton, 0, Qt::AlignRight | Qt::AlignTop);
    rightLayout->addWidget(_minButton, 0, Qt::AlignRight | Qt::AlignTop);
    rightLayout->addWidget(_maxButton, 0, Qt::AlignRight | Qt::AlignTop);
    rightLayout->addWidget(_closeButton, 0, Qt::AlignRight | Qt::AlignTop);

    _titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _backButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _themeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    _closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    connect(_backButton,  &QAbstractButton::clicked, this, &FluentTitleBar::backRequested);
    connect(_themeButton, &QAbstractButton::clicked, this, &FluentTitleBar::themeRequested);
    connect(_minButton,   &QAbstractButton::clicked, this, &FluentTitleBar::minimizeRequested);
    connect(_maxButton,   &QAbstractButton::clicked, this, &FluentTitleBar::maximizeRequested);
    connect(_closeButton, &QAbstractButton::clicked, this, &FluentTitleBar::closeRequested);

    StyleSheet::registerWidget(this, ":/res/style/{theme}/title_bar.qss");

    const QString fillPath = ":/res/images/window_bar/%1_{color}.svg";
    _backButton->setIcon(Fluent::icon(Fluent::IconType::LEFT_ARROW));
    _themeButton->setIcon(Fluent::icon(fillPath.arg("theme")));
    _minButton->setIcon(Fluent::icon(fillPath.arg("minimize")));
    _maxButton->setIcon(Fluent::icon(fillPath.arg("maximize")));
    _closeButton->setIcon(Fluent::icon(fillPath.arg("close")));

    connect(_maxButton, &QAbstractButton::clicked, this, [=](bool max) {
        _maxButton->setIcon(Fluent::icon(fillPath.arg(max ? "restore" : "maximize")));
        emulateLeaveEvent(_maxButton);
    });
}


FluentTitleBar::~FluentTitleBar()
{

}


QLabel *FluentTitleBar::titleLabel() const {

    return _titleLabel;
}

QAbstractButton *FluentTitleBar::iconButton() const {

    return _iconButton;
}

QAbstractButton *FluentTitleBar::backButton() const {

    return _backButton;
}

QAbstractButton *FluentTitleBar::themeButton() const {
    return _themeButton;
}

QAbstractButton *FluentTitleBar::minButton() const {
    return _minButton;
}

QAbstractButton *FluentTitleBar::maxButton() const {
    return _maxButton;
}

QAbstractButton *FluentTitleBar::closeButton() const {
    return _closeButton;
}


QWidget *FluentTitleBar::hostWidget() const {
    return _hostWidget;
}

void FluentTitleBar::setHostWidget(QWidget *w) {
    if (!_hostWidget.isNull()) {
        _hostWidget->removeEventFilter(this);
    }

    _hostWidget = w;

    if (!_hostWidget.isNull()) {
        _hostWidget->installEventFilter(this);
    }
}

bool FluentTitleBar::titleFollowWindow() const {
    return _autoTitle;
}

void FluentTitleBar::setTitleFollowWindow(bool value) {
    _autoTitle = value;
}

bool FluentTitleBar::iconFollowWindow() const {
    return _autoIcon;
}

void FluentTitleBar::setIconFollowWindow(bool value) {
    _autoIcon = value;
}


bool FluentTitleBar::eventFilter(QObject *obj, QEvent *event) {
    auto w = _hostWidget;
    if (obj == w) {
        QAbstractButton *iconBtn = iconButton();
        QLabel *label = titleLabel();
        QAbstractButton *maxBtn = maxButton();
        switch (event->type()) {
        case QEvent::WindowIconChange: {
            if (_autoIcon && iconBtn) {
                iconBtn->setIcon(w->windowIcon());
                iconChanged(w->windowIcon());
            }
            break;
        }
        case QEvent::WindowTitleChange: {
            if (_autoTitle && label) {
                label->setText(w->windowTitle());
                titleChanged(w->windowTitle());
            }
            break;
        }
        case QEvent::WindowStateChange: {
            if (maxBtn) {
                maxBtn->setChecked(w->isMaximized());
                const QString fillPath = ":/res/images/window_bar/%1_{color}.svg";
                maxBtn->setIcon(Fluent::icon(fillPath.arg(w->isMaximized() ? "restore" : "maximize")));
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

void FluentTitleBar::setWindowButtonHint(WindowButtonHint hint, bool isEnable)
{

    if (isEnable) {
        setWindowButtonHints(_buttonFlags | hint);
    } else {
        setWindowButtonHints(_buttonFlags & ~hint);
    }
}

void FluentTitleBar::setWindowButtonHints(WindowButtonHints hints)
{

    _buttonFlags = hints;
    if (_buttonFlags.testFlag(WindowButtonHint::None)) {
        _backButton->setVisible(false);
        _iconButton->setVisible(false);
        _titleLabel->setVisible(false);
        _themeButton->setVisible(false);
        _minButton->setVisible(false);
        _maxButton->setVisible(false);
        _closeButton->setVisible(false);
    }
    else
    {
        _backButton->setVisible(_buttonFlags.testFlag(WindowButtonHint::RouteBack));
        _iconButton->setVisible(_buttonFlags.testFlag(WindowButtonHint::WindowIcon));
        _titleLabel->setVisible(_buttonFlags.testFlag(WindowButtonHint::Title));
        _themeButton->setVisible(_buttonFlags.testFlag(WindowButtonHint::ThemeToggle));
        _minButton->setVisible(_buttonFlags.testFlag(WindowButtonHint::Minimize));
        _maxButton->setVisible(_buttonFlags.testFlag(WindowButtonHint::Maximize));
        _closeButton->setVisible(_buttonFlags.testFlag(WindowButtonHint::Close));
    }
}

WindowButtonHints FluentTitleBar::windowButtonHints() const
{
    return _buttonFlags;
}

QWidget *FluentTitleBar::centerWidget() const
{

    return _centerWidget;
}
