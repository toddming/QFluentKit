#include "PushButton.h"
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionButton>
#include <QTimer>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"
#include "Animation.h"
#include "menu/RoundMenu.h"
#include "menu/MenuActionListWidget.h"

// PushButton
PushButton::PushButton(QWidget *parent) :
    QPushButton(parent)
{
    init();
}

PushButton::PushButton(const QString &text, QWidget* parent) :
    QPushButton(text, parent)
{
    init();
}

PushButton::PushButton(const QString &text, const FluentIconBase &icon, QWidget* parent) :
    QPushButton(text, parent)
    , m_fluentIcon(icon.clone())
{
    init();
    setProperty("hasIcon", true);
}

void PushButton::init()
{
    installEventFilter(this);

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::BUTTON);

    setProperty("hasIcon", false);

    setIconSize(QSize(16, 16));

    QTimer::singleShot(0, this, [this]() {
        postInit();
    });

}

void PushButton::postInit()
{

}

void PushButton::setFluentIcon(const FluentIconBase &icon)
{
    m_fluentIcon.reset(icon.clone());

    setProperty("hasIcon", m_fluentIcon != nullptr);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

FluentIconBase* PushButton::fluentIcon() const
{
    return m_fluentIcon.get();
}

void PushButton::mousePressEvent(QMouseEvent *e)
{
    m_isPressed = true;
    QPushButton::mousePressEvent(e);
}

void PushButton::mouseReleaseEvent(QMouseEvent *e)
{
    m_isPressed = false;
    QPushButton::mouseReleaseEvent(e);
}

void PushButton::enterEvent(QEnterEvent *e)
{
    m_isHovered = true;
    update();
    QPushButton::enterEvent(e);
}

void PushButton::leaveEvent(QEvent *e)
{
    m_isHovered = false;
    update();
    QPushButton::leaveEvent(e);
}

void PushButton::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);

    if (!m_fluentIcon)
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
    FluentIconUtils::drawIcon(*m_fluentIcon, painter, rect);
}


// HyperlinkButton
void HyperlinkButton::drawIcon(QPainter* painter, const QRectF& rect)
{
    if (isEnabled()) {
        QMap<QString, QString> attrs;
        attrs["fill"] = Theme::instance()->themeColor().name();
        FluentIconUtils::drawIcon(*fluentIcon(), painter, rect, ThemeType::AUTO, QIcon::Off, attrs);
    } else {
        painter->setOpacity(Theme::instance()->isDarkTheme() ? 0.3628 : 0.36);
    }
}


// ToggleButton
void ToggleButton::drawIcon(QPainter* painter, const QRectF& rect)
{
    ThemeType::ThemeMode _theme;
    if (Theme::instance()->isDarkTheme()) {
        _theme = isChecked() ? ThemeType::DARK : ThemeType::LIGHT;
    } else {
        _theme = isChecked() ? ThemeType::LIGHT : ThemeType::DARK;
    }
    FluentIconUtils::drawIcon(*fluentIcon(), painter, rect, _theme);
}


void ToggleButton::postInit()
{
    setCheckable(true);
    setChecked(false);
}



DropDownButtonBase::DropDownButtonBase(QWidget *parent) :
    PushButton(parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{
}

DropDownButtonBase::DropDownButtonBase(const QString &text, QWidget* parent) :
    PushButton(parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{
}

DropDownButtonBase::DropDownButtonBase(const QString &text, const FluentIconBase &icon, QWidget* parent) :
    PushButton(text, parent)
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
    int hd = menu->view()->heightForAnimation(pd, MenuAnimationType::DROP_DOWN);

    QPoint pu = mapToGlobal(QPoint(x, 0));
    int hu = menu->view()->heightForAnimation(pu, MenuAnimationType::PULL_UP);

    if (hd >= hu) {
        menu->view()->adjustSize(pd, MenuAnimationType::DROP_DOWN);
        menu->exec(pd, true, MenuAnimationType::DROP_DOWN);
    } else {
        menu->view()->adjustSize(pu, MenuAnimationType::PULL_UP);
        menu->exec(pu, true, MenuAnimationType::PULL_UP);
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
    if (Theme::instance()->isDarkTheme()) {
        FluentIconUtils::drawIcon(FluentIcon(FluentIconType::ARROW_DOWN), painter, rect);
    } else {
        QMap<QString, QString> attrs;
        attrs["fill"] = "#646464";
        FluentIconUtils::drawIcon(FluentIcon(FluentIconType::ARROW_DOWN), painter, rect, ThemeType::AUTO,  QIcon::Off, attrs);
    }
}

void DropDownButtonBase::paintEvent(QPaintEvent* /*event*/)
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
            bgColor =  Theme::instance()->themeColor(isDark ? ThemeType::DARK_2 : ThemeType::LIGHT_3);
        } else if (isHovered()) {
            bgColor =  Theme::instance()->themeColor(isDark ? ThemeType::DARK_1 : ThemeType::LIGHT_1);
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

    ToggleButton::paintEvent(event);
}


// PrimaryDropDownPushButton
void PrimaryDropDownPushButton::paintEvent(QPaintEvent *event)
{
    PushButton::paintEvent(event);
    DropDownButtonBase::paintEvent(event);
}

void PrimaryDropDownPushButton::drawDropDownIcon(QPainter *painter, const QRectF &rect)
{
    FluentIcon(FluentIconType::ARROW_DOWN).render(painter, rect, Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
}

void PrimaryDropDownPushButton::mouseReleaseEvent(QMouseEvent *e)
{
    DropDownButtonBase::mouseReleaseEvent(e);
    showMenu();
}

void PrimaryDropDownPushButton::drawIcon(QPainter *painter, const QRectF &rect)
{
    ThemeType::ThemeMode _theme = Theme::instance()->isDarkTheme()? ThemeType::DARK : ThemeType::LIGHT;
    FluentIconUtils::drawIcon(*fluentIcon(), painter, rect, _theme);
}



