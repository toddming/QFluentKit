#include "PushButton.h"
#include "FluentIcon.h"
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionButton>
#include <QEnterEvent>
#include <QHBoxLayout>
#include <QPointer>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"
#include "Animation.h"
#include "ToolButton.h"
#include "Menu/RoundMenu.h"
#include "Menu/MenuActionListWidget.h"

// PushButton
PushButton::PushButton(QWidget *parent)
    : QPushButton(parent)
    , m_isPressed(false)
    , m_isHover(false)
{
    init();
}

PushButton::PushButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
    , m_isPressed(false)
    , m_isHover(false)
{
    init();
}

PushButton::PushButton(const QString &text, const QIcon &icon, QWidget *parent)
    : PushButton(text, parent)
{
    setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}

PushButton::PushButton(const QString &text, Fluent::IconType type, QWidget *parent)
    : PushButton(text, parent)
{
    setIcon(type);
}

void PushButton::init()
{
    installEventFilter(this);

    StyleSheet::registerWidget(this, Fluent::ThemeStyle::BUTTON);

    setProperty("hasIcon", false);

    setIconSize(QSize(16, 16));
}

bool PushButton::isPressed() const
{
    return m_isPressed;
}

bool PushButton::isHover() const
{
    return m_isHover;
}

void PushButton::mousePressEvent(QMouseEvent *event)
{
    m_isPressed = true;
    QPushButton::mousePressEvent(event);
}

void PushButton::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    QPushButton::mouseReleaseEvent(event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void PushButton::enterEvent(QEnterEvent *event)
{
    m_isHover = true;
    update();
    QPushButton::enterEvent(event);
}
#else
void PushButton::enterEvent(QEvent *event)
{
    m_isHover = true;
    update();
    QPushButton::enterEvent(event);
}
#endif

void PushButton::leaveEvent(QEvent *event)
{
    m_isHover = false;
    update();
    QPushButton::leaveEvent(event);
}

void PushButton::paintEvent(QPaintEvent *event)
{
    // Temporarily clear the icon so QPushButton::paintEvent doesn't draw it
    // (we draw it ourselves via drawIcon with custom opacity)
    QIcon savedIcon;
    if (!icon().isNull()) {
        savedIcon = icon();
        QPushButton::setIcon(QIcon());
    }

    QPushButton::paintEvent(event);

    if (!savedIcon.isNull()) {
        QPushButton::setIcon(savedIcon);
    }

    if (savedIcon.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (!isEnabled()) {
        painter.setOpacity(0.3628);
    } else if (m_isPressed) {
        painter.setOpacity(0.786);
    }

    int w = iconSize().width();
    int h = iconSize().height();
    int y = (height() - h) / 2;
    int mw = minimumSizeHint().width();
    int x = (mw > 0) ? 12 + (width() - mw) / 2 : 12;

    // RTL布局支持
    if (isRightToLeft()) {
        x = width() - w - x;
    }

    drawIcon(&painter, QRectF(x, y, w, h));
}

void PushButton::drawIcon(QPainter* painter, const QRectF& rect)
{
    icon().paint(painter, rect.toRect());
}

void PushButton::setIcon(Fluent::IconType type)
{
    m_fluentIcon = FluentQIcon(type);
    QPushButton::setIcon(m_fluentIcon);
    setProperty("hasIcon", true);
}

void PushButton::setIcon(const QIcon &icon)
{
    m_fluentIcon = FluentQIcon(icon);
    QPushButton::setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}


// PrimaryPushButton
PrimaryPushButton::PrimaryPushButton(QWidget *parent)
    : PushButton(parent)
{

}

PrimaryPushButton::PrimaryPushButton(const QString &text, QWidget *parent)
    : PushButton(text, parent)
{

}

PrimaryPushButton::PrimaryPushButton(const QString &text, Fluent::IconType type, QWidget *parent)
    : PushButton(text, parent)
{
    setIcon(type);
}

PrimaryPushButton::PrimaryPushButton(const QString &text, const QIcon &icon, QWidget *parent)
    : PushButton(text, parent)
{
    setIcon(icon);
}

void PrimaryPushButton::setIcon(Fluent::IconType type)
{
    m_fluentIcon = FluentQIcon(type);
    PushButton::setIcon(m_fluentIcon);
    setProperty("hasIcon", true);
}

void PrimaryPushButton::setIcon(const QIcon &icon)
{
    m_fluentIcon = FluentQIcon(icon);
    PushButton::setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}

void PrimaryPushButton::drawIcon(QPainter* painter, const QRectF& rect)
{
    if (m_fluentIcon.hasType()) {
        m_fluentIcon.reversed().paint(painter, rect.toRect());
    } else {
        icon().paint(painter, rect.toRect());
    }
}



// HyperlinkButton
void HyperlinkButton::drawIcon(QPainter* painter, const QRectF& rect)
{
    if (!isEnabled()) {
        painter->setOpacity(Theme::isDark() ? 0.3628 : 0.36);
    }
    icon().paint(painter, rect.toRect());
}


// ToggleButton
ToggleButton::ToggleButton(QWidget *parent)
    : PushButton(parent)
{
    setCheckable(true);
    setChecked(false);
}

ToggleButton::ToggleButton(const QString &text, QWidget *parent)
    : PushButton(text, parent)
{
    setCheckable(true);
    setChecked(false);
}

ToggleButton::ToggleButton(const QString &text, const QIcon &icon, QWidget *parent)
    : PushButton(text, parent)
{
    setCheckable(true);
    setChecked(false);
    setIcon(icon);
}

// 新增：IconType 构造
ToggleButton::ToggleButton(Fluent::IconType type, QWidget *parent)
    : PushButton(parent)
{
    setCheckable(true);
    setChecked(false);
    setIcon(type);
}

ToggleButton::ToggleButton(const QString &text, Fluent::IconType type, QWidget *parent)
    : PushButton(text, parent)
{
    setCheckable(true);
    setChecked(false);
    setIcon(type);
}

void ToggleButton::setIcon(Fluent::IconType type)
{
    m_fluentIcon = FluentQIcon(type);
    PushButton::setIcon(m_fluentIcon);
    setProperty("hasIcon", true);
}

void ToggleButton::setIcon(const QIcon &icon)
{
    m_fluentIcon = FluentQIcon(icon);
    PushButton::setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}

void ToggleButton::drawIcon(QPainter* painter, const QRectF& rect)
{
    if (m_fluentIcon.hasType()) {
        (isChecked() ? m_fluentIcon.reversed() : m_fluentIcon).paint(painter, rect.toRect());
    } else {
        icon().paint(painter, rect.toRect());
    }
}


// DropDownButtonBase
DropDownButtonBase::DropDownButtonBase(QWidget *parent)
    : PushButton(parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{
}

DropDownButtonBase::DropDownButtonBase(const QString &text, QWidget *parent)
    : PushButton(text, parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{
}

DropDownButtonBase::DropDownButtonBase(const QString &text, const QIcon &icon, QWidget *parent)
    : PushButton(text, icon, parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{
}

DropDownButtonBase::DropDownButtonBase(const QString &text, Fluent::IconType type, QWidget *parent)
    : PushButton(text, type, parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{
}

void DropDownButtonBase::setMenu(RoundMenu* menu)
{
    m_menu = menu;
}

RoundMenu* DropDownButtonBase::menu() const
{
    return m_menu;
}

void DropDownButtonBase::showMenu()
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

void DropDownButtonBase::hideMenu()
{
    if (!m_menu.isNull()) {
        m_menu->hide();
    }
}

void DropDownButtonBase::drawDropDownIcon(QPainter* painter, const QRectF& rect)
{
    if (Theme::isDark()) {
        FluentIconUtils::drawThemeIcon(Fluent::IconType::ARROW_DOWN, painter, rect);
    } else {
        QHash<QString, QString> attrs;
        attrs["fill"] = "#646464";
        FluentIconUtils::drawIcon(FluentIcon(Fluent::IconType::ARROW_DOWN), painter, rect, Fluent::ThemeMode::AUTO, false, attrs);
    }
}

void DropDownButtonBase::paintEvent(QPaintEvent* /*event*/)
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

void DropDownButtonBase::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        update();
    }
    PushButton::mousePressEvent(event);
}

void DropDownButtonBase::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = false;
        update();
    }
    PushButton::mouseReleaseEvent(event);
}

// DropDownPushButton
void DropDownPushButton::mouseReleaseEvent(QMouseEvent* event)
{
    PushButton::mouseReleaseEvent(event);
    showMenu();
}

void DropDownPushButton::paintEvent(QPaintEvent* event)
{
    PushButton::paintEvent(event);
    DropDownButtonBase::paintEvent(event);
}


// PillPushButton
void PillPushButton::paintEvent(QPaintEvent* event)
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

    ToggleButton::paintEvent(event);
}


// PrimaryDropDownPushButton
PrimaryDropDownPushButton::PrimaryDropDownPushButton(QWidget *parent)
    : DropDownButtonBase(parent)
{
}

PrimaryDropDownPushButton::PrimaryDropDownPushButton(const QString &text, QWidget *parent)
    : DropDownButtonBase(text, parent)
{
}

PrimaryDropDownPushButton::PrimaryDropDownPushButton(const QString &text, Fluent::IconType type, QWidget *parent)
    : DropDownButtonBase(text, parent)
{
    setIcon(type);
}

PrimaryDropDownPushButton::PrimaryDropDownPushButton(const QString &text, const QIcon &icon, QWidget *parent)
    : DropDownButtonBase(text, parent)
{
    setIcon(icon);
}

void PrimaryDropDownPushButton::paintEvent(QPaintEvent *event)
{
    PushButton::paintEvent(event);
    DropDownButtonBase::paintEvent(event);
}

void PrimaryDropDownPushButton::drawDropDownIcon(QPainter *painter, const QRectF &rect)
{
    FluentIconUtils::drawThemeIcon(Fluent::IconType::ARROW_DOWN, painter, rect, Fluent::ThemeMode::AUTO, true);
}

void PrimaryDropDownPushButton::mouseReleaseEvent(QMouseEvent *e)
{
    DropDownButtonBase::mouseReleaseEvent(e);
    showMenu();
}

void PrimaryDropDownPushButton::setIcon(Fluent::IconType type)
{
    m_fluentIcon = FluentQIcon(type);
    DropDownButtonBase::setIcon(m_fluentIcon);
    setProperty("hasIcon", true);
}

void PrimaryDropDownPushButton::setIcon(const QIcon &icon)
{
    m_fluentIcon = FluentQIcon(icon);
    DropDownButtonBase::setIcon(icon);
    setProperty("hasIcon", !icon.isNull());
}

void PrimaryDropDownPushButton::drawIcon(QPainter *painter, const QRectF &rect)
{
    if (m_fluentIcon.hasType()) {
        m_fluentIcon.reversed().paint(painter, rect.toRect());
    } else {
        icon().paint(painter, rect.toRect());
    }
}


// SplitButtonBase
SplitButtonBase::SplitButtonBase(QWidget *parent)
    : QWidget(parent)
{
    m_hBoxLayout = new QHBoxLayout(this);
    m_hBoxLayout->setSpacing(0);
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);

    m_dropButton = new SplitDropButton(this);
    m_hBoxLayout->addWidget(m_dropButton);

    connect(m_dropButton, &ToolButton::clicked, this, &SplitButtonBase::dropDownClicked);
    connect(m_dropButton, &ToolButton::clicked, this, &SplitButtonBase::showFlyout);

    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void SplitButtonBase::setWidget(QWidget *widget)
{
    m_hBoxLayout->insertWidget(0, widget, 1, Qt::AlignLeft);
}

void SplitButtonBase::setDropButton(ToolButton *button)
{
    m_hBoxLayout->removeWidget(m_dropButton);
    m_dropButton->deleteLater();

    m_dropButton = button;
    connect(m_dropButton, &QAbstractButton::clicked, this, &SplitButtonBase::dropDownClicked);
    connect(m_dropButton, &QAbstractButton::clicked, this, &SplitButtonBase::showFlyout);

    m_hBoxLayout->addWidget(button);
}

void SplitButtonBase::setDropIconSize(const QSize &size)
{
    m_dropButton->setIconSize(size);
}

void SplitButtonBase::setFlyout(QWidget *flyout)
{
    m_flyout = flyout;
}

void SplitButtonBase::showFlyout()
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


// SplitPushButton
SplitPushButton::SplitPushButton(QWidget *parent)
    : SplitButtonBase(parent)
{
    init();
}

SplitPushButton::SplitPushButton(const QString &text, QWidget *parent)
    : SplitButtonBase(parent)
{
    init();
    m_button->setText(text);
}

SplitPushButton::SplitPushButton(const QString &text, const QIcon &icon, QWidget *parent)
    : SplitButtonBase(parent)
{
    init();
    m_button->setText(text);
    m_button->setIcon(icon);
    m_button->setProperty("hasIcon", !icon.isNull());
}

SplitPushButton::SplitPushButton(const QString &text, Fluent::IconType type, QWidget *parent)
    : SplitButtonBase(parent)
{
    init();
    m_button->setText(text);
    m_button->setIcon(type);
    m_button->setProperty("hasIcon", true);
}

void SplitPushButton::init()
{
    m_button = new PushButton(this);
    m_button->setObjectName("splitPushButton");
    connect(m_button, &PushButton::clicked, this, &SplitPushButton::clicked);
    setWidget(m_button);
}

QString SplitPushButton::text() const
{
    return m_button->text();
}

void SplitPushButton::setText(const QString &text)
{
    m_button->setText(text);
}

void SplitPushButton::setIconSize(const QSize &size)
{
    m_button->setIconSize(size);
}


// PrimarySplitPushButton
PrimarySplitPushButton::PrimarySplitPushButton(QWidget *parent)
    : SplitButtonBase(parent)
{
    init();
}

PrimarySplitPushButton::PrimarySplitPushButton(const QString &text, QWidget *parent)
    : SplitButtonBase(parent)
{
    init();
    m_button->setText(text);
}

PrimarySplitPushButton::PrimarySplitPushButton(const QString &text, const QIcon &icon, QWidget *parent)
    : SplitButtonBase(parent)
{
    init();
    m_button->setText(text);
    m_button->setIcon(icon);
    m_button->setProperty("hasIcon", !icon.isNull());
}

PrimarySplitPushButton::PrimarySplitPushButton(const QString &text, Fluent::IconType type, QWidget *parent)
    : SplitButtonBase(parent)
{
    init();
    m_button->setText(text);
    m_button->setIcon(type);
    m_button->setProperty("hasIcon", true);
}

void PrimarySplitPushButton::init()
{
    setDropButton(new PrimarySplitDropButton(this));

    m_button = new PrimaryPushButton(this);
    m_button->setObjectName("primarySplitPushButton");
    connect(m_button, &PrimaryPushButton::clicked, this, &PrimarySplitPushButton::clicked);
    setWidget(m_button);
}

QString PrimarySplitPushButton::text() const
{
    return m_button->text();
}

void PrimarySplitPushButton::setText(const QString &text)
{
    m_button->setText(text);
}

void PrimarySplitPushButton::setIconSize(const QSize &size)
{
    m_button->setIconSize(size);
}
