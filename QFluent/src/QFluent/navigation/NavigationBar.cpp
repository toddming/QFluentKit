#include "NavigationBar.h"
#include <QApplication>
#include <QStyle>
#include <QCursor>
#include <QPainter>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QFont>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMap>
#include <QIcon>
#include <QColor>

#include "Icon.h"
#include "Theme.h"
#include "StyleSheet.h"

#include "../scrollbar/ScrollBar.h"


IconSlideAnimation::IconSlideAnimation(QWidget* parent)
    : QPropertyAnimation(parent), _offset(0), maxOffset(6) {
    setTargetObject(this);
    setPropertyName("offset");
}

float IconSlideAnimation::getOffset() const {
    return _offset;
}

void IconSlideAnimation::setOffset(float value) {
    _offset = value;
    QWidget *parent = qobject_cast<QWidget*>(this->parent());
    if (parent != nullptr)
        parent->update();
}

void IconSlideAnimation::slideDown() {
    setEndValue(maxOffset);
    setDuration(100);
    start();
}

void IconSlideAnimation::slideUp() {
    setEndValue(0);
    setDuration(100);
    start();
}

QVariant IconSlideAnimation::animateValue(const QVariant& startValue, const QVariant& endValue, float progress) {
    float start = startValue.toFloat();
    float end = endValue.toFloat();
    return QVariant(start + (end - start) * progress);
}

// NavigationBarPushButton 实现
NavigationBarPushButton::NavigationBarPushButton(IconType::FLuentIcon icon, const QString& text, bool isSelectable,
                                              IconType::FLuentIcon selectedIcon, QWidget* parent)
    : NavigationPushButton(icon, text, isSelectable, parent),
      m_iconAni(new IconSlideAnimation(this)),
      _selectedIcon(selectedIcon),
      _isSelectedTextVisible(true),
      lightSelectedColor(QColor()),
      darkSelectedColor(QColor()) {
    setFixedSize(64, 58);

    setIndicatorColor(Theme::instance()->themeColor(), Theme::instance()->themeColor());
    Theme::instance()->setFont(this, 12);
}

void NavigationBarPushButton::setSelectedColor(const QColor& light, const QColor& dark) {
    lightSelectedColor = light;
    darkSelectedColor = dark;
    update();
}

void NavigationBarPushButton::setSelectedIcon(IconType::FLuentIcon icon) {
    _selectedIcon = icon;
    update();
}

void NavigationBarPushButton::setSelectedTextVisible(bool isVisible) {
    _isSelectedTextVisible = isVisible;
    update();
}

void NavigationBarPushButton::setSelected(bool isSelected) {
    NavigationPushButton::setSelected(isSelected);
    if (isSelected) {
        m_iconAni->slideDown();
    } else {
        m_iconAni->slideUp();
    }
}

void NavigationBarPushButton::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);

    _drawBackground(painter);
    _drawIcon(painter);
    _drawText(painter);
}

void NavigationBarPushButton::_drawBackground(QPainter& painter) {
    if (property("isSelected").toBool()) {
        QColor bg = Theme::instance()->isDarkTheme() ? QColor(255, 255, 255, 42) : Qt::white;
        painter.setBrush(bg);
        painter.drawRoundedRect(rect(), 5, 5);
        // draw indicator
        QColor indicatorColor = Theme::instance()->themeColor();
        if (!property("isPressed").toBool()) {
            painter.setBrush(indicatorColor);
            painter.drawRoundedRect(0, 16, 4, 24, 2, 2);
        } else {
            painter.setBrush(indicatorColor);
            painter.drawRoundedRect(0, 19, 4, 18, 2, 2);
        }
    } else if (property("isPressed").toBool() || property("isEnter").toBool()) {
        int c = Theme::instance()->isDarkTheme() ? 255 : 0;
        int alpha = property("isEnter").toBool() ? 9 : 6;
        painter.setBrush(QColor(c, c, c, alpha));
        painter.drawRoundedRect(rect(), 5, 5);
    }
}

void NavigationBarPushButton::_drawIcon(QPainter& painter) {
    if (property("isPressed").toBool() || (!property("isEnter").toBool() && !property("isSelected").toBool())) {
        painter.setOpacity(0.6);
    }
    if (!isEnabled()) {
        painter.setOpacity(0.4);
    }

    QRectF rect;
    if (_isSelectedTextVisible) {
        rect = QRectF(22, 13, 20, 20);
    } else {
        rect = QRectF(22, 13 + m_iconAni->getOffset(), 20, 20);
    }

    IconType::FLuentIcon selectedIcon = _selectedIcon != IconType::FLuentIcon::NONE ? _selectedIcon : fluentButton();
    if (property("isSelected").toBool()) {
        if (selectedIcon != IconType::FLuentIcon::NONE) {
            QMap<QString, QString> attrs;
            attrs["fill"] = Theme::instance()->themeColor().name();
            Icon::drawSvgIcon(&painter, selectedIcon, rect, attrs);
        }
    } else {
        Icon::drawSvgIcon(&painter, fluentButton(), rect);
    }
}

void NavigationBarPushButton::_drawText(QPainter& painter) {
    if (property("isSelected").toBool() && !_isSelectedTextVisible) {
        return;
    }

    QColor textColor = property("isSelected").toBool() ? Theme::instance()->themeColor() : (Theme::instance()->isDarkTheme() ? Qt::white : Qt::black);
    painter.setPen(textColor);
    painter.setFont(font());
    QRect rect(0, 32, width(), 26);
    painter.drawText(rect, Qt::AlignCenter, text());
}

// NavigationBar 实现
NavigationBar::NavigationBar(QWidget* parent)
    : QWidget(parent),
      m_scrollArea(new QScrollArea(this)),
      m_scrollWidget(new QWidget()),
      m_vBoxLayout(new QVBoxLayout(this)),
      m_topLayout(new QVBoxLayout()),
      m_bottomLayout(new QVBoxLayout()),
      m_scrollLayout(new QVBoxLayout(m_scrollWidget)),
      m_expandAni(new QPropertyAnimation(this, "minimumWidth", this)),
      m_expandWidth(160),
      m_isMinimalEnabled(false),
      m_displayMode(NavigationType::NavigationDisplayMode::COMPACT),
      m_isMenuButtonVisible(true),
      m_isReturnButtonVisible(false),
      m_isCollapsible(true),
      m_isAcrylicEnabled(false) {

    __initWidget();
}

NavigationBar::~NavigationBar() {
    delete m_scrollLayout;
    delete m_scrollWidget;
    delete m_scrollArea;
    delete m_topLayout;
    delete m_bottomLayout;
    delete m_vBoxLayout;
    delete m_expandAni;
}

void NavigationBar::__initWidget() {
    this->setAttribute(Qt::WA_StyledBackground);
    this->window()->installEventFilter(this);

    m_scrollWidget->setObjectName("scrollWidget");
    m_scrollArea->setVerticalScrollBar(new ScrollBar(this));
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_scrollWidget);
    m_scrollArea->setWidgetResizable(true);

    m_expandAni->setEasingCurve(QEasingCurve::OutQuad);
    m_expandAni->setDuration(150);

    // Initialize layouts
    m_vBoxLayout->setContentsMargins(0, 5, 0, 5);
    m_topLayout->setContentsMargins(4, 0, 4, 0);
    m_bottomLayout->setContentsMargins(4, 0, 4, 0);
    m_scrollLayout->setContentsMargins(4, 0, 4, 0);

    m_vBoxLayout->setSpacing(4);
    m_topLayout->setSpacing(4);
    m_bottomLayout->setSpacing(4);
    m_scrollLayout->setSpacing(4);

    m_vBoxLayout->addLayout(m_topLayout, 0);
    m_vBoxLayout->addWidget(m_scrollArea, 1);
    m_vBoxLayout->addLayout(m_bottomLayout, 0);

    m_vBoxLayout->setAlignment(Qt::AlignTop);
    m_topLayout->setAlignment(Qt::AlignTop);
    m_scrollLayout->setAlignment(Qt::AlignTop);
    m_bottomLayout->setAlignment(Qt::AlignBottom);

    // Register styles
    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::NAVIGATION_INTERFACE);
    StyleSheetManager::instance()->registerWidget(m_scrollWidget, ThemeType::ThemeStyle::NAVIGATION_INTERFACE);
}

void NavigationBar::__initLayout() {
    // Layouts are initialized in __initWidget()
}

NavigationWidget* NavigationBar::widget(const QString& routeKey) {
    if (!m_items.contains(routeKey)) {
        throw RouteKeyError(QString("`%1` is illegal.").arg(routeKey));
    }
    return m_items[routeKey].widget;
}

void NavigationBar::addItem(const QString& routeKey, IconType::FLuentIcon icon, const QString& text,
                          const std::function<void()>& onClick, bool selectable,
                          IconType::FLuentIcon selectedIcon,
                          NavigationType::NavigationItemPosition position) {
    insertItem(-1, routeKey, icon, text, onClick, selectable, selectedIcon, position);
}

void NavigationBar::addWidget(const QString& routeKey, NavigationWidget* widget,
                            const std::function<void()>& onClick,
                            NavigationType::NavigationItemPosition position) {
    insertWidget(-1, routeKey, widget, onClick, position);
}

void NavigationBar::insertItem(int index, const QString& routeKey, IconType::FLuentIcon icon, const QString& text,
                             const std::function<void()>& onClick, bool selectable,
                             IconType::FLuentIcon selectedIcon,
                             NavigationType::NavigationItemPosition position) {
    if (m_items.contains(routeKey)) {
        return;
    }

    NavigationBarPushButton* w = new NavigationBarPushButton(icon, text, selectable, selectedIcon, this);
    w->setSelectedColor(m_lightSelectedColor, m_darkSelectedColor);
    insertWidget(index, routeKey, w, onClick, position);
}

void NavigationBar::insertWidget(int index, const QString& routeKey, NavigationWidget* widget,
                               const std::function<void()>& onClick,
                               NavigationType::NavigationItemPosition position) {
    if (m_items.contains(routeKey)) {
        return;
    }

    _registerWidget(routeKey, widget, onClick);
    _insertWidgetToLayout(index, widget, position);
}

void NavigationBar::addSeparator(NavigationType::NavigationItemPosition position) {
    insertSeparator(-1, position);
}

void NavigationBar::insertSeparator(int index, NavigationType::NavigationItemPosition position) {
    NavigationSeparator* separator = new NavigationSeparator(this);
    _insertWidgetToLayout(index, separator, position);
}

void NavigationBar::_registerWidget(const QString& routeKey, NavigationWidget* widget, const std::function<void()>& onClick) {
    connect(widget, &NavigationWidget::clicked, this, &NavigationBar::_onWidgetClicked);
    if (onClick) {
        connect(widget, &NavigationWidget::clicked, onClick);
    }
    widget->setProperty("routeKey", routeKey);
    m_items[routeKey] = {routeKey, "", widget};
}

void NavigationBar::_insertWidgetToLayout(int index, NavigationWidget* widget, NavigationType::NavigationItemPosition position) {
    if (position == NavigationType::NavigationItemPosition::TOP) {
        widget->setParent(this);
        m_topLayout->insertWidget(index, widget, 0, Qt::AlignTop);
    } else if (position == NavigationType::NavigationItemPosition::SCROLL) {
        widget->setParent(m_scrollWidget);
        m_scrollLayout->insertWidget(index, widget, 0, Qt::AlignTop);
    } else {
        widget->setParent(this);
        m_bottomLayout->insertWidget(index, widget, 0, Qt::AlignBottom);
    }
    widget->show();
}

void NavigationBar::removeWidget(const QString& routeKey) {
    if (!m_items.contains(routeKey)) {
        return;
    }

    NavigationItem item = m_items.take(routeKey);
    item.widget->deleteLater();
}

void NavigationBar::setCurrentItem(const QString& routeKey) {
    if (!m_items.contains(routeKey)) {
        return;
    }

    for (auto it = m_items.constBegin(); it != m_items.constEnd(); ++it) {
        it.value().widget->setSelected(it.key() == routeKey);
    }
}

void NavigationBar::setFont(const QFont& font) {
    QWidget::setFont(font);
    for (NavigationBarPushButton* button : buttons()) {
        button->setFont(font);
    }
}

void NavigationBar::setSelectedTextVisible(bool isVisible) {
    for (NavigationBarPushButton* button : buttons()) {
        button->setSelectedTextVisible(isVisible);
    }
}

void NavigationBar::setSelectedColor(const QColor& light, const QColor& dark) {
    m_lightSelectedColor = light;
    m_darkSelectedColor = dark;
    for (NavigationBarPushButton* button : buttons()) {
        button->setSelectedColor(light, dark);
    }
}

QList<NavigationBarPushButton*> NavigationBar::buttons() const {
    QList<NavigationBarPushButton*> result;
    for (auto item : m_items) {
        if (NavigationBarPushButton* button = qobject_cast<NavigationBarPushButton*>(item.widget)) {
            result.append(button);
        }
    }
    return result;
}

void NavigationBar::_onWidgetClicked() {
    NavigationWidget* widget = qobject_cast<NavigationWidget*>(sender());
    if (widget->property("isSelectable").toBool()) {
        setCurrentItem(widget->property("routeKey").toString());
    }
}

void NavigationBar::setWidgetCompacted(bool isCompacted) {
    for (auto item : m_items) {
        item.widget->setCompacted(isCompacted);
    }
}

void NavigationBar::paintEvent(QPaintEvent* e) {
    QWidget::paintEvent(e);
}

bool NavigationBar::eventFilter(QObject* obj, QEvent* e) {
    // This is a placeholder for the event filter
    // The Python version had some event filtering for window resizing and mouse clicks
    // We'll implement it later if needed
    return QWidget::eventFilter(obj, e);
}
