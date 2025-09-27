#include "pushbutton.h"
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionButton>

#include "Icon.h"
#include "menu/MenuActionListWidget.h"


PushButton::PushButton(QWidget *parent) :
    QPushButton(parent), m_isPressed(false), m_isHover(false)
{
    initialize();
}

PushButton::PushButton(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType) :
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

    m_iconType = IconType::FLuentIcon::NONE;

    setProperty("hasIcon", false);

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::BUTTON);

    setIconSize(QSize(16, 16));

}



void PushButton::setButtonIcon(const IconType::FLuentIcon iconType)
{
    m_iconType = iconType;

    // 更新样式属性
    setProperty("hasIcon", iconType != IconType::FLuentIcon::NONE);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

IconType::FLuentIcon PushButton::buttonIcon() const
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

    if (m_iconType == IconType::FLuentIcon::NONE)
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

    Icon::drawSvgIcon(&painter, m_iconType, QRectF(x, y, w, h));
    // 绘制图标
}

// void PushButton::drawIcon(QPainter *painter, const QRectF &rect, QIcon::Mode mode)
// {

// }





PrimaryPushButton::PrimaryPushButton(QWidget *parent) :
    PushButton(parent)
{
    setProperty("hasIcon", false);
}

PrimaryPushButton::PrimaryPushButton(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType) :
    PushButton(text, parent)
{
    setProperty("hasIcon", false);
}





TransparentPushButton::TransparentPushButton(QWidget *parent) :
    PushButton(parent)
{
    setProperty("hasIcon", false);
}

TransparentPushButton::TransparentPushButton(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType) :
    PushButton(text, parent, iconType)
{
    setProperty("hasIcon", iconType != IconType::FLuentIcon::NONE);
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

HyperlinkButton::HyperlinkButton(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType) :
    PushButton(text, parent)
{
    setProperty("hasIcon", iconType != IconType::FLuentIcon::NONE);
}



ToggleButton::ToggleButton(QWidget *parent) :
    PushButton(parent)
{
    setProperty("hasIcon", false);
    postInit();
}

ToggleButton::ToggleButton(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType) :
    PushButton(text, parent)
{
    setProperty("hasIcon", iconType != IconType::FLuentIcon::NONE);
    postInit();
}

ToggleButton::ToggleButton(const QString &fontFamily, QChar iconChar, QWidget *parent) :
    PushButton(fontFamily, iconChar, parent)
{
    setProperty("hasIcon", false);
    postInit();
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

TransparentTogglePushButton::TransparentTogglePushButton(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType) :
    ToggleButton(text, parent)
{
    setProperty("hasIcon", iconType != IconType::FLuentIcon::NONE);
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

DropDownButtonBase::DropDownButtonBase(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType)
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
    if (Theme::instance()->isDarkMode()) {
        Icon::drawSvgIcon(painter, IconType::FLuentIcon::ARROW_DOWN, rect);
    } else {
        QMap<QString, QString> attrs;
        attrs["fill"] = "#646464";
        Icon::drawSvgIcon(painter, IconType::FLuentIcon::ARROW_DOWN, rect, attrs);
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

DropDownPushButton::DropDownPushButton(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType) :
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

TransparentDropDownPushButton::TransparentDropDownPushButton(const QString &text, QWidget *parent, const IconType::FLuentIcon iconType) :
    DropDownPushButton(text, parent, iconType)
{

}
