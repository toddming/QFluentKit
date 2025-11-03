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


ToolButton::ToolButton(QWidget* parent)
    : QToolButton(parent)
    , m_isPressed(false)
    , m_isHover(false)
{
    init();
}

ToolButton::ToolButton(FluentIconType::IconType icon, QWidget* parent)
    : ToolButton(parent)
{
    setIcon(icon);
}

ToolButton::ToolButton(const QString& templatePath, QWidget* parent)
    : ToolButton(parent)
{
    setIcon(templatePath);
}

ToolButton::ToolButton(QIcon icon, QWidget* parent)
    : ToolButton(parent)
{
    QToolButton::setIcon(icon);
}


ToolButton::ToolButton(const FluentIconBase &icon, QWidget* parent)
    : QToolButton(parent)
    , m_icon(icon.clone())
{
    init();
}

void ToolButton::init()
{
    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::BUTTON);

    setIcon(FluentIconType::IconType::NONE);

    QTimer::singleShot(0, this, [this]() {
        postInit();
    });
}

void ToolButton::setIcon(FluentIconType::IconType icon)
{
    m_iconType = icon;
    update();
}

void ToolButton::setIcon(const QString& templatePath)
{
    m_templatePath = templatePath;
    m_iconType = FluentIconType::IconType::NONE;

    update();
}

void ToolButton::setIcon(const FluentIconBase &icon)
{

}

FluentIconBase& ToolButton::fluentIcon() const
{
    return *m_icon;
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

void ToolButton::enterEvent(QEnterEvent* event)
{
    m_isHover = true;
    update();
    QToolButton::enterEvent(event);
}

void ToolButton::leaveEvent(QEvent* event)
{
    m_isHover = false;
    update();
    QToolButton::leaveEvent(event);
}

void ToolButton::paintEvent(QPaintEvent* event)
{
    QToolButton::paintEvent(event);

    if (!m_icon) return;

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

void ToolButton::postInit()
{

}

void ToolButton::drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme)
{
    FluentIconUtils::drawIcon(*m_icon, painter, rect);
}

void PrimaryToolButton::drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme)
{
    if (iconType() != FluentIconType::IconType::NONE) {
        FluentIcon(iconType()).render(painter, rect, Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
    } else {
        FluentIcon(templatePath()).render(painter, rect, Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
    }
}


void ToggleToolButton::drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme)
{
    if (iconType() != FluentIconType::IconType::NONE) {
        if (Theme::instance()->isDarkTheme()) {
            FluentIcon(iconType()).render(painter, rect, isChecked() ? ThemeType::DARK : ThemeType::LIGHT);
        } else {
            FluentIcon(iconType()).render(painter, rect, isChecked() ? ThemeType::LIGHT : ThemeType::DARK);
        }
    } else {
        if (Theme::instance()->isDarkTheme()) {
            FluentIcon(templatePath()).render(painter, rect, isChecked() ? ThemeType::DARK : ThemeType::LIGHT);
        } else {
            FluentIcon(templatePath()).render(painter, rect, isChecked() ? ThemeType::LIGHT : ThemeType::DARK);
        }
    }
}


void ToggleToolButton::postInit()
{
    setCheckable(true);
    setChecked(false);
}


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

    ToggleToolButton::paintEvent(event);
}




DropDownToolButtonBase::DropDownToolButtonBase(QWidget *parent)
    : ToolButton(parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{

}


DropDownToolButtonBase::DropDownToolButtonBase(FluentIconType::IconType icon, QWidget *parent)
    : ToolButton(icon, parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{

}

DropDownToolButtonBase::DropDownToolButtonBase(QIcon icon, QWidget *parent)
    : ToolButton(icon, parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{

}

DropDownToolButtonBase::DropDownToolButtonBase(const QString &templatePath, QWidget *parent)
    : ToolButton(templatePath, parent)
    , m_menu(nullptr)
    , m_arrowAni(new TranslateYAnimation(this))
{

}

DropDownToolButtonBase::~DropDownToolButtonBase()
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

void DropDownToolButtonBase::hideMenu()
{
    if (!m_menu.isNull()) {
        m_menu->hide();
    }
}

void DropDownToolButtonBase::drawDropDownIcon(QPainter* painter, const QRectF& rect)
{
    if (Theme::instance()->isDarkTheme()) {
        FluentIcon(FluentIconType::ARROW_DOWN).render(painter, rect);
    } else {
        QMap<QString, QString> attrs;
        attrs["fill"] = "#646464";
        FluentIcon(FluentIconType::ARROW_DOWN).render(painter, rect, ThemeType::AUTO, QList<int>(), attrs);
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

void DropDownToolButton::drawIcon(QPainter* painter, const QRectF& rect, ThemeType::ThemeMode theme)
{
    QRectF r = rect;
    r.moveLeft(12);
    if (iconType() != FluentIconType::IconType::NONE) {
        FluentIcon(iconType()).render(painter, r, theme);
    } else {
        FluentIcon(templatePath()).render(painter, r, theme);
    }
}

void PrimaryDropDownToolButton::drawIcon(QPainter *painter, const QRectF &rect, ThemeType::ThemeMode theme)
{
    Q_UNUSED(theme);
    QRectF r = rect;
    r.moveLeft(12);
    if (iconType() != FluentIconType::IconType::NONE) {
        FluentIcon(iconType()).render(painter, r, Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
    } else {
        FluentIcon(templatePath()).render(painter, r, Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
    }
}

void PrimaryDropDownToolButton::drawDropDownIcon(QPainter *painter, const QRectF &rect)
{
    FluentIcon(FluentIconType::ARROW_DOWN).render(painter, rect, Theme::instance()->isDarkTheme() ? ThemeType::DARK : ThemeType::LIGHT);
}
