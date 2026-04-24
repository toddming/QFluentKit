#include "ToolButton.h"
#include "FluentIcon.h"
#include <QPainter>
#include <QStyleOptionButton>
#include <QApplication>
#include <QSize>
#include <QIcon>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QPointer>
#include <memory>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"
#include "Animation.h"
#include "Menu/RoundMenu.h"
#include "Menu/MenuActionListWidget.h"

// ToolButton
ToolButton::ToolButton(QWidget *parent)
    : QToolButton(parent)
    , m_isPressed(false)
    , m_isHover(false)
{
    init();
}

ToolButton::ToolButton(const QIcon &icon, QWidget *parent)
    : QToolButton(parent)
    , m_isPressed(false)
    , m_isHover(false)
{
    setIcon(icon);
    init();
}

ToolButton::ToolButton(Fluent::IconType type, QWidget *parent)
    : QToolButton(parent)
    , m_isPressed(false)
    , m_isHover(false)
{
    setIcon(type);
    init();
}

void ToolButton::init()
{
    StyleSheet::registerWidget(this, Fluent::ThemeStyle::BUTTON);
}

bool ToolButton::isPressed() const
{
    return m_isPressed;
}

bool ToolButton::isHover() const
{
    return m_isHover;
}

void ToolButton::mousePressEvent(QMouseEvent *event)
{
    m_isPressed = true;
    QToolButton::mousePressEvent(event);
}

void ToolButton::mouseReleaseEvent(QMouseEvent* event)
{
    m_isPressed = false;
    QToolButton::mouseReleaseEvent(event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ToolButton::enterEvent(QEnterEvent* event)
{
    m_isHover = true;
    update();
    QToolButton::enterEvent(event);
}
#else
void ToolButton::enterEvent(QEvent* event)
{
    m_isHover = true;
    update();
    QToolButton::enterEvent(event);
}
#endif

void ToolButton::leaveEvent(QEvent* event)
{
    m_isHover = false;
    update();
    QToolButton::leaveEvent(event);
}

void ToolButton::paintEvent(QPaintEvent* event)
{
    // Temporarily clear the icon so QToolButton::paintEvent doesn't draw it
    // (we draw it ourselves via drawIcon with custom opacity)
    QIcon savedIcon;
    if (!icon().isNull()) {
        savedIcon = icon();
        QToolButton::setIcon(QIcon());
    }

    QToolButton::paintEvent(event);

    if (!savedIcon.isNull()) {
        QToolButton::setIcon(savedIcon);
    }

    if (savedIcon.isNull()) return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (!isEnabled()) {
        painter.setOpacity(0.43);
    } else if (m_isPressed) {
        painter.setOpacity(0.63);
    }

    int w = iconSize().width();
    int h = iconSize().height();
    int y = (height() - h) / 2;
    int x = (width() - w) / 2;

    drawIcon(&painter, QRectF(x, y, w, h));
}


void ToolButton::drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme)
{
    icon().paint(painter, rect.toRect());
}

void ToolButton::setIcon(Fluent::IconType type)
{
    m_fluentIcon = FluentQIcon(type);
    QToolButton::setIcon(m_fluentIcon);
    setProperty("hasIcon", true);
}

void ToolButton::setIcon(const QIcon &icon)
{
    m_fluentIcon = FluentQIcon(icon);
    QToolButton::setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}

// PrimaryToolButton
PrimaryToolButton::PrimaryToolButton(QWidget *parent)
    : ToolButton(parent)
{
}

PrimaryToolButton::PrimaryToolButton(Fluent::IconType type, QWidget *parent)
    : ToolButton(parent)
{
    setIcon(type); // 自动反转
}

PrimaryToolButton::PrimaryToolButton(const QIcon &icon, QWidget *parent)
    : ToolButton(parent)
{
    setIcon(icon);
}

void PrimaryToolButton::setIcon(Fluent::IconType type)
{
    m_fluentIcon = FluentQIcon(type);
    ToolButton::setIcon(m_fluentIcon);
    setProperty("hasIcon", true);
}

void PrimaryToolButton::setIcon(const QIcon &icon)
{
    m_fluentIcon = FluentQIcon(icon);
    ToolButton::setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}

void PrimaryToolButton::drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme)
{
    Q_UNUSED(theme);
    if (m_fluentIcon.hasType()) {
        m_fluentIcon.reversed().paint(painter, rect.toRect());
    } else {
        icon().paint(painter, rect.toRect());
    }
}

// ToggleToolButton
ToggleToolButton::ToggleToolButton(QWidget *parent)
    : ToolButton(parent)
{
    setCheckable(true);
    setChecked(false);
}

ToggleToolButton::ToggleToolButton(const QIcon &icon, QWidget *parent)
    : ToolButton(parent)
{
    setCheckable(true);
    setChecked(false);
    setIcon(icon);
}

// 新增：IconType 构造，自动存储正反两套图标
ToggleToolButton::ToggleToolButton(Fluent::IconType type, QWidget *parent)
    : ToolButton(parent)
{
    setCheckable(true);
    setChecked(false);
    setIcon(type);
}

void ToggleToolButton::setIcon(Fluent::IconType type)
{
    m_fluentIcon = FluentQIcon(type);
    ToolButton::setIcon(m_fluentIcon);
    setProperty("hasIcon", true);
}

void ToggleToolButton::setIcon(const QIcon &icon)
{
    m_fluentIcon = FluentQIcon(icon);
    ToolButton::setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}

void ToggleToolButton::drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme)
{
    Q_UNUSED(theme);
    if (m_fluentIcon.hasType()) {
        (isChecked() ? m_fluentIcon.reversed() : m_fluentIcon).paint(painter, rect.toRect());
    } else {
        icon().paint(painter, rect.toRect());
    }
}

// PillToolButton
void PillToolButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool isDark = Theme::isDark();

    QRect rect;
    QColor borderColor;
    QColor bgColor;

    if (!isChecked()) {
        rect = this->rect().adjusted(1, 1, -1, -1);
        borderColor = isDark ? QColor(255, 255, 255, 18) : QColor(0, 0, 0, 15);

        if (!isEnabled()) {
            bgColor = isDark ? QColor(255, 255, 255, 11) : QColor(249, 249, 249, 75);
        } else if (isPressed() || isHover()) {
            bgColor = isDark ? QColor(255, 255, 255, 21) : QColor(249, 249, 249, 128);
        } else {
            bgColor = isDark ? QColor(255, 255, 255, 15) : QColor(243, 243, 243, 194);
        }
    } else {
        if (!isEnabled()) {
            bgColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
        } else if (isPressed()) {
            bgColor =  Theme::themeColor(isDark ? Fluent::ThemeColor::DARK_2 : Fluent::ThemeColor::LIGHT_3);
        } else if (isHover()) {
            bgColor =  Theme::themeColor(isDark ? Fluent::ThemeColor::DARK_1 : Fluent::ThemeColor::LIGHT_1);
        } else {
            bgColor = Theme::themeColor(Fluent::ThemeColor::PRIMARY);
        }

        borderColor = Qt::transparent;
        rect = this->rect();
    }

    painter.setPen(borderColor);
    painter.setBrush(bgColor);

    int r = rect.height() / 2;
    painter.drawRoundedRect(rect, r, r);

    ToggleToolButton::paintEvent(event);
}

// DropDownToolButtonBase
DropDownToolButtonBase::DropDownToolButtonBase(QWidget *parent)
    : ToolButton(parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{

}

DropDownToolButtonBase::DropDownToolButtonBase(const QIcon &icon, QWidget* parent)
    : ToolButton(icon, parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{

}

DropDownToolButtonBase::DropDownToolButtonBase(Fluent::IconType type, QWidget* parent)
    : ToolButton(type, parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{

}

void DropDownToolButtonBase::setMenu(RoundMenu* menu)
{
    m_menu = menu;
}

RoundMenu* DropDownToolButtonBase::menu() const
{
    return m_menu;
}

void DropDownToolButtonBase::showMenu()
{
    if (m_menu.isNull()) {
        return;
    }

    RoundMenu *menu = m_menu;
    menu->view()->setMinimumWidth(width());
    menu->view()->adjustSize();
    menu->adjustSize();

    int x = - menu->view()->width() / 2 + menu->view()->contentsMargins().left() + width() / 2;

    QPoint pd = mapToGlobal(QPoint(x, height()));
    int hd = menu->view()->heightForAnimation(pd, Fluent::MenuAnimation::DROP_DOWN);

    QPoint pu = mapToGlobal(QPoint(x, 0));
    int hu = menu->view()->heightForAnimation(pu, Fluent::MenuAnimation::PULL_UP);

    if (hd >= hu) {
        menu->view()->adjustSize(pd, Fluent::MenuAnimation::DROP_DOWN);
        menu->exec(pd, true, Fluent::MenuAnimation::DROP_DOWN);
    } else {
        menu->view()->adjustSize(pu, Fluent::MenuAnimation::PULL_UP);
        menu->exec(pu, true, Fluent::MenuAnimation::PULL_UP);
    }
}

void DropDownToolButtonBase::hideMenu()
{
    if (!m_menu.isNull()) {
        m_menu->hide();
    }
}

void DropDownToolButtonBase::drawDropDownIcon(QPainter* painter, const QRectF& rect)
{
    FluentIcon icon(Fluent::IconType::ARROW_DOWN);
    if (Theme::isDark()) {
        FluentIconUtils::drawIcon(icon, painter, rect);
    } else {
        QHash<QString, QString> attrs;
        attrs["fill"] = "#646464";
        FluentIconUtils::drawIcon(icon, painter, rect, Fluent::ThemeMode::AUTO, false, attrs);
    }
}

void DropDownToolButtonBase::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_isHover) {
        painter.setOpacity(0.8);
    } else if (m_isPressed) {
        painter.setOpacity(0.7);
    }

    QRectF rect(width() - 22, height() / 2.0 - 5 + m_arrowAni->y(), 10, 10);

    drawDropDownIcon(&painter, rect);
}

void DropDownToolButtonBase::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        update();
    }
    ToolButton::mousePressEvent(event);
}

void DropDownToolButtonBase::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        update();
    }
    ToolButton::mouseReleaseEvent(event);
}

// DropDownToolButton
void DropDownToolButton::mouseReleaseEvent(QMouseEvent* event)
{
    ToolButton::mouseReleaseEvent(event);
    showMenu();
}

void DropDownToolButton::paintEvent(QPaintEvent* event)
{
    ToolButton::paintEvent(event);
    DropDownToolButtonBase::paintEvent(event);
}

void DropDownToolButton::drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme)
{
    Q_UNUSED(theme);
    QRectF r = rect;
    r.moveLeft(12);
    icon().paint(painter, r.toRect());
}

// PrimaryDropDownToolButton
PrimaryDropDownToolButton::PrimaryDropDownToolButton(QWidget *parent)
    : DropDownToolButton(parent)
{
}

PrimaryDropDownToolButton::PrimaryDropDownToolButton(Fluent::IconType type, QWidget *parent)
    : DropDownToolButton(parent)
{
    setIcon(type);
}

PrimaryDropDownToolButton::PrimaryDropDownToolButton(const QIcon &icon, QWidget *parent)
    : DropDownToolButton(parent)
{
    setIcon(icon);
}

void PrimaryDropDownToolButton::setIcon(Fluent::IconType type)
{
    m_fluentIcon = FluentQIcon(type);
    DropDownToolButton::setIcon(m_fluentIcon);
    setProperty("hasIcon", true);
}

void PrimaryDropDownToolButton::setIcon(const QIcon &icon)
{
    m_fluentIcon = FluentQIcon(icon);
    DropDownToolButton::setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}

void PrimaryDropDownToolButton::drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme)
{
    Q_UNUSED(theme);
    QRectF r = rect;
    r.moveLeft(12);

    if (m_fluentIcon.hasType()) {
        m_fluentIcon.reversed().paint(painter, r.toRect());
    } else {
        icon().paint(painter, r.toRect());
    }
}

void PrimaryDropDownToolButton::drawDropDownIcon(QPainter *painter, const QRectF &rect)
{
    FluentIconUtils::drawThemeIcon(Fluent::IconType::ARROW_DOWN, painter, rect, Fluent::ThemeMode::AUTO, true);
}

// SplitDropButton
SplitDropButton::SplitDropButton(QWidget *parent)
    : ToolButton(parent)
    , m_arrowAni(new TranslateYAnimation(this))
{
    setIcon(Fluent::icon(Fluent::IconType::ARROW_DOWN));
    setIconSize(QSize(10, 10));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

void SplitDropButton::drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme)
{
    QRectF r = rect;
    r.translate(0, m_arrowAni->y());

    if (isPressed()) {
        painter->setOpacity(0.5);
    } else if (isHover()) {
        painter->setOpacity(1);
    } else {
        painter->setOpacity(0.63);
    }
    ToolButton::drawIcon(painter, r, theme);
}


// PrimarySplitDropButton
PrimarySplitDropButton::PrimarySplitDropButton(QWidget *parent)
    : PrimaryToolButton(parent)
    , m_arrowAni(new TranslateYAnimation(this))
{
    setIcon(Fluent::icon(Fluent::IconType::ARROW_DOWN, true));
    setIconSize(QSize(10, 10));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

void PrimarySplitDropButton::drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme)
{
    QRectF r = rect;
    r.translate(0, m_arrowAni->y());

    if (isPressed()) {
        painter->setOpacity(0.5);
    } else if (isHover()) {
        painter->setOpacity(1);
    } else {
        painter->setOpacity(0.63);
    }
    PrimaryToolButton::drawIcon(painter, r, theme);
}


// SplitToolBase
SplitToolBase::SplitToolBase(QWidget *parent)
    : QWidget(parent)
{
    m_hBoxLayout = new QHBoxLayout(this);
    m_hBoxLayout->setSpacing(0);
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);

    m_dropButton = new SplitDropButton(this);
    m_hBoxLayout->addWidget(m_dropButton);

    connect(m_dropButton, &ToolButton::clicked, this, &SplitToolBase::dropDownClicked);
    connect(m_dropButton, &ToolButton::clicked, this, &SplitToolBase::showFlyout);

    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void SplitToolBase::setWidget(QWidget *widget)
{
    m_hBoxLayout->insertWidget(0, widget, 1, Qt::AlignLeft);
}

void SplitToolBase::setDropButton(ToolButton *button)
{
    m_hBoxLayout->removeWidget(m_dropButton);
    m_dropButton->deleteLater();

    m_dropButton = button;
    connect(m_dropButton, &QAbstractButton::clicked, this, &SplitToolBase::dropDownClicked);
    connect(m_dropButton, &QAbstractButton::clicked, this, &SplitToolBase::showFlyout);

    m_hBoxLayout->addWidget(button);
}

void SplitToolBase::setDropIconSize(const QSize &size)
{
    m_dropButton->setIconSize(size);
}

void SplitToolBase::setFlyout(QWidget *flyout)
{
    m_flyout = flyout;
}

void SplitToolBase::showFlyout()
{
    if (!m_flyout) return;

    QWidget* w = m_flyout;

    auto* menu = qobject_cast<RoundMenu*>(w);
    int dx = 0;
    if (menu) {
        menu->view()->setMinimumWidth(width());
        menu->adjustSize();
        dx = menu->layout()->contentsMargins().left();
    }

    // 计算位置
    // x = -w.width()/2 + dx + self.width()/2
    int x_offset = -w->width() / 2 + dx + width() / 2;
    int y_offset = height();

    QPoint globalPos = mapToGlobal(QPoint(x_offset, y_offset));

    // 调用 exec 或 show
    if (menu) {
        menu->exec(globalPos);
    } else {
        w->move(globalPos);
        w->show();
    }
}


// SplitToolButton
SplitToolButton::SplitToolButton(QWidget* parent)
    : SplitToolBase(parent)
{
    init();
}

SplitToolButton::SplitToolButton(const QIcon& icon, QWidget* parent)
    : SplitToolBase(parent)
{
    init();
    m_button->setIcon(icon);
}

SplitToolButton::SplitToolButton(Fluent::IconType type, QWidget* parent)
    : SplitToolBase(parent)
{
    init();
    m_button->setIcon(type);
}

void SplitToolButton::init()
{
    m_button = new ToolButton(this);
    m_button->setObjectName("splitToolButton");
    connect(m_button, &QToolButton::clicked, this, &SplitToolButton::clicked);
    setWidget(m_button);
}

void SplitToolButton::setIconSize(const QSize& size) {
    m_button->setIconSize(size);
}


// PrimarySplitToolButton
PrimarySplitToolButton::PrimarySplitToolButton(QWidget* parent)
    : SplitToolBase(parent)
{
    init();
}

PrimarySplitToolButton::PrimarySplitToolButton(const QIcon& icon, QWidget* parent)
    : SplitToolBase(parent)
{
    init();
    m_button->setIcon(icon);
}

PrimarySplitToolButton::PrimarySplitToolButton(Fluent::IconType type, QWidget* parent)
    : SplitToolBase(parent)
{
    init();
    m_button->setIcon(type);
}

void PrimarySplitToolButton::init()
{
    setDropButton(new PrimarySplitDropButton(this));

    m_button = new PrimaryToolButton(this);
    m_button->setObjectName("primarySplitToolButton");
    connect(m_button, &QToolButton::clicked, this, &PrimarySplitToolButton::clicked);

    setWidget(m_button);
}

void PrimarySplitToolButton::setIconSize(const QSize& size) {
    m_button->setIconSize(size);
}