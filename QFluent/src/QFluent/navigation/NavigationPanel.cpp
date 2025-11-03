#include "NavigationPanel.h"
#include <QApplication>
#include <QStyle>
#include <QCursor>
#include <QPainter>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMap>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QIcon>
#include <QColor>
#include <QPoint>
#include <QPainterPath>
#include <QMargins>
#include <QScrollArea>

#include "FluentIcon.h"
#include "Theme.h"
#include "ProfileCard.h"
#include "NavigationWidget.h"
#include "StyleSheet.h"

#include "../scrollbar/ScrollBar.h"
#include "QFluent/menu/RoundMenu.h"
#include "QFluent/menu/MenuActionListWidget.h"


NavigationPanel::NavigationPanel(QWidget* parent, bool isMinimalEnabled)
    : QFrame(parent), m_parent(parent), m_isMenuButtonVisible(true),
      m_isReturnButtonVisible(false), m_isCollapsible(true), m_isAcrylicEnabled(false),
      m_isMinimalEnabled(isMinimalEnabled) {

    // 初始化成员
    m_scrollArea = new QScrollArea(this);
    m_scrollWidget = new QWidget();

    m_menuButton = new NavigationToolButton(FluentIconType::IconType::MENU, this);
    m_avatarWidget = new NavigationAvatarWidget("Administrator", QImage(":/res/app/avatar.png"), this);
    connect(m_avatarWidget, &NavigationAvatarWidget::clicked, this, [=](){
        auto menu = new RoundMenu("menu", this);
        auto card = new ProfileCard(":/res/app/avatar.png", "硝子酱", "shokokawaii@outlook.com", menu);
        menu->setItemHeight(36);
        menu->view()->setMaxVisibleItems(0);
        menu->addWidget(card);
        menu->addSeparator();
        menu->addAction(new Action(FluentIcon(FluentIconType::PEOPLE), "管理账户和设置"));
        menu->addAction(new Action(FluentIcon(FluentIconType::SHOPPING_CART), "支付方式"));
        menu->addAction(new Action(FluentIcon(FluentIconType::CODE), "兑换代码和礼品卡"));
        menu->addSeparator();
        menu->addAction(new Action(FluentIcon(FluentIconType::SETTING), "设置"));

        menu->exec(m_avatarWidget->mapToGlobal(QPoint(m_avatarWidget->width()+5, -100)));
    });

    m_vBoxLayout = new NavigationItemLayout(this);
    m_topLayout = new QVBoxLayout();
    m_bottomLayout = new QVBoxLayout();
    m_scrollLayout = new QVBoxLayout(m_scrollWidget);

    // m_history = qrouter;

    m_expandAni = new QPropertyAnimation(this, "minimumWidth", this);
    m_expandWidth = 160;

    if (isMinimalEnabled) {
        m_displayMode = NavigationType::NavigationDisplayMode::MINIMAL;
    } else {
        m_displayMode = NavigationType::NavigationDisplayMode::COMPACT;
    }

    initWidget();
}

void NavigationPanel::initWidget() {
    this->setFixedWidth(48);
    this->setAttribute(Qt::WA_StyledBackground);
    this->window()->installEventFilter(this);

    m_scrollArea->setVerticalScrollBar(new ScrollBar(this));
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_scrollWidget);
    m_scrollArea->setWidgetResizable(true);

    m_expandAni->setEasingCurve(QEasingCurve::OutQuad);
    m_expandAni->setDuration(150);

    connect(m_menuButton, &NavigationToolButton::clicked, this, &NavigationPanel::toggle);
    connect(m_expandAni, &QPropertyAnimation::finished, this, &NavigationPanel::onExpandAniFinished);

    // connect(m_history, &Router::emptyChanged, m_returnButton, &QToolButton::setDisabled);
    // connect(m_returnButton, &NavigationToolButton::clicked, m_history, &Router::pop);



    this->setProperty("menu", false);
    m_scrollWidget->setObjectName("scrollWidget");

    // this->setStyleSheet("background-color: green;");
    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::NAVIGATION_INTERFACE);
    StyleSheetManager::instance()->registerWidget(m_scrollWidget, ThemeType::ThemeStyle::NAVIGATION_INTERFACE);

    initLayout();
}

void NavigationPanel::initLayout() {
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

    m_topLayout->addWidget(m_avatarWidget, 0, Qt::AlignTop);
    m_topLayout->addWidget(m_menuButton, 0, Qt::AlignTop);
}

void NavigationPanel::updateAcrylicColor() {
    QColor tintColor, luminosityColor;

    if (1) {
        tintColor = QColor(32, 32, 32, 200);
        luminosityColor = QColor(0, 0, 0, 0);
    } else {
        tintColor = QColor(255, 255, 255, 180);
        luminosityColor = QColor(255, 255, 255, 0);
    }

    // m_acrylicBrush->setTintColor(tintColor);
    // m_acrylicBrush->setLuminosityColor(luminosityColor);
}

NavigationWidget* NavigationPanel::widget(const QString& routeKey) {
    if (!m_items.contains(routeKey)) {
        throw RouteKeyError(QString("`%1` is illegal.").arg(routeKey));
    }

    return m_items[routeKey].widget;
}

void NavigationPanel::addItem(const QString& routeKey, FluentIconType::IconType icon, const QString& text,
                            const std::function<void()>& onClick, bool selectable,
                            NavigationType::NavigationItemPosition position, const QString& tooltip,
                            const QString& parentRouteKey) {
    insertItem(-1, routeKey, icon, text, onClick, selectable, position, tooltip, parentRouteKey);
}

void NavigationPanel::addWidget(const QString& routeKey, NavigationWidget* widget,
                              const std::function<void()>& onClick,
                              NavigationType::NavigationItemPosition position, const QString& tooltip,
                              const QString& parentRouteKey) {
    insertWidget(-1, routeKey, widget, onClick, position, tooltip, parentRouteKey);
}

void NavigationPanel::insertItem(int index, const QString& routeKey, FluentIconType::IconType icon,
                               const QString& text, const std::function<void()>& onClick,
                               bool selectable, NavigationType::NavigationItemPosition position,
                               const QString& tooltip, const QString& parentRouteKey) {
    if (m_items.contains(routeKey)) {
        return;
    }

    NavigationTreeWidget* w = new NavigationTreeWidget(icon, text, selectable, this);
    w->setExpandWidth(m_expandWidth);
    insertWidget(index, routeKey, w, onClick, position, tooltip, parentRouteKey);
}

void NavigationPanel::insertWidget(int index, const QString& routeKey, NavigationWidget* widget,
                                 const std::function<void()>& onClick,
                                 NavigationType::NavigationItemPosition position, const QString& tooltip,
                                 const QString& parentRouteKey) {
    if (m_items.contains(routeKey)) {
        return;
    }

    registerWidget(routeKey, parentRouteKey, widget, onClick, tooltip);

    if (!parentRouteKey.isEmpty()) {
        this->widget(parentRouteKey)->insertChild(index, widget);
    } else {
        insertWidgetToLayout(index, widget, position);
    }
}

void NavigationPanel::addSeparator(NavigationType::NavigationItemPosition position) {
    insertSeparator(-1, position);
}

void NavigationPanel::insertSeparator(int index, NavigationType::NavigationItemPosition position) {
    NavigationSeparator* separator = new NavigationSeparator(this);
    insertWidgetToLayout(index, separator, position);
}

void NavigationPanel::registerWidget(const QString& routeKey, const QString& parentRouteKey,
                                   NavigationWidget* widget, const std::function<void()>& onClick,
                                   const QString& tooltip) {
    connect(widget, &NavigationWidget::clicked, this, &NavigationPanel::onWidgetClicked);

    if (onClick) {
        connect(widget, &NavigationWidget::clicked, onClick);
    }

    widget->setProperty("routeKey", routeKey);
    widget->setProperty("parentRouteKey", parentRouteKey);
    m_items[routeKey] = {routeKey, parentRouteKey, widget};

    if (m_displayMode == NavigationType::NavigationDisplayMode::EXPAND || m_displayMode == NavigationType::NavigationDisplayMode::MENU) {
        widget->setCompacted(false);
    }
}

void NavigationPanel::insertWidgetToLayout(int index, NavigationWidget* widget, NavigationType::NavigationItemPosition position) {
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

void NavigationPanel::removeWidget(const QString& routeKey) {
    if (!m_items.contains(routeKey)) {
        return;
    }

    NavigationItem item = m_items.take(routeKey);

    // if (!item.parentRouteKey.isEmpty()) {
    //     widget(item.parentRouteKey)->removeChild(item.widget);
    // }

    // 处理树形小部件的子项
    if (NavigationTreeWidgetBase* treeWidget = dynamic_cast<NavigationTreeWidgetBase*>(item.widget)) {
        QList<NavigationWidget*> children = treeWidget->findChildren<NavigationWidget*>(QString(), Qt::FindChildrenRecursively);
        for (NavigationWidget* child : children) {
            QString key = child->property("routeKey").toString();
            if (key.isEmpty()) {
                continue;
            }

            m_items.remove(key);
            child->deleteLater();
            // m_history->remove(key);
        }
    }

    item.widget->deleteLater();
    // m_history->remove(routeKey);
}

void NavigationPanel::setCurrentItem(const QString& routeKey) {
    if (!m_items.contains(routeKey)) {
        return;
    }

    for (auto it = m_items.constBegin(); it != m_items.constEnd(); ++it) {
        it.value().widget->setSelected(it.key() == routeKey);
    }
}

void NavigationPanel::setMenuButtonVisible(bool isVisible) {
    m_isMenuButtonVisible = isVisible;
    m_menuButton->setVisible(isVisible);
}

void NavigationPanel::setReturnButtonVisible(bool isVisible) {
    //m_isReturnButtonVisible = isVisible;
    //m_returnButton->setVisible(isVisible);
}

void NavigationPanel::setCollapsible(bool on) {
    m_isCollapsible = on;
    if (!on && m_displayMode != NavigationType::NavigationDisplayMode::EXPAND) {
        expand(false);
    }
}

void NavigationPanel::setExpandWidth(int width) {
    if (width <= 42) {
        return;
    }

    m_expandWidth = width;
    m_avatarWidget->setExpandWidth(width);
}

void NavigationPanel::setAcrylicEnabled(bool isEnabled) {
    if (isEnabled == m_isAcrylicEnabled) {
        return;
    }

    m_isAcrylicEnabled = isEnabled;
    this->setProperty("transparent", canDrawAcrylic());
    this->style()->unpolish(this);
    this->style()->polish(this);
    this->update();
}

bool NavigationPanel::isAcrylicEnabled() const {
    return m_isAcrylicEnabled;
}

void NavigationPanel::expand(bool useAni) {
    setWidgetCompacted(false);
    m_expandAni->setProperty("expand", true);

    if (!m_isMinimalEnabled || !m_isCollapsible) {
        m_displayMode = NavigationType::NavigationDisplayMode::EXPAND;
    } else {
        this->setProperty("menu", true);
        this->style()->unpolish(this);
        this->style()->polish(this);
        m_displayMode = NavigationType::NavigationDisplayMode::MENU;


        if (!m_parent->isWindow()) {
            QPoint pos = this->parentWidget()->pos();
            this->setParent(this->window());
            this->move(pos);
        }

        this->show();
    }

    if (useAni) {
        emit displayModeChanged(m_displayMode);
        m_expandAni->setStartValue(48);
        m_expandAni->setEndValue(m_expandWidth + 15);
        m_expandAni->start();
    } else {
        this->resize(m_expandWidth + 15, this->height());
        onExpandAniFinished();
    }
}

void NavigationPanel::collapse() {
    setWidgetCompacted(true);
    if (m_expandAni->state() == QPropertyAnimation::Running) {
        return;
    }

    // 折叠所有树形小部件
    for (const NavigationItem& item : m_items) {
        if (NavigationTreeWidgetBase* treeWidget = dynamic_cast<NavigationTreeWidgetBase*>(item.widget)) {
            if (treeWidget->isRoot()) {
                treeWidget->setExpanded(false);
            }
        }
        item.widget->setCompacted(false);
    }


    m_expandAni->setStartValue(m_expandWidth);
    m_expandAni->setEndValue(48);
    m_expandAni->setProperty("expand", false);
    m_expandAni->start();
}

void NavigationPanel::toggle() {
    if (m_displayMode == NavigationType::NavigationDisplayMode::COMPACT || m_displayMode == NavigationType::NavigationDisplayMode::MINIMAL) {
        expand();
    } else {
        collapse();
    }
}

bool NavigationPanel::isCollapsed() const {
    return m_displayMode == NavigationType::NavigationDisplayMode::COMPACT;
}

void NavigationPanel::onWidgetClicked() {
    NavigationWidget* widget = qobject_cast<NavigationWidget*>(sender());
    // if (!widget->isSelectable()) {
    //     if (NavigationTreeWidget* treeWidget = dynamic_cast<NavigationTreeWidget*>(widget)) {
    //         showFlyoutNavigationMenu(treeWidget);
    //     }
    //     return;
    // }

    setCurrentItem(widget->property("routeKey").toString());

    bool isLeaf = true;
    if (NavigationTreeWidgetBase* treeWidget = dynamic_cast<NavigationTreeWidgetBase*>(widget)) {
        isLeaf = treeWidget->isLeaf();
    }

    if (m_displayMode == NavigationType::NavigationDisplayMode::MENU && isLeaf) {
        collapse();
    } else if (isCollapsed()) {
        if (NavigationTreeWidget* treeWidget = dynamic_cast<NavigationTreeWidget*>(widget)) {
            showFlyoutNavigationMenu(treeWidget);
        }
    }
}

void NavigationPanel::showFlyoutNavigationMenu(NavigationTreeWidget* widget) {
    // if (!(isCollapsed() && widget)) {
    //     return;
    // }

    // if (!widget->isRoot() || widget->isLeaf()) {
    //     return;
    // }

    // QHBoxLayout* layout = new QHBoxLayout();

    // FlyoutViewBase* view;
    // Flyout* flyout;

    // if (canDrawAcrylic()) {
    //     view = new AcrylicFlyoutViewBase();
    //     view->setLayout(layout);
    //     flyout = new AcrylicFlyout(view, this->window());
    // } else {
    //     view = new FlyoutViewBase();
    //     view->setLayout(layout);
    //     flyout = new Flyout(view, this->window());
    // }

    // // 添加导航菜单到弹出窗口
    // NavigationFlyoutMenu* menu = new NavigationFlyoutMenu(widget, view);
    // layout->setContentsMargins(0, 0, 0, 0);
    // layout->addWidget(menu);

    // // 执行弹出动画
    // flyout->resize(flyout->sizeHint());
    // QPoint pos = SlideRightFlyoutAnimationManager(flyout).position(widget);
    // flyout->exec(pos, FlyoutAnimationType::SLIDE_RIGHT);

    // connect(menu, &NavigationFlyoutMenu::expanded, [this, flyout, widget, menu]() {
    //     adjustFlyoutMenuSize(flyout, widget, menu);
    // });
}

// void NavigationPanel::adjustFlyoutMenuSize(Flyout* flyout, NavigationTreeWidget* widget, NavigationFlyoutMenu* menu) {
//     flyout->view()->setFixedSize(menu->size());
//     flyout->setFixedSize(flyout->layout()->sizeHint());

//     SlideRightFlyoutAnimationManager manager(flyout);
//     QPoint pos = manager.position(widget);

//     QRect rect = this->window()->geometry();
//     int w = flyout->sizeHint().width() + 5;
//     int h = flyout->sizeHint().height();
//     int x = qMax(rect.left(), qMin(pos.x(), rect.right() - w));
//     int y = qMax(rect.top() + 42, qMin(pos.y() - 4, rect.bottom() - h + 5));
//     flyout->move(x, y);
// }

bool NavigationPanel::eventFilter(QObject* obj, QEvent* e) {
    // if (obj != this->window() || !m_isCollapsible) {
    //     return QFrame::eventFilter(obj, e);
    // }

    // if (e->type() == QEvent::MouseButtonRelease) {
    //     QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
    //     if (!this->geometry().contains(mouseEvent->pos()) && m_displayMode == NavigationType::NavigationDisplayMode::MENU) {
    //         collapse();
    //     }
    // } else if (e->type() == QEvent::Resize) {
    //     QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(e);
    //     int w = resizeEvent->size().width();
    //     if (w < m_minimumExpandWidth && m_displayMode == NavigationType::NavigationDisplayMode::EXPAND) {
    //         collapse();
    //     } else if (w >= m_minimumExpandWidth && m_displayMode == NavigationType::NavigationDisplayMode::COMPACT && !m_isMenuButtonVisible) {
    //         expand();
    //     }
    // }

    return QFrame::eventFilter(obj, e);
}

void NavigationPanel::onExpandAniFinished() {
    if (!m_expandAni->property("expand").toBool()) {
        if (m_isMinimalEnabled) {
            m_displayMode = NavigationType::NavigationDisplayMode::MINIMAL;
        } else {
            m_displayMode = NavigationType::NavigationDisplayMode::COMPACT;
        }

        emit displayModeChanged(m_displayMode);
    }

    if (m_displayMode == NavigationType::NavigationDisplayMode::MINIMAL) {
        this->hide();
        this->setProperty("menu", false);
        this->style()->unpolish(this);
        this->style()->polish(this);
    } else if (m_displayMode == NavigationType::NavigationDisplayMode::COMPACT) {
        this->setProperty("menu", false);
        this->style()->unpolish(this);
        this->style()->polish(this);

        for (const NavigationItem& item : m_items) {
            item.widget->setCompacted(true);
        }

        if (!m_parent->isWindow()) {
            this->setParent(m_parent);
            this->move(0, 0);
            this->show();
        }
    }
}

void NavigationPanel::setWidgetCompacted(bool isCompacted) {
    QList<NavigationWidget*> widgets = this->findChildren<NavigationWidget*>();


    for (NavigationWidget* widget : widgets) {
        widget->setCompacted(isCompacted);
    }
}

int NavigationPanel::layoutMinHeight() {
    int th = m_topLayout->minimumSize().height();
    int bh = m_bottomLayout->minimumSize().height();

    QList<NavigationSeparator*> separators = this->findChildren<NavigationSeparator*>();
    int sh = 0;
    for (NavigationSeparator* separator : separators) {
        sh += separator->height();
    }

    int spacing = m_topLayout->count() * m_topLayout->spacing() +
                 m_bottomLayout->count() * m_bottomLayout->spacing();

    return 36 + th + bh + sh + spacing;
}

bool NavigationPanel::canDrawAcrylic() {
    return false;//m_acrylicBrush->isAvailable() && m_isAcrylicEnabled;
}

void NavigationPanel::paintEvent(QPaintEvent* e) {
    if (!canDrawAcrylic() || m_displayMode != NavigationType::NavigationDisplayMode::MENU) {
        QFrame::paintEvent(e);
        return;
    }

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(0, 1, this->width() - 1, this->height() - 1, 7, 7);
    path.addRect(0, 1, 8, this->height() - 1);
    // m_acrylicBrush->setClipPath(path);

    updateAcrylicColor();
    // m_acrylicBrush->paint();

    QFrame::paintEvent(e);
}

void NavigationItemLayout::setGeometry(const QRect& rect) {
    QVBoxLayout::setGeometry(rect);

    for (int i = 0; i < this->count(); ++i) {
        QLayoutItem* item = this->itemAt(i);
        if (NavigationSeparator* separator = qobject_cast<NavigationSeparator*>(item->widget())) {
            QRect geo = item->geometry();
            separator->setGeometry(QRect(0, geo.y(), geo.width(), geo.height()));
        }
    }
}
