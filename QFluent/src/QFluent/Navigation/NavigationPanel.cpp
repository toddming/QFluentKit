#include "NavigationPanel.h"

#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QStyle>

#include "FluentIcon.h"
#include "StyleSheet.h"
#include "NavigationWidget.h"
#include "NavigationToolTip.h"
#include "QFluent/Flyout.h"
#include "QFluent/ScrollArea.h"

// 常量定义
namespace {
    constexpr int DEFAULT_COMPACT_WIDTH = 48;
    constexpr int DEFAULT_EXPAND_WIDTH = 160;
    constexpr int MIN_EXPAND_WIDTH = 42;
    constexpr int MENU_WIDTH_OFFSET = 15;
    constexpr int ANIMATION_DURATION = 185;
    constexpr int DEFAULT_MIN_EXPAND_WIDTH = 1008;
    constexpr int LAYOUT_TOP_MARGIN = 5;
    constexpr int LAYOUT_BOTTOM_MARGIN = 5;
    constexpr int LAYOUT_SIDE_MARGIN = 4;
    constexpr int LAYOUT_SPACING = 4;
    constexpr int MIN_WINDOW_TOP_OFFSET = 42;
}

NavigationPanel::NavigationPanel(QWidget* parent, bool isMinimalEnabled)
    : QFrame(parent)
    , m_parentWidget(parent)
    , m_isMenuButtonVisible(true)
    , m_isReturnButtonVisible(false)
    , m_isCollapsible(true)
    , m_isAcrylicEnabled(false)
    , m_isMinimalEnabled(isMinimalEnabled)
    , m_minimumExpandWidth(DEFAULT_MIN_EXPAND_WIDTH)
    , m_expandWidth(DEFAULT_EXPAND_WIDTH)
    , m_scrollArea(nullptr)
    , m_scrollWidget(nullptr)
    , m_menuButton(nullptr)
    , m_mainLayout(nullptr)
    , m_topLayout(nullptr)
    , m_bottomLayout(nullptr)
    , m_scrollLayout(nullptr)
    , m_expandAnimation(nullptr)
    , m_displayMode(isMinimalEnabled ? NavigationPanel::DisplayMode::MINIMAL
                                     : NavigationPanel::DisplayMode::COMPACT)
{
    initWidget();
}

NavigationPanel::~NavigationPanel()
{
}

void NavigationPanel::initWidget()
{
    setFixedWidth(DEFAULT_COMPACT_WIDTH);
    setAttribute(Qt::WA_StyledBackground);

    if (window()) {
        window()->installEventFilter(this);
    }

    // 创建滚动区域
    m_scrollArea = new ScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setViewportMargins(0, 0, 0, 20);

    m_scrollWidget = new QWidget();
    m_scrollWidget->setObjectName(QStringLiteral("scrollWidget"));
    m_scrollArea->setWidget(m_scrollWidget);

    // 创建菜单按钮
    m_menuButton = new NavigationToolButton(Fluent::icon(Fluent::IconType::MENU), this);
    connect(m_menuButton, &NavigationToolButton::clicked,
            this, &NavigationPanel::toggle);

    // 创建展开动画
    m_expandAnimation = new QPropertyAnimation(this, "minimumWidth", this);
    m_expandAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_expandAnimation->setDuration(ANIMATION_DURATION);
    connect(m_expandAnimation, &QPropertyAnimation::finished,
            this, &NavigationPanel::onExpandAnimationFinished);

    // 设置样式
    setProperty("menu", false);
    StyleSheet::registerWidget(this, Fluent::ThemeStyle::NAVIGATION_INTERFACE);
    StyleSheet::registerWidget(m_scrollWidget, Fluent::ThemeStyle::NAVIGATION_INTERFACE);

    initLayout();
}

void NavigationPanel::initLayout()
{
    m_mainLayout = new NavigationItemLayout(this);
    m_topLayout = new QVBoxLayout();
    m_bottomLayout = new QVBoxLayout();
    m_scrollLayout = new QVBoxLayout(m_scrollWidget);

    // 设置边距
    m_mainLayout->setContentsMargins(0, LAYOUT_TOP_MARGIN, 0, LAYOUT_BOTTOM_MARGIN);
    m_topLayout->setContentsMargins(LAYOUT_SIDE_MARGIN, 0, LAYOUT_SIDE_MARGIN, 0);
    m_bottomLayout->setContentsMargins(LAYOUT_SIDE_MARGIN, 0, LAYOUT_SIDE_MARGIN, 0);
    m_scrollLayout->setContentsMargins(LAYOUT_SIDE_MARGIN, 0, LAYOUT_SIDE_MARGIN, 0);

    // 设置间距
    m_mainLayout->setSpacing(LAYOUT_SPACING);
    m_topLayout->setSpacing(LAYOUT_SPACING);
    m_bottomLayout->setSpacing(LAYOUT_SPACING);
    m_scrollLayout->setSpacing(LAYOUT_SPACING);

    // 组装布局
    m_mainLayout->addLayout(m_topLayout, 0);
    m_mainLayout->addWidget(m_scrollArea, 1);
    m_mainLayout->addLayout(m_bottomLayout, 0);

    // 设置对齐
    m_mainLayout->setAlignment(Qt::AlignTop);
    m_topLayout->setAlignment(Qt::AlignTop);
    m_scrollLayout->setAlignment(Qt::AlignTop);
    m_bottomLayout->setAlignment(Qt::AlignBottom);

    // 添加菜单按钮
    m_topLayout->addWidget(m_menuButton, 0, Qt::AlignTop);
}

void NavigationPanel::updateAcrylicColor()
{
    // 预留亚克力效果实现
    // TODO: 实现亚克力背景效果
}

NavigationWidget* NavigationPanel::widget(const QString& routeKey)
{
    if (!m_items.contains(routeKey)) {
        qWarning("NavigationPanel::widget: Route key '%s' is not registered.", qPrintable(routeKey));
        return nullptr;
    }

    return m_items[routeKey].widget;
}

NavigationUserCard* NavigationPanel::addUserCard(
    const QString& routeKey,
    const QVariant& avatar,
    const QString& title,
    const QString& subtitle,
    std::function<void()> onClick,
    NavigationPanel::ItemPosition position,
    bool aboveMenuButton)
{
    Q_UNUSED(routeKey);

    auto* card = new NavigationUserCard(this);
    card->setExpandWidth(m_expandWidth);

    // 设置头像
    if (!avatar.isNull()) {
        if (avatar.canConvert<QIcon>()) {
            card->setAvatar(avatar.value<QIcon>());
        } else if (avatar.canConvert<QString>()) {
            card->setAvatar(avatar.value<QString>());
        }
    }

    card->setTitle(title);
    card->setSubtitle(subtitle);

    // 确定插入位置
    int index = -1;
    if (aboveMenuButton && position == NavigationPanel::ItemPosition::TOP) {
        for (int i = 0; i < m_topLayout->count(); ++i) {
            QLayoutItem* item = m_topLayout->itemAt(i);
            if (item && item->widget() == m_menuButton) {
                index = i;
                break;
            }
        }
    }

    // 插入或添加
    if (index >= 0) {
        m_topLayout->insertWidget(index, card);
    } else {
        m_topLayout->addWidget(card);
    }

    // 连接点击信号
    if (onClick) {
        connect(card, &NavigationUserCard::clicked, this, onClick);
    }

    return card;
}

void NavigationPanel::addItem(
    const QString& routeKey,
    const QIcon& icon,
    const QString& text,
    const std::function<void()>& onClick,
    bool selectable,
    NavigationPanel::ItemPosition position,
    const QString& tooltip,
    const QString& parentRouteKey)
{
    insertItem(-1, routeKey, icon, text, onClick, selectable, position, tooltip, parentRouteKey);
}

void NavigationPanel::addWidget(
    const QString& routeKey,
    NavigationWidget* widget,
    const std::function<void()>& onClick,
    NavigationPanel::ItemPosition position,
    const QString& tooltip,
    const QString& parentRouteKey)
{
    insertWidget(-1, routeKey, widget, onClick, position, tooltip, parentRouteKey);
}

void NavigationPanel::insertItem(
    int index,
    const QString& routeKey,
    const QIcon& icon,
    const QString& text,
    const std::function<void()>& onClick,
    bool selectable,
    NavigationPanel::ItemPosition position,
    const QString& tooltip,
    const QString& parentRouteKey)
{
    if (m_items.contains(routeKey)) {
        qWarning("Route key '%s' already exists.", qPrintable(routeKey));
        return;
    }

    auto* treeWidget = new NavigationTreeWidget(text, icon, selectable, this);
    treeWidget->setExpandWidth(m_expandWidth);
    insertWidget(index, routeKey, treeWidget, onClick, position, tooltip, parentRouteKey);
}

void NavigationPanel::insertWidget(
    int index,
    const QString& routeKey,
    NavigationWidget* widget,
    const std::function<void()>& onClick,
    NavigationPanel::ItemPosition position,
    const QString& tooltip,
    const QString& parentRouteKey)
{
    if (!widget) {
        qWarning("Cannot insert null widget.");
        return;
    }

    if (m_items.contains(routeKey)) {
        qWarning("Route key '%s' already exists.", qPrintable(routeKey));
        return;
    }

    widget->setExpandWidth(m_expandWidth);
    registerWidget(routeKey, parentRouteKey, widget, onClick, tooltip);

    if (!parentRouteKey.isEmpty() && m_items.contains(parentRouteKey)) {
        this->widget(parentRouteKey)->insertChild(index, widget);
    } else {
        insertWidgetToLayout(index, widget, position);
    }
}

void NavigationPanel::addSeparator(NavigationPanel::ItemPosition position)
{
    insertSeparator(-1, position);
}

void NavigationPanel::addItemHeader(const QString& text, NavigationPanel::ItemPosition position)
{
    insertItemHeader(-1, text, position);
}

void NavigationPanel::insertSeparator(int index, NavigationPanel::ItemPosition position)
{
    auto* separator = new NavigationSeparator(this);
    separator->setExpandWidth(m_expandWidth);
    insertWidgetToLayout(index, separator, position);
}

void NavigationPanel::insertItemHeader(int index, const QString& text, NavigationPanel::ItemPosition position)
{
    auto* header = new NavigationItemHeader(text, this);
    insertWidgetToLayout(index, header, position);
}

void NavigationPanel::registerWidget(
    const QString& routeKey,
    const QString& parentRouteKey,
    NavigationWidget* widget,
    const std::function<void()>& onClick,
    const QString& tooltip)
{
    if (!widget) {
        return;
    }

    connect(widget, &NavigationWidget::clicked, this, &NavigationPanel::onWidgetClicked);

    if (onClick) {
        connect(widget, &NavigationWidget::clicked, this, onClick);
    }

    widget->setProperty("routeKey", routeKey);
    widget->setProperty("parentRouteKey", parentRouteKey);

    if (!tooltip.isEmpty()) {
        widget->setToolTip(tooltip);
        // 避免重复安装 tooltip 事件过滤器
        if (!widget->property("hasToolTipFilter").toBool()) {
            widget->installEventFilter(new NavigationToolTipFilter(widget, 1000));
            widget->setProperty("hasToolTipFilter", true);
        }
    }

    m_items[routeKey] = NavigationItem(routeKey, parentRouteKey, widget);

    if (m_displayMode == NavigationPanel::DisplayMode::EXPAND ||
        m_displayMode == NavigationPanel::DisplayMode::MENU) {
        widget->setCompacted(false);
    }
}

void NavigationPanel::insertWidgetToLayout(
    int index,
    NavigationWidget* widget,
    NavigationPanel::ItemPosition position)
{
    if (!widget) {
        return;
    }

    switch (position) {
        case NavigationPanel::ItemPosition::TOP:
            widget->setParent(this);
            m_topLayout->insertWidget(index, widget, 0, Qt::AlignTop);
            break;

        case NavigationPanel::ItemPosition::SCROLL:
            widget->setParent(m_scrollWidget);
            m_scrollLayout->insertWidget(index, widget, 0, Qt::AlignTop);
            break;

        case NavigationPanel::ItemPosition::BOTTOM:
            widget->setParent(this);
            m_bottomLayout->insertWidget(index, widget, 0, Qt::AlignBottom);
            break;
    }

    widget->show();
}

void NavigationPanel::removeWidget(const QString& routeKey)
{
    if (!m_items.contains(routeKey)) {
        return;
    }

    NavigationItem item = m_items.take(routeKey);

    // 处理树形部件的子项
    if (auto* treeWidget = qobject_cast<NavigationTreeWidgetBase*>(item.widget)) {
        QList<NavigationWidget*> children = treeWidget->findChildren<NavigationWidget*>(
            QString(), Qt::FindChildrenRecursively);

        for (NavigationWidget* child : children) {
            QString childKey = child->property("routeKey").toString();
            if (!childKey.isEmpty() && m_items.contains(childKey)) {
                m_items.remove(childKey);
            }
        }
    }

    if (item.widget) {
        item.widget->deleteLater();
    }
}

void NavigationPanel::setCurrentItem(const QString& routeKey)
{
    if (!m_items.contains(routeKey)) {
        return;
    }

    for (auto it = m_items.constBegin(); it != m_items.constEnd(); ++it) {
        if (it.value().widget) {
            it.value().widget->setSelected(it.key() == routeKey);
        }
    }
}

void NavigationPanel::setMenuButtonVisible(bool isVisible)
{
    m_isMenuButtonVisible = isVisible;
    if (m_menuButton) {
        m_menuButton->setVisible(isVisible);
    }
}

void NavigationPanel::setReturnButtonVisible(bool isVisible)
{
    m_isReturnButtonVisible = isVisible;
    // TODO: 实现返回按钮
}

void NavigationPanel::setCollapsible(bool on)
{
    m_isCollapsible = on;
    if (!on && m_displayMode != NavigationPanel::DisplayMode::EXPAND) {
        expand(false);
    }
}

void NavigationPanel::setExpandWidth(int width)
{
    if (width <= MIN_EXPAND_WIDTH) {
        qWarning("Expand width must be greater than %d", MIN_EXPAND_WIDTH);
        return;
    }

    m_expandWidth = width;
}

void NavigationPanel::setAcrylicEnabled(bool isEnabled)
{
    if (isEnabled == m_isAcrylicEnabled) {
        return;
    }

    m_isAcrylicEnabled = isEnabled;
    setProperty("transparent", canDrawAcrylic());
    style()->unpolish(this);
    style()->polish(this);
    update();
}

bool NavigationPanel::isAcrylicEnabled() const
{
    return m_isAcrylicEnabled;
}

void NavigationPanel::expand(bool useAnimation)
{
    setWidgetCompacted(false);
    m_expandAnimation->setProperty("expand", true);

    if (!m_isMinimalEnabled || !m_isCollapsible) {
        m_displayMode = NavigationPanel::DisplayMode::EXPAND;
    } else {
        setProperty("menu", true);
        style()->unpolish(this);
        style()->polish(this);
        m_displayMode = NavigationPanel::DisplayMode::MENU;

        // 从父窗口分离为独立窗口
        QWidget* parent = m_parentWidget;
        if (parent && !parent->isWindow()) {
            QPoint pos = parent->pos();
            setParent(window());
            move(pos);
        }

        show();
    }

    if (useAnimation) {
        emit displayModeChanged(m_displayMode);
        m_expandAnimation->setStartValue(DEFAULT_COMPACT_WIDTH);
        m_expandAnimation->setEndValue(m_expandWidth + MENU_WIDTH_OFFSET);
        m_expandAnimation->start();
    } else {
        resize(m_expandWidth + MENU_WIDTH_OFFSET, height());
        onExpandAnimationFinished();
    }
}

void NavigationPanel::collapse()
{
    setWidgetCompacted(true);

    if (m_expandAnimation->state() == QPropertyAnimation::Running) {
        return;
    }

    // 折叠所有树形部件
    for (const NavigationItem& item : m_items) {
        if (auto* treeWidget = qobject_cast<NavigationTreeWidgetBase*>(item.widget)) {
            if (treeWidget->isRoot()) {
                treeWidget->setExpanded(false);
            }
        }
        if (item.widget) {
            item.widget->setCompacted(true);
        }
    }

    m_expandAnimation->setStartValue(m_expandWidth);
    m_expandAnimation->setEndValue(DEFAULT_COMPACT_WIDTH);
    m_expandAnimation->setProperty("expand", false);
    m_expandAnimation->start();
}

void NavigationPanel::toggle()
{
    if (m_displayMode == NavigationPanel::DisplayMode::COMPACT ||
        m_displayMode == NavigationPanel::DisplayMode::MINIMAL) {
        expand();
    } else {
        collapse();
    }
}

bool NavigationPanel::isCollapsed() const
{
    return m_displayMode == NavigationPanel::DisplayMode::COMPACT;
}

void NavigationPanel::onWidgetClicked()
{
    auto* widget = qobject_cast<NavigationWidget*>(sender());
    if (!widget) {
        return;
    }

    if (!widget->property("isSelectable").toBool()) {
        if (auto* treeWidget = qobject_cast<NavigationTreeWidget*>(widget)) {
            showFlyoutNavigationMenu(treeWidget);
        }
        return;
    }

    setCurrentItem(widget->property("routeKey").toString());

    bool isLeaf = true;
    if (auto* treeWidget = qobject_cast<NavigationTreeWidgetBase*>(widget)) {
        isLeaf = treeWidget->isLeaf();
    }

    if (m_displayMode == NavigationPanel::DisplayMode::MENU && isLeaf) {
        collapse();
    } else if (isCollapsed()) {
        if (auto* treeWidget = qobject_cast<NavigationTreeWidget*>(widget)) {
            showFlyoutNavigationMenu(treeWidget);
        }
    }
}

void NavigationPanel::showFlyoutNavigationMenu(NavigationTreeWidget* widget)
{
    if (!isCollapsed() || !widget) {
        return;
    }

    if (!widget->isRoot() || widget->isLeaf()) {
        return;
    }

    auto* layout = new QHBoxLayout();
    auto* view = new FlyoutViewBase();
    view->setLayout(layout);

    auto* flyout = new Flyout(view, window());

    // 添加导航菜单到弹出窗口
    auto* menu = new NavigationFlyoutMenu(widget, view);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(menu);

    // 执行弹出动画
    flyout->resize(flyout->sizeHint());
    SlideRightFlyoutAnimationManager manager(flyout);
    QPoint pos = manager.position(widget);
    flyout->exec(pos, FlyoutAnimationType::SLIDE_RIGHT);

    connect(menu, &NavigationFlyoutMenu::expanded,
            this, [this, flyout, widget, menu]() {
        adjustFlyoutMenuSize(flyout, widget, menu);
    });
}

void NavigationPanel::adjustFlyoutMenuSize(
    Flyout* flyout,
    NavigationTreeWidget* widget,
    NavigationFlyoutMenu* menu)
{
    if (!flyout || !widget || !menu) {
        return;
    }

    flyout->view()->setFixedSize(menu->size());
    flyout->setFixedSize(flyout->layout()->sizeHint());

    SlideRightFlyoutAnimationManager manager(flyout);
    QPoint pos = manager.position(widget);

    QRect windowRect = window()->geometry();
    int flyoutWidth = flyout->sizeHint().width() + 5;
    int flyoutHeight = flyout->sizeHint().height();

    int x = qBound(windowRect.left(), pos.x(), windowRect.right() - flyoutWidth);
    int y = qBound(windowRect.top() + MIN_WINDOW_TOP_OFFSET,
                   pos.y() - 4,
                   windowRect.bottom() - flyoutHeight + 5);

    flyout->move(x, y);
}

bool NavigationPanel::eventFilter(QObject* obj, QEvent* event)
{
    if (obj != window() || !m_isCollapsible) {
        return QFrame::eventFilter(obj, event);
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
<<<<<<< HEAD
    Q_ASSERT(mouseEvent);
=======
        Q_ASSERT(mouseEvent);
>>>>>>> a7b5ee8 (fix: 修复多处内存安全和缓存一致性问题)
        if (!geometry().contains(mouseEvent->pos()) &&
            m_displayMode == NavigationPanel::DisplayMode::MENU) {
            collapse();
        }
    } else if (event->type() == QEvent::Resize) {
        auto* resizeEvent = static_cast<QResizeEvent*>(event);
<<<<<<< HEAD
    Q_ASSERT(resizeEvent);
=======
        Q_ASSERT(resizeEvent);
>>>>>>> a7b5ee8 (fix: 修复多处内存安全和缓存一致性问题)
        int width = resizeEvent->size().width();

        if (width < m_minimumExpandWidth &&
            m_displayMode == NavigationPanel::DisplayMode::EXPAND) {
            collapse();
        } else if (width >= m_minimumExpandWidth &&
                   m_displayMode == NavigationPanel::DisplayMode::COMPACT &&
                   !m_isMenuButtonVisible) {
            expand();
        }
    }

    return QFrame::eventFilter(obj, event);
}

void NavigationPanel::onExpandAnimationFinished()
{
    if (!m_expandAnimation->property("expand").toBool()) {
        m_displayMode = m_isMinimalEnabled ? NavigationPanel::DisplayMode::MINIMAL
                                           : NavigationPanel::DisplayMode::COMPACT;
        emit displayModeChanged(m_displayMode);
    }

    if (m_displayMode == NavigationPanel::DisplayMode::MINIMAL) {
        hide();
        setProperty("menu", false);
        style()->unpolish(this);
        style()->polish(this);
    } else if (m_displayMode == NavigationPanel::DisplayMode::COMPACT) {
        setProperty("menu", false);
        style()->unpolish(this);
        style()->polish(this);

        for (const NavigationItem& item : m_items) {
            if (item.widget) {
                item.widget->setCompacted(true);
            }
        }

        // 重新附加到父窗口
        QWidget* parent = m_parentWidget;
        if (parent && !parent->isWindow()) {
            setParent(parent);
            move(0, 0);
            show();
        }
    }
}

void NavigationPanel::setWidgetCompacted(bool isCompacted)
{
    QList<NavigationWidget*> widgets = findChildren<NavigationWidget*>();
    for (NavigationWidget* widget : widgets) {
        if (widget) {
            widget->setCompacted(isCompacted);
        }
    }
}

int NavigationPanel::calculateLayoutMinHeight() const
{
    int topHeight = m_topLayout->minimumSize().height();
    int bottomHeight = m_bottomLayout->minimumSize().height();

    QList<NavigationSeparator*> separators = findChildren<NavigationSeparator*>();
    int separatorHeight = 0;
    for (const NavigationSeparator* separator : separators) {
        if (separator) {
            separatorHeight += separator->height();
        }
    }

    int spacing = m_topLayout->count() * m_topLayout->spacing() +
                  m_bottomLayout->count() * m_bottomLayout->spacing();

    return 36 + topHeight + bottomHeight + separatorHeight + spacing;
}

bool NavigationPanel::canDrawAcrylic() const
{
    return false; // TODO: 实现亚克力效果检测
}

void NavigationPanel::paintEvent(QPaintEvent* event)
{
    if (!canDrawAcrylic() || m_displayMode != NavigationPanel::DisplayMode::MENU) {
        QFrame::paintEvent(event);
        return;
    }

    // TODO: 实现亚克力背景绘制
    QPainter painter(this);
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(0, 1, width() - 1, height() - 1, 7, 7);
    path.addRect(0, 1, 8, height() - 1);

    updateAcrylicColor();

    QFrame::paintEvent(event);
}

// NavigationItemLayout 实现
NavigationItemLayout::NavigationItemLayout(QWidget* parent)
    : QVBoxLayout(parent)
{
}

void NavigationItemLayout::setGeometry(const QRect& rect)
{
    QVBoxLayout::setGeometry(rect);

    for (int i = 0; i < count(); ++i) {
        QLayoutItem* item = itemAt(i);
        if (!item) {
            continue;
        }

        if (auto* separator = qobject_cast<NavigationSeparator*>(item->widget())) {
            QRect geo = item->geometry();
            separator->setGeometry(QRect(0, geo.y(), geo.width(), geo.height()));
        }
    }
}
