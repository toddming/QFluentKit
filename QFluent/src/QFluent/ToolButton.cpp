#include "ToolButton.h"
#include <QPainter>
#include <QStyleOptionButton>
#include <QApplication>
#include <QTimer>
#include <QSize>
#include <QIcon>
#include <QMouseEvent>
#include <QPaintEvent>
#include <memory>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"
#include "Animation.h"
#include "menu/RoundMenu.h"
#include "menu/MenuActionListWidget.h"

// ToolButton
ToolButton::ToolButton(QWidget* parent)
    : QToolButton(parent)
{
    init();
}

ToolButton::ToolButton(const QIcon &icon, QWidget* parent)
    : QToolButton(parent)
{
    setIcon(icon);
    init();
}


ToolButton::ToolButton(const FluentIconBase &icon, QWidget* parent)
    : QToolButton(parent)
    , m_fluentIcon(icon.clone())
{
    init();
}

void ToolButton::init()
{
    m_isPressed = false;
    m_isHovered = false;
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::BUTTON);
}


void ToolButton::setFluentIcon(const FluentIconBase &icon)
{
    m_fluentIcon.reset(icon.clone());
}

FluentIconBase* ToolButton::fluentIcon() const
{
    return m_fluentIcon.get();
}

void ToolButton::mousePressEvent(QMouseEvent* event)
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
    m_isHovered = true;
    update();
    QToolButton::enterEvent(event);
}
#else
void ToolButton::enterEvent(QEvent* event)
{
    m_isHovered = true;
    update();
    QToolButton::enterEvent(event);
}
#endif

void ToolButton::leaveEvent(QEvent* event)
{
    m_isHovered = false;
    update();
    QToolButton::leaveEvent(event);
}

void ToolButton::paintEvent(QPaintEvent* event)
{
    QToolButton::paintEvent(event);

    if (!m_fluentIcon) return;

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
    FluentIconUtils::drawIcon(*m_fluentIcon, painter, rect);
}

// PrimaryToolButton
void PrimaryToolButton::drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme)
{
    Q_UNUSED(theme);
    Fluent::ThemeMode _theme = Theme::instance()->isDarkTheme() ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT;
    FluentIconUtils::drawIcon(*fluentIcon(), painter, rect, _theme);
}

// ToggleToolButton
ToggleToolButton::ToggleToolButton(QWidget* parent)
    : ToolButton(parent)
{
    setCheckable(true);
    setChecked(false);
}

ToggleToolButton::ToggleToolButton(const QIcon &icon, QWidget* parent)
    : ToolButton(icon, parent)
{
    setCheckable(true);
    setChecked(false);
}

ToggleToolButton::ToggleToolButton(const FluentIconBase &icon, QWidget* parent)
    : ToolButton(icon, parent)
{
    setCheckable(true);
    setChecked(false);
}


void ToggleToolButton::drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme)
{
    Fluent::ThemeMode _theme = theme;
    if (Theme::instance()->isDarkTheme()) {
        _theme = isChecked() ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT;
    } else {
        _theme = isChecked() ? Fluent::ThemeMode::LIGHT : Fluent::ThemeMode::DARK;
    }
    FluentIconUtils::drawIcon(*fluentIcon(), painter, rect, _theme);
}

// PillToolButton
void PillToolButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool isDark = Theme::instance()->isDarkTheme();

    QRect rect;
    QColor borderColor;
    QColor bgColor;

    if (!isChecked()) {
        rect = this->rect().adjusted(1, 1, -1, -1);
        borderColor = isDark ? QColor(255, 255, 255, 18) : QColor(0, 0, 0, 15);

        if (!isEnabled()) {
            bgColor = isDark ? QColor(255, 255, 255, 11) : QColor(249, 249, 249, 75);
        } else if (isPressed() || isHovered()) {
            bgColor = isDark ? QColor(255, 255, 255, 21) : QColor(249, 249, 249, 128);
        } else {
            bgColor = isDark ? QColor(255, 255, 255, 15) : QColor(243, 243, 243, 194);
        }
    } else {
        if (!isEnabled()) {
            bgColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
        } else if (isPressed()) {
            bgColor =  Theme::instance()->themeColor(isDark ? Fluent::ThemeColor::DARK_2 : Fluent::ThemeColor::LIGHT_3);
        } else if (isHovered()) {
            bgColor =  Theme::instance()->themeColor(isDark ? Fluent::ThemeColor::DARK_1 : Fluent::ThemeColor::LIGHT_1);
        } else {
            bgColor = Theme::instance()->themeColor();
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

DropDownToolButtonBase::DropDownToolButtonBase(const FluentIconBase &icon, QWidget* parent)
    : ToolButton(icon, parent)
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
    if (Theme::instance()->isDarkTheme()) {
        FluentIconUtils::drawIcon(FluentIcon(Fluent::IconType::ARROW_DOWN), painter, rect);
    } else {
        QMap<QString, QString> attrs;
        attrs["fill"] = "#646464";
        FluentIconUtils::drawIcon(FluentIcon(Fluent::IconType::ARROW_DOWN), painter, rect, Fluent::ThemeMode::AUTO,  QIcon::Off, attrs);
    }
}

void DropDownToolButtonBase::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_isHovered) {
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
    QRectF r = rect;
    r.moveLeft(12);
    FluentIconUtils::drawIcon(*fluentIcon(), painter, r, theme);
}

// PrimaryDropDownToolButton
void PrimaryDropDownToolButton::drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme)
{
    Q_UNUSED(theme);
    QRectF r = rect;
    r.moveLeft(12);

    Fluent::ThemeMode _theme = Theme::instance()->isDarkTheme() ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT;
    FluentIconUtils::drawIcon(*fluentIcon(), painter, r, _theme);
}

void PrimaryDropDownToolButton::drawDropDownIcon(QPainter *painter, const QRectF &rect)
{
    FluentIcon(Fluent::IconType::ARROW_DOWN).render(painter, rect, Theme::instance()->isDarkTheme() ? Fluent::ThemeMode::DARK : Fluent::ThemeMode::LIGHT);
}

// SplitDropButton
SplitDropButton::SplitDropButton(QWidget *parent)
    : ToolButton(parent)
    , m_arrowAni(new TranslateYAnimation(this))
{
    setFluentIcon(FluentIcon(Fluent::IconType::ARROW_DOWN));
    setIconSize(QSize(10, 10));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

void SplitDropButton::drawIcon(QPainter *painter, const QRectF &rect, Fluent::ThemeMode theme)
{
    QRectF r = rect;
    r.translate(0, m_arrowAni->y());

    if (isPressed()) {
        painter->setOpacity(0.5);
    } else if (isHovered()) {
        painter->setOpacity(1);
    } else {
        painter->setOpacity(0.63);
    }
    ToolButton::drawIcon(painter, rect, theme);
}
