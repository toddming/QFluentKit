#include "FluentTitleBar.h"

#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtGui/QtEvents>
#include <QHBoxLayout>
#include <QTimer>

#include "../common/Theme.h"
#include "FluentIcon.h"


FluentTitleBar::FluentTitleBar(QWidget *parent)
    : QFrame(parent) {
    initWidgets();
}


FluentTitleBar::~FluentTitleBar() = default;


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


void FluentTitleBar::initWidgets()
{
    setFixedHeight(48);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setContentsMargins(5, 0, 0, 0);
    hBoxLayout->setSpacing(0);

    m_titleLabel  = new QLabel(this);
    m_backButton  = new QPushButton(this);
    m_iconButton  = new QPushButton(this);
    m_themeButton = new QPushButton(this);
    m_minButton   = new QPushButton(this);
    m_maxButton   = new QPushButton(this);
    m_closeButton = new QPushButton(this);
    m_maxButton->setCheckable(true);
    m_themeButton->setCheckable(true);
    m_backButton->hide();

    m_backButton->setFixedSize(40, 36);
    m_iconButton->setFixedSize(40, 36);
    m_themeButton->setFixedSize(40, 36);
    m_minButton->setFixedSize(40, 36);
    m_maxButton->setFixedSize(40, 36);
    m_closeButton->setFixedSize(40, 36);

    m_backButton->setProperty("system-button", true);
    m_themeButton->setProperty("system-button", true);
    m_minButton->setProperty("system-button", true);
    m_maxButton->setProperty("system-button", true);
    m_closeButton->setProperty("system-button", true);
    m_iconButton->setProperty("system-button", true);

    m_backButton->setObjectName("back-button");
    m_themeButton->setObjectName("theme-button");
    m_minButton->setObjectName("min-button");
    m_maxButton->setObjectName("max-button");
    m_closeButton->setObjectName("close-button");
    m_titleLabel->setObjectName("win-title-label");
    m_iconButton->setObjectName("icon-button");

    hBoxLayout->addWidget(m_backButton);
    hBoxLayout->addWidget(m_iconButton);
    hBoxLayout->addWidget(m_titleLabel);
    hBoxLayout->addWidget(m_themeButton, 0, Qt::AlignTop);
    hBoxLayout->addWidget(m_minButton, 0, Qt::AlignTop);
    hBoxLayout->addWidget(m_maxButton, 0, Qt::AlignTop);
    hBoxLayout->addWidget(m_closeButton, 0, Qt::AlignTop);

    m_titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_backButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_themeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    connect(m_backButton,  &QAbstractButton::clicked, this, &FluentTitleBar::backRequested);
    connect(m_themeButton, &QAbstractButton::clicked, this, &FluentTitleBar::themeRequested);
    connect(m_minButton,   &QAbstractButton::clicked, this, &FluentTitleBar::minimizeRequested);
    connect(m_maxButton,   &QAbstractButton::clicked, this, &FluentTitleBar::maximizeRequested);
    connect(m_closeButton, &QAbstractButton::clicked, this, &FluentTitleBar::closeRequested);

    sTheme->registerWidget(this, Theme::StyleSheetType::TITLE_BAR);

    const QString fillPath = ":/res/images/window_bar/%1_%2.svg";
    m_backButton->setIcon(Icon::FluentIcon(Icon::IconType::LEFT_ARROW));
    m_themeButton->setIcon(Icon::SvgIcon(fillPath, "theme", "light", "dark"));
    m_minButton->setIcon(Icon::SvgIcon(fillPath, "minimize", "light", "dark"));
    m_maxButton->setIcon(Icon::SvgIcon(fillPath, "maximize", "light", "dark"));
    m_closeButton->setIcon(Icon::SvgIcon(fillPath, "close", "light", "dark"));

    connect(m_maxButton, &QAbstractButton::clicked, this, [=](bool max) {
        m_maxButton->setIcon(Icon::SvgIcon(fillPath, max ? "restore" : "maximize", "light", "dark"));
        emulateLeaveEvent(m_maxButton);
    });

}


QLabel *FluentTitleBar::titleLabel() const {
    return m_titleLabel;
}

QAbstractButton *FluentTitleBar::iconButton() const {
    return m_iconButton;
}

QAbstractButton *FluentTitleBar::backButton() const {
    return m_backButton;
}

QAbstractButton *FluentTitleBar::themeButton() const {
    return m_themeButton;
}

QAbstractButton *FluentTitleBar::minButton() const {
    return m_minButton;
}

QAbstractButton *FluentTitleBar::maxButton() const {
    return m_maxButton;
}

QAbstractButton *FluentTitleBar::closeButton() const {
    return m_closeButton;
}


QWidget *FluentTitleBar::hostWidget() const {
    return m_hostWidget;
}

void FluentTitleBar::setHostWidget(QWidget *w) {
    if (!m_hostWidget.isNull()) {
        m_hostWidget->removeEventFilter(this);
    }

    m_hostWidget = w;

    if (!m_hostWidget.isNull()) {
        m_hostWidget->installEventFilter(this);
    }
}


bool FluentTitleBar::eventFilter(QObject *obj, QEvent *event) {
    auto w = m_hostWidget;
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

