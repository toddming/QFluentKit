#include "PushButton.h"
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionButton>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"
#include "Animation.h"
#include "menu/RoundMenu.h"
#include "menu/MenuActionListWidget.h"


PushButton::PushButton(QWidget *parent) :
    QPushButton(parent), m_isPressed(false), m_isHover(false)
{
    initialize();
}

PushButton::PushButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    QPushButton(text, parent), m_isPressed(false), m_isHover(false)
{
    initialize();
    setButtonIcon(iconType);
}

PushButton::PushButton(const QString &fontFamily, QChar iconChar, QWidget *parent) :
    QPushButton(iconChar, parent), m_isPressed(false), m_isHover(false)
{
    initialize();

    QFont font(fontFamily);
    font.setPixelSize(14);
    setFont(font);
}



void PushButton::initialize()
{
    installEventFilter(this);

    m_iconType = FluentIconType::IconType::NONE;

    setProperty("hasIcon", false);

    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::BUTTON);

    setIconSize(QSize(16, 16));

}



void PushButton::setButtonIcon(const FluentIconType::IconType iconType)
{
    m_iconType = iconType;

    // 更新样式属性
    setProperty("hasIcon", iconType != FluentIconType::IconType::NONE);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

FluentIconType::IconType PushButton::buttonIcon() const
{
    return m_iconType;
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
    m_isHover = true;
    update();
    QPushButton::enterEvent(e);
}

void PushButton::leaveEvent(QEvent *e)
{
    m_isHover = false;
    update();
    QPushButton::leaveEvent(e);
}

void PushButton::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);

    if (m_iconType == FluentIconType::IconType::NONE)
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
    FluentIcon(m_iconType).render(painter, rect);
}



PrimaryPushButton::PrimaryPushButton(QWidget *parent) :
    PushButton(parent)
{
    setProperty("hasIcon", false);
}

PrimaryPushButton::PrimaryPushButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    PushButton(text, parent)
{
    setProperty("hasIcon", false);
}





TransparentPushButton::TransparentPushButton(QWidget *parent) :
    PushButton(parent)
{
    setProperty("hasIcon", false);
}

TransparentPushButton::TransparentPushButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    PushButton(text, parent, iconType)
{
    setProperty("hasIcon", iconType != FluentIconType::IconType::NONE);
}

TransparentPushButton::TransparentPushButton(const QString &fontFamily, QChar iconChar, QWidget *parent) :
    PushButton(fontFamily, iconChar, parent)
{
    setProperty("hasIcon", false);
}




HyperlinkButton::HyperlinkButton(QWidget *parent) :
    PushButton(parent)
{
    setProperty("hasIcon", false);
}

HyperlinkButton::HyperlinkButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    PushButton(text, parent, iconType)
{
    setProperty("hasIcon", iconType != FluentIconType::IconType::NONE);
}

void HyperlinkButton::drawIcon(QPainter* painter, const QRectF& rect)
{
    if (isEnabled()) {
        QMap<QString, QString> attrs;
        attrs["fill"] = Theme::instance()->themeColor().name();
        FluentIcon(FluentIconType::LINK).render(painter, rect, ThemeType::AUTO, QList<int>(), attrs);
    } else {
        painter->setOpacity(Theme::instance()->isDarkTheme() ? 0.3628 : 0.36);
    }
}



ToggleButton::ToggleButton(QWidget *parent) :
    PushButton(parent)
{
    setProperty("hasIcon", false);
    postInit();
}

ToggleButton::ToggleButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    PushButton(text, parent, iconType)
{
    setProperty("hasIcon", iconType != FluentIconType::IconType::NONE);
    postInit();
}

ToggleButton::ToggleButton(const QString &fontFamily, QChar iconChar, QWidget *parent) :
    PushButton(fontFamily, iconChar, parent)
{
    setProperty("hasIcon", false);
    postInit();
}

void ToggleButton::drawIcon(QPainter* painter, const QRectF& rect)
{
    if (Theme::instance()->isDarkTheme()) {
        FluentIcon(buttonIcon()).render(painter, rect, isChecked() ? ThemeType::DARK : ThemeType::LIGHT);
    } else {
        FluentIcon(buttonIcon()).render(painter, rect, isChecked() ? ThemeType::LIGHT : ThemeType::DARK);
    }
}


void ToggleButton::postInit()
{
    setCheckable(true);
    setChecked(false);
}





TransparentTogglePushButton::TransparentTogglePushButton(QWidget *parent) :
    ToggleButton(parent)
{
    setProperty("hasIcon", false);
}

TransparentTogglePushButton::TransparentTogglePushButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    ToggleButton(text, parent, iconType)
{
    setProperty("hasIcon", iconType != FluentIconType::IconType::NONE);
}

TransparentTogglePushButton::TransparentTogglePushButton(const QString &fontFamily, QChar iconChar, QWidget *parent) :
    ToggleButton(fontFamily, iconChar, parent)
{
    setProperty("hasIcon", false);
}



DropDownButtonBase::DropDownButtonBase(QWidget* parent)
    : PushButton(parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{

}

DropDownButtonBase::DropDownButtonBase(const QString &text, QWidget *parent, const FluentIconType::IconType iconType)
    : PushButton(text, parent, iconType)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{
}

DropDownButtonBase::~DropDownButtonBase()
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

    // Calculate position for DROP_DOWN
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
        FluentIcon(FluentIconType::ARROW_DOWN).render(painter, rect);
    } else {
        QMap<QString, QString> attrs;
        attrs["fill"] = "#646464";
        FluentIcon(FluentIconType::ARROW_DOWN).render(painter, rect, ThemeType::AUTO, QList<int>(), attrs);
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






DropDownPushButton::DropDownPushButton(QWidget *parent) : DropDownButtonBase(parent)
{

}

DropDownPushButton::DropDownPushButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    DropDownButtonBase(text, parent, iconType)
{

}


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









TransparentDropDownPushButton::TransparentDropDownPushButton(QWidget *parent) : DropDownPushButton(parent)
{

}

TransparentDropDownPushButton::TransparentDropDownPushButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    DropDownPushButton(text, parent, iconType)
{

}





PillPushButton::PillPushButton(QWidget *parent) : ToggleButton(parent)
{

}

PillPushButton::PillPushButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    ToggleButton(text, parent, iconType)
{

}


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
        } else if (isPressed() || isHover()) {
            bgColor = isDark ? QColor(255, 255, 255, 21) : QColor(249, 249, 249, 128);
        } else {
            bgColor = isDark ? QColor(255, 255, 255, 15) : QColor(243, 243, 243, 194);
        }
    } else {
        if (!isEnabled()) {
            bgColor = isDark ? QColor(255, 255, 255, 40) : QColor(0, 0, 0, 55);
        } else if (isPressed()) {
            bgColor =  Theme::instance()->themeColor(isDark ? ThemeType::DARK_2 : ThemeType::LIGHT_3);
        } else if (isHover()) {
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






PrimaryDropDownPushButton::PrimaryDropDownPushButton(QWidget *parent) : DropDownButtonBase(parent)
{

}

PrimaryDropDownPushButton::PrimaryDropDownPushButton(const QString &text, QWidget *parent, const FluentIconType::IconType iconType) :
    DropDownButtonBase(text, parent, iconType)
{

}

void PrimaryDropDownPushButton::paintEvent(QPaintEvent *event)
{
    PushButton::paintEvent(event);
    DropDownButtonBase::paintEvent(event);
}

void PrimaryDropDownPushButton::drawDropDownIcon(QPainter *painter, const QRectF &rect)
{
    FluentIcon(FluentIconType::ARROW_DOWN).render(painter, rect, Theme::instance()->isDarkTheme()? ThemeType::DARK : ThemeType::LIGHT);
}

void PrimaryDropDownPushButton::mouseReleaseEvent(QMouseEvent *e)
{
    DropDownButtonBase::mouseReleaseEvent(e);
    showMenu();
}

void PrimaryDropDownPushButton::drawIcon(QPainter *painter, const QRectF &rect)
{
    FluentIcon(buttonIcon()).render(painter, rect, Theme::instance()->isDarkTheme()? ThemeType::DARK : ThemeType::LIGHT);
}



