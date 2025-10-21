#include "FluentTitleBar.h"

#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtGui/QtEvents>
#include <QHBoxLayout>
#include <QTimer>
#include <QLabel>
#include <QPushButton>

#include "Theme.h"
#include "Icon.h"
#include "StyleSheet.h"

#include "Private/FluentTitleBarPrivate.h"

static inline void emulateLeaveEvent(QWidget *widget) {
    Q_ASSERT(widget);
    if (!widget) {
        return;
    }
    QTimer::singleShot(0, widget, [widget]() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QScreen *screen = widget->screen();
#else
        const QScreen *screen = widget->windowHandle()->screen();
#endif
        const QPoint globalPos = QCursor::pos(screen);
        if (!QRect(widget->mapToGlobal(QPoint{0, 0}), widget->size()).contains(globalPos)) {
            QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));
            if (widget->testAttribute(Qt::WA_Hover)) {
                const QPoint localPos = widget->mapFromGlobal(globalPos);
                const QPoint scenePos = widget->window()->mapFromGlobal(globalPos);
                static constexpr const auto oldPos = QPoint{};
                const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
                const auto event =
                        new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
                Q_UNUSED(localPos);
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#else
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#endif
                QCoreApplication::postEvent(widget, event);
            }
        }
    });
}

FluentTitleBar::FluentTitleBar(QWidget *parent)
    : QFrame(parent)
    , d_ptr(new FluentTitleBarPrivate())
{
    Q_D(FluentTitleBar);
    d->q_ptr = this;

    setFixedHeight(48);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setContentsMargins(5, 0, 0, 0);
    hBoxLayout->setSpacing(0);

    d->_titleLabel  = new QLabel(this);
    d->_iconLabel   = new QLabel(this);
    d->_backButton  = new QPushButton(this);
    d->_iconButton  = new QPushButton(this);
    d->_themeButton = new QPushButton(this);
    d->_minButton   = new QPushButton(this);
    d->_maxButton   = new QPushButton(this);
    d->_closeButton = new QPushButton(this);
    d->_maxButton->setCheckable(true);
    d->_themeButton->setCheckable(true);
    d->_backButton->hide();
    d->_iconLabel->setScaledContents(true);

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

    hBoxLayout->addWidget(d->_backButton);
    hBoxLayout->addWidget(d->_iconLabel);
    hBoxLayout->addWidget(d->_iconButton);
    hBoxLayout->addWidget(d->_titleLabel);
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(d->_themeButton, 0, Qt::AlignTop);
    hBoxLayout->addWidget(d->_minButton, 0, Qt::AlignTop);
    hBoxLayout->addWidget(d->_maxButton, 0, Qt::AlignTop);
    hBoxLayout->addWidget(d->_closeButton, 0, Qt::AlignTop);

    d->_iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::TITLE_BAR);

    const QString fillPath = ":/res/images/window_bar/%1_%2.svg";
    d->_backButton->setIcon(Icon::FluentIcon(IconType::FLuentIcon::LEFT_ARROW));
    d->_themeButton->setIcon(Icon::SvgIcon(fillPath, "theme", "light", "dark"));
    d->_minButton->setIcon(Icon::SvgIcon(fillPath, "minimize", "light", "dark"));
    d->_maxButton->setIcon(Icon::SvgIcon(fillPath, "maximize", "light", "dark"));
    d->_closeButton->setIcon(Icon::SvgIcon(fillPath, "close", "light", "dark"));

    connect(d->_maxButton, &QAbstractButton::clicked, this, [=](bool max) {
        d->_maxButton->setIcon(Icon::SvgIcon(fillPath, max ? "restore" : "maximize", "light", "dark"));
        emulateLeaveEvent(d->_maxButton);
    });
}


FluentTitleBar::~FluentTitleBar()
{

}


QLabel *FluentTitleBar::titleLabel() const {
    Q_D_CONST(FluentTitleBar);
    return d->_titleLabel;
}

QLabel *FluentTitleBar::iconLabel() const {
    Q_D_CONST(FluentTitleBar);
    return d->_iconLabel;
}

QAbstractButton *FluentTitleBar::iconButton() const {
    Q_D_CONST(FluentTitleBar);
    return d->_iconButton;
}

QAbstractButton *FluentTitleBar::backButton() const {
    Q_D_CONST(FluentTitleBar);
    return d->_backButton;
}

QAbstractButton *FluentTitleBar::themeButton() const {
    Q_D_CONST(FluentTitleBar);
    return d->_themeButton;
}

QAbstractButton *FluentTitleBar::minButton() const {
    Q_D_CONST(FluentTitleBar);
    return d->_minButton;
}

QAbstractButton *FluentTitleBar::maxButton() const {
    Q_D_CONST(FluentTitleBar);
    return d->_maxButton;
}

QAbstractButton *FluentTitleBar::closeButton() const {
    Q_D_CONST(FluentTitleBar);
    return d->_closeButton;
}


QWidget *FluentTitleBar::hostWidget() const {
    Q_D_CONST(FluentTitleBar);
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


bool FluentTitleBar::eventFilter(QObject *obj, QEvent *event) {
    Q_D_CONST(FluentTitleBar);

    auto w = d->_hostWidget;
    if (obj == w) {
        QAbstractButton *iconBtn = iconButton();
        QLabel *label = titleLabel();
        QAbstractButton *maxBtn = maxButton();
        switch (event->type()) {
        case QEvent::WindowIconChange: {
            if (iconBtn) {
                iconBtn->setIcon(w->windowIcon());
                iconChanged(w->windowIcon());
            }
            break;
        }
        case QEvent::WindowTitleChange: {
            if (label) {
                label->setText(w->windowTitle());
                titleChanged(w->windowTitle());
            }
            break;
        }
        case QEvent::WindowStateChange: {
            if (maxBtn) {
                maxBtn->setChecked(w->isMaximized());
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

void FluentTitleBar::setWindowButtonFlag(AppBarType::ButtonType buttonFlag, bool isEnable)
{
    Q_D(FluentTitleBar);
    if (isEnable) {
        setWindowButtonFlags(d->_buttonFlags | buttonFlag);
    } else {
        setWindowButtonFlags(d->_buttonFlags & ~buttonFlag);
    }
}

void FluentTitleBar::setWindowButtonFlags(AppBarType::ButtonFlags buttonFlags)
{
    Q_D(FluentTitleBar);
    d->_buttonFlags = buttonFlags;
    if (d->_buttonFlags.testFlag(AppBarType::NoneButtonHint)) {
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
        d->_backButton->setVisible(d->_buttonFlags.testFlag(AppBarType::RouteBackButtonHint));
        d->_iconButton->setVisible(d->_buttonFlags.testFlag(AppBarType::IconButtonHint));
        d->_iconLabel->setVisible(d->_buttonFlags.testFlag(AppBarType::IconLabelHint));
        d->_titleLabel->setVisible(d->_buttonFlags.testFlag(AppBarType::WindowTitleHint));
        d->_themeButton->setVisible(d->_buttonFlags.testFlag(AppBarType::ThemeChangeButtonHint));
        d->_minButton->setVisible(d->_buttonFlags.testFlag(AppBarType::MinimizeButtonHint));
        d->_maxButton->setVisible(d->_buttonFlags.testFlag(AppBarType::MaximizeButtonHint));
        d->_closeButton->setVisible(d->_buttonFlags.testFlag(AppBarType::CloseButtonHint));
    }
}

AppBarType::ButtonFlags FluentTitleBar::getWindowButtonFlags() const
{
    return d_ptr->_buttonFlags;
}
