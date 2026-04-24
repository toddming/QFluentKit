#include "TabBar.h"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QApplication>
#include <QFontMetrics>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>

#include "StyleSheet.h"

// ==================== TabToolButton ====================

TabToolButton::TabToolButton(const QIcon &icon, QWidget *parent)
    : TransparentToolButton(icon, parent)
{
    setFixedSize(32, 24);
    setIconSize(QSize(12, 12));
}

TabToolButton::TabToolButton(QWidget *parent)
    : TransparentToolButton(parent)
{
    setFixedSize(32, 24);
    setIconSize(QSize(12, 12));
}

void TabToolButton::drawIcon(QPainter *painter, const QRectF &rect,
                             Fluent::ThemeMode theme)
{
    Q_UNUSED(theme);
    icon().paint(painter, rect.toRect());
}

// ==================== TabItem ====================

TabItem::TabItem(const QString &text, QWidget *parent, const QIcon &icon)
    : QPushButton(text, parent)
    , m_closeButton(nullptr)
    , m_shadowEffect(nullptr)
    , m_slideAnimation(nullptr)
    , m_icon(icon)
    , m_lightSelectedBackgroundColor(249, 249, 249)
    , m_darkSelectedBackgroundColor(40, 40, 40)
    , m_borderRadius(5)
    , m_isHover(false)
    , m_isPressed(false)
    , m_isSelected(false)
    , m_isShadowEnabled(true)
    , m_closeButtonDisplayMode(TabCloseButtonDisplayMode::Always)
{
    m_closeButton = new TabToolButton(Fluent::icon(Fluent::IconType::CLOSE), this);
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_slideAnimation = new QPropertyAnimation(this, "pos", this);

    initWidget();
}

TabItem::TabItem(const QString &text, Fluent::IconType type, QWidget *parent)
    : TabItem(text, parent, Fluent::icon(type))
{
}

void TabItem::initWidget()
{
    setFixedHeight(36);
    setAttribute(Qt::WA_LayoutUsesWidgetRect);

    m_closeButton->setIconSize(QSize(10, 10));

    m_shadowEffect->setBlurRadius(5);
    m_shadowEffect->setOffset(0, 1);
    setGraphicsEffect(m_shadowEffect);
    setSelected(false);

    connect(m_closeButton, &TabToolButton::clicked,
            this, &TabItem::closed);
}

void TabItem::slideTo(int x, int duration)
{
    m_slideAnimation->setStartValue(pos());
    m_slideAnimation->setEndValue(QPoint(x, y()));
    m_slideAnimation->setDuration(duration);
    m_slideAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_slideAnimation->start();
}

void TabItem::setShadowEnabled(bool enabled)
{
    if (enabled == m_isShadowEnabled)
        return;

    m_isShadowEnabled = enabled;
    m_shadowEffect->setColor(QColor(0, 0, 0, 50 * canShowShadow()));
}

bool TabItem::canShowShadow() const
{
    return m_isSelected && m_isShadowEnabled;
}

void TabItem::setRouteKey(const QString &key)
{
    m_routeKey = key;
}

QString TabItem::routeKey() const
{
    return m_routeKey;
}

void TabItem::setBorderRadius(int radius)
{
    m_borderRadius = radius;
    update();
}

void TabItem::setSelected(bool selected)
{
    m_isSelected = selected;
    m_shadowEffect->setColor(QColor(0, 0, 0, 50 * canShowShadow()));
    update();

    if (selected)
        raise();

    if (m_closeButtonDisplayMode == TabCloseButtonDisplayMode::OnHover)
        m_closeButton->setVisible(selected);
}

void TabItem::setCloseButtonDisplayMode(TabCloseButtonDisplayMode mode)
{
    if (mode == m_closeButtonDisplayMode)
        return;

    m_closeButtonDisplayMode = mode;

    if (mode == TabCloseButtonDisplayMode::Never) {
        m_closeButton->hide();
    } else if (mode == TabCloseButtonDisplayMode::Always) {
        m_closeButton->show();
    } else {
        m_closeButton->setVisible(m_isHover || m_isSelected);
    }
}

void TabItem::setTextColor(const QColor &color)
{
    m_textColor = color;
    update();
}

void TabItem::setSelectedBackgroundColor(const QColor &light, const QColor &dark)
{
    m_lightSelectedBackgroundColor = light;
    m_darkSelectedBackgroundColor = dark;
    update();
}

void TabItem::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
    m_closeButton->move(width() - 6 - m_closeButton->width(),
                       (height() - m_closeButton->height()) / 2);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void TabItem::enterEvent(QEnterEvent *event)
{
    QPushButton::enterEvent(event);
    m_isHover = true;
    if (m_closeButtonDisplayMode == TabCloseButtonDisplayMode::OnHover)
        m_closeButton->show();
}
#else
void TabItem::enterEvent(QEvent *event)
{
    QPushButton::enterEvent(event);
    m_isHover = true;
    if (m_closeButtonDisplayMode == TabCloseButtonDisplayMode::OnHover)
        m_closeButton->show();
}
#endif

void TabItem::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);
    m_isHover = false;
    if (m_closeButtonDisplayMode == TabCloseButtonDisplayMode::OnHover &&
        !m_isSelected) {
        m_closeButton->hide();
    }
}

void TabItem::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);
    m_isPressed = true;
    forwardMouseEvent(event);
}

void TabItem::mouseMoveEvent(QMouseEvent *event)
{
    QPushButton::mouseMoveEvent(event);
    forwardMouseEvent(event);
}

void TabItem::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);
    m_isPressed = false;
    forwardMouseEvent(event);
}

void TabItem::forwardMouseEvent(QMouseEvent *event)
{
    const QPoint pos = mapToParent(event->pos());
    QMouseEvent newEvent(event->type(), pos, event->button(),
                        event->buttons(), event->modifiers());
    QApplication::sendEvent(parentWidget(), &newEvent);
}

QSize TabItem::sizeHint() const
{
    return QSize(maximumWidth(), 36);
}

void TabItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_isSelected)
        drawSelectedBackground(&painter);
    else
        drawNotSelectedBackground(&painter);

    // 绘制图标
    if (!m_isSelected) {
        painter.setOpacity(Theme::isDark() ? 0.79 : 0.61);
    }

    m_icon.paint(&painter, 10, 10, 16, 16, Qt::AlignCenter);

    // 绘制文本
    drawText(&painter);
}

void TabItem::drawSelectedBackground(QPainter *painter)
{
    const int w = width();
    const int h = height();
    const int r = m_borderRadius;
    const int d = 2 * r;

    const bool isDark = Theme::isDark();

    // 绘制顶部边框
    QPainterPath path;
    path.arcMoveTo(1, h - d - 1, d, d, 225);
    path.arcTo(1, h - d - 1, d, d, 225, -45);
    path.lineTo(1, r);
    path.arcTo(1, 1, d, d, -180, -90);
    path.lineTo(w - r, 1);
    path.arcTo(w - d - 1, 1, d, d, 90, -90);
    path.lineTo(w - 1, h - r);
    path.arcTo(w - d - 1, h - d - 1, d, d, 0, -45);

    QColor topBorderColor(0, 0, 0, 20);
    if (isDark) {
        if (m_isPressed)
            topBorderColor = QColor(255, 255, 255, 18);
        else if (m_isHover)
            topBorderColor = QColor(255, 255, 255, 13);
    } else {
        topBorderColor = QColor(0, 0, 0, 16);
    }

    painter->strokePath(path, topBorderColor);

    // 绘制底部边框
    QPainterPath bottomPath;
    bottomPath.arcMoveTo(1, h - d - 1, d, d, 225);
    bottomPath.arcTo(1, h - d - 1, d, d, 225, 45);
    bottomPath.lineTo(w - r - 1, h - 1);
    bottomPath.arcTo(w - d - 1, h - d - 1, d, d, 270, 45);

    QColor bottomBorderColor = topBorderColor;
    if (!isDark)
        bottomBorderColor = QColor(0, 0, 0, 63);

    painter->strokePath(bottomPath, bottomBorderColor);

    // 绘制背景
    painter->setPen(Qt::NoPen);
    const QRect rect = this->rect().adjusted(1, 1, -1, -1);
    painter->setBrush(isDark ? m_darkSelectedBackgroundColor :
                              m_lightSelectedBackgroundColor);
    painter->drawRoundedRect(rect, r, r);
}

void TabItem::drawNotSelectedBackground(QPainter *painter)
{
    if (!m_isPressed && !m_isHover)
        return;

    const bool isDark = Theme::isDark();
    QColor color;

    if (m_isPressed)
        color = isDark ? QColor(255, 255, 255, 12) : QColor(0, 0, 0, 7);
    else
        color = isDark ? QColor(255, 255, 255, 15) : QColor(0, 0, 0, 10);

    painter->setBrush(color);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect().adjusted(1, 1, -1, -1),
                            m_borderRadius, m_borderRadius);
}

void TabItem::drawText(QPainter *painter)
{
    const QFontMetrics fm(font());
    const int textWidth = fm.horizontalAdvance(text());

    QRectF textRect;
    if (m_icon.isNull()) {
        const int deltaWidth = m_closeButton->isVisible() ? 47 : 20;
        textRect = QRectF(10, 0, width() - deltaWidth, height());
    } else {
        const int deltaWidth = m_closeButton->isVisible() ? 70 : 45;
        textRect = QRectF(33, 0, width() - deltaWidth, height());
    }

    QPen pen;
    QColor color = Theme::isDark() ? Qt::white : Qt::black;
    if (m_textColor.isValid())
        color = m_textColor;

    const qreal rectWidth = textRect.width();

    if (textWidth > rectWidth) {
        QLinearGradient gradient(textRect.x(), 0, textWidth + textRect.x(), 0);
        gradient.setColorAt(0, color);
        gradient.setColorAt(qMax(0.0, (rectWidth - 10) / textWidth), color);
        gradient.setColorAt(qMax(0.0, rectWidth / textWidth), Qt::transparent);
        gradient.setColorAt(1, Qt::transparent);
        pen.setBrush(QBrush(gradient));
    } else {
        pen.setColor(color);
    }

    painter->setPen(pen);
    painter->setFont(font());
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text());
}

// ==================== TabBar ====================

TabBar::TabBar(QWidget *parent)
    : SingleDirectionScrollArea(parent, Qt::Horizontal)
    , m_view(nullptr)
    , m_hBoxLayout(nullptr)
    , m_itemLayout(nullptr)
    , m_widgetLayout(nullptr)
    , m_addButton(nullptr)
    , m_lightSelectedBackgroundColor(249, 249, 249)
    , m_darkSelectedBackgroundColor(40, 40, 40)
    , m_currentIndex(-1)
    , m_tabMaxWidth(240)
    , m_tabMinWidth(64)
    , m_isMovable(true)
    , m_isScrollable(false)
    , m_isTabShadowEnabled(true)
    , m_isDragging(false)
    , m_closeButtonDisplayMode(TabCloseButtonDisplayMode::Always)
    , m_isAdjustingLayout(false)
{
    m_view = new QWidget(this);
    m_hBoxLayout = new QHBoxLayout(m_view);
    m_itemLayout = new QHBoxLayout();
    m_widgetLayout = new QHBoxLayout();
    m_addButton = new TabToolButton(Fluent::icon(Fluent::IconType::ADD), this);

    initWidget();
}

void TabBar::initWidget()
{
    setFixedHeight(46);
    setWidget(m_view);
    setViewportMargins(0, 0, 0, 0);

    m_hBoxLayout->setSizeConstraint(QHBoxLayout::SetMaximumSize);

    connect(m_addButton, &TabToolButton::clicked,
            this, &TabBar::tabAddRequested);

    m_view->setObjectName(QStringLiteral("view"));
    StyleSheet::registerWidget(this, Fluent::ThemeStyle::TAB_VIEW);
    StyleSheet::registerWidget(m_view, Fluent::ThemeStyle::TAB_VIEW);

    initLayout();
}

void TabBar::initLayout()
{
    m_hBoxLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_itemLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_widgetLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_itemLayout->setContentsMargins(5, 5, 5, 5);
    m_widgetLayout->setContentsMargins(0, 0, 0, 0);
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);

    m_itemLayout->setSizeConstraint(QHBoxLayout::SetMinAndMaxSize);

    m_hBoxLayout->setSpacing(0);
    m_itemLayout->setSpacing(0);

    m_hBoxLayout->addLayout(m_itemLayout);
    m_hBoxLayout->addSpacing(3);

    m_widgetLayout->addWidget(m_addButton, 0, Qt::AlignLeft);
    m_hBoxLayout->addLayout(m_widgetLayout);
    m_hBoxLayout->addStretch(1);
}

void TabBar::setAddButtonVisible(bool visible)
{
    m_addButton->setVisible(visible);
}

TabItem *TabBar::addTab(const QString &routeKey, const QString &text,
                        const QIcon &icon, std::function<void()> onClick)
{
    return insertTab(-1, routeKey, text, icon, onClick);
}

TabItem *TabBar::addTab(const QString &routeKey, const QString &text,
                        Fluent::IconType type, std::function<void()> onClick)
{
    return insertTab(-1, routeKey, text, type, onClick);
}

TabItem *TabBar::insertTab(int index, const QString &routeKey,
                           const QString &text, const QIcon &icon,
                           std::function<void()> onClick)
{
    if (m_itemMap.contains(routeKey)) {
        qWarning("TabBar::insertTab: The route key '%s' is duplicated.",
                qPrintable(routeKey));
        return nullptr;
    }

    if (index == -1)
        index = m_items.size();

    // 调整当前索引
    if (index <= currentIndex() && currentIndex() >= 0)
        m_currentIndex++;

    TabItem *item = new TabItem(text, m_view, icon);
    item->setRouteKey(routeKey);

    // 设置标签页尺寸
    const int width = isScrollable() ? tabMaximumWidth() : tabMinimumWidth();
    item->setMinimumWidth(width);
    item->setMaximumWidth(tabMaximumWidth());

    item->setShadowEnabled(isTabShadowEnabled());
    item->setCloseButtonDisplayMode(m_closeButtonDisplayMode);
    item->setSelectedBackgroundColor(m_lightSelectedBackgroundColor,
                                    m_darkSelectedBackgroundColor);

    connect(item, &TabItem::pressed, this, &TabBar::onItemPressed);
    connect(item, &TabItem::closed, this, [this, item]() {
        emit tabCloseRequested(m_items.indexOf(item));
    });

    if (onClick) {
        connect(item, &TabItem::pressed, this, onClick);
    }

    m_itemLayout->insertWidget(index, item, 1);
    m_items.insert(index, item);
    m_itemMap[routeKey] = item;

    if (m_items.size() == 1)
        setCurrentIndex(0);

    return item;
}

TabItem *TabBar::insertTab(int index, const QString &routeKey,
                           const QString &text, Fluent::IconType type,
                           std::function<void()> onClick)
{
    return insertTab(index, routeKey, text, Fluent::icon(type), onClick);
}

void TabBar::removeTab(int index)
{
    if (index < 0 || index >= m_items.size())
        return;

    // 调整当前索引
    if (index < currentIndex()) {
        m_currentIndex--;
    } else if (index == currentIndex()) {
        if (currentIndex() > 0) {
            setCurrentIndex(currentIndex() - 1);
            emit currentChanged(currentIndex());
        } else if (m_items.size() == 1) {
            m_currentIndex = -1;
        } else {
            setCurrentIndex(1);
            m_currentIndex = 0;
            emit currentChanged(0);
        }
    }

    // 移除标签页
    TabItem *item = m_items.takeAt(index);
    m_itemMap.remove(item->routeKey());
    m_hBoxLayout->removeWidget(item);
    item->deleteLater();

    update();
}

void TabBar::removeTabByKey(const QString &routeKey)
{
    if (!m_itemMap.contains(routeKey))
        return;

    removeTab(m_items.indexOf(tab(routeKey)));
}

void TabBar::setCurrentIndex(int index)
{
    if (index == m_currentIndex)
        return;

    if (currentIndex() >= 0 && currentIndex() < m_items.size())
        m_items[currentIndex()]->setSelected(false);

    m_currentIndex = index;

    if (index >= 0 && index < m_items.size())
        m_items[index]->setSelected(true);
}

void TabBar::setCurrentTab(const QString &routeKey)
{
    if (!m_itemMap.contains(routeKey))
        return;

    setCurrentIndex(m_items.indexOf(tab(routeKey)));
}

TabItem *TabBar::currentTab() const
{
    return tabItem(currentIndex());
}

void TabBar::onItemPressed()
{
    TabItem *pressedItem = qobject_cast<TabItem *>(sender());
    if (!pressedItem)
        return;

    for (TabItem *item : std::as_const(m_items)) {
        item->setSelected(item == pressedItem);
    }

    const int index = m_items.indexOf(pressedItem);
    emit tabBarClicked(index);

    if (index != currentIndex()) {
        setCurrentIndex(index);
        emit currentChanged(index);
    }
}

void TabBar::setCloseButtonDisplayMode(TabCloseButtonDisplayMode mode)
{
    if (mode == m_closeButtonDisplayMode)
        return;

    m_closeButtonDisplayMode = mode;
    for (TabItem *item : std::as_const(m_items)) {
        item->setCloseButtonDisplayMode(mode);
    }
}

TabItem *TabBar::tabItem(int index) const
{
    if (index >= 0 && index < m_items.size())
        return m_items[index];
    return nullptr;
}

TabItem *TabBar::tab(const QString &routeKey) const
{
    return m_itemMap.value(routeKey, nullptr);
}

QRect TabBar::tabRegion() const
{
    return m_itemLayout->geometry();
}

QRect TabBar::tabRect(int index) const
{
    if (index < 0 || index >= m_items.size())
        return QRect();

    int x = 0;
    for (int i = 0; i < index; ++i) {
        x += tabItem(i)->width();
    }

    QRect rect = tabItem(index)->geometry();
    rect.moveLeft(x);
    return rect;
}

QString TabBar::tabText(int index) const
{
    const TabItem *item = tabItem(index);
    return item ? item->text() : QString();
}

QIcon TabBar::tabIcon(int index) const
{
    const TabItem *item = tabItem(index);
    return item ? item->icon() : QIcon();
}

QString TabBar::tabToolTip(int index) const
{
    const TabItem *item = tabItem(index);
    return item ? item->toolTip() : QString();
}

void TabBar::setTabsClosable(bool closable)
{
    setCloseButtonDisplayMode(closable ? TabCloseButtonDisplayMode::Always
                                       : TabCloseButtonDisplayMode::Never);
}

bool TabBar::tabsClosable() const
{
    return m_closeButtonDisplayMode != TabCloseButtonDisplayMode::Never;
}

void TabBar::setTabIcon(int index, const QIcon &icon)
{
    TabItem *item = tabItem(index);
    if (item)
        item->setIcon(icon);
}

void TabBar::setTabText(int index, const QString &text)
{
    TabItem *item = tabItem(index);
    if (item)
        item->setText(text);
}

void TabBar::setTabVisible(int index, bool visible)
{
    TabItem *item = tabItem(index);
    if (!item)
        return;

    item->setVisible(visible);

    if (visible && currentIndex() < 0) {
        setCurrentIndex(0);
    } else if (!visible) {
        if (currentIndex() > 0) {
            setCurrentIndex(currentIndex() - 1);
            emit currentChanged(currentIndex());
        } else if (m_items.size() == 1) {
            m_currentIndex = -1;
        } else {
            setCurrentIndex(1);
            m_currentIndex = 0;
            emit currentChanged(0);
        }
    }
}

void TabBar::setTabTextColor(int index, const QColor &color)
{
    TabItem *item = tabItem(index);
    if (item)
        item->setTextColor(color);
}

void TabBar::setTabToolTip(int index, const QString &toolTip)
{
    TabItem *item = tabItem(index);
    if (item)
        item->setToolTip(toolTip);
}

void TabBar::setTabSelectedBackgroundColor(const QColor &light, const QColor &dark)
{
    m_lightSelectedBackgroundColor = light;
    m_darkSelectedBackgroundColor = dark;

    for (TabItem *item : std::as_const(m_items)) {
        item->setSelectedBackgroundColor(light, dark);
    }
}

void TabBar::setTabShadowEnabled(bool enabled)
{
    if (enabled == isTabShadowEnabled())
        return;

    m_isTabShadowEnabled = enabled;
    for (TabItem *item : std::as_const(m_items)) {
        item->setShadowEnabled(enabled);
    }
}

void TabBar::paintEvent(QPaintEvent *event)
{
    SingleDirectionScrollArea::paintEvent(event);

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制分隔线
    const QColor color = Theme::isDark() ?
                        QColor(255, 255, 255, 21) : QColor(0, 0, 0, 15);
    painter.setPen(color);

    const int itemCount = m_items.size();
    for (int i = 0; i < itemCount; ++i) {
        TabItem *item = m_items[i];
        bool canDraw = !(item->isHover() || item->isSelected());

        if (i < itemCount - 1) {
            TabItem *nextItem = m_items[i + 1];
            if (nextItem->isHover() || nextItem->isSelected())
                canDraw = false;
        }

        if (canDraw) {
            const int x = item->geometry().right();
            const int y = height() / 2 - 8;
            painter.drawLine(x, y, x, y + 16);
        }
    }
}

void TabBar::setMovable(bool movable)
{
    m_isMovable = movable;
}

void TabBar::setScrollable(bool scrollable)
{
    m_isScrollable = scrollable;
    const int width = scrollable ? m_tabMaxWidth : m_tabMinWidth;
    for (TabItem *item : std::as_const(m_items)) {
        item->setMinimumWidth(width);
    }
}

void TabBar::setTabMaximumWidth(int width)
{
    if (width == m_tabMaxWidth)
        return;

    m_tabMaxWidth = width;
    for (TabItem *item : std::as_const(m_items)) {
        item->setMaximumWidth(width);
    }
}

void TabBar::setTabMinimumWidth(int width)
{
    if (width == m_tabMinWidth)
        return;

    m_tabMinWidth = width;

    if (!isScrollable()) {
        for (TabItem *item : std::as_const(m_items)) {
            item->setMinimumWidth(width);
        }
    }
}

void TabBar::mousePressEvent(QMouseEvent *event)
{
    SingleDirectionScrollArea::mousePressEvent(event);

    if (!isMovable() || event->button() != Qt::LeftButton ||
        !m_itemLayout->geometry().contains(event->pos())) {
        return;
    }

    // 新增:如果正在调整布局,先完成调整
    if (m_isAdjustingLayout) {
        forceFinishAdjustment();
    }

    m_dragPos = event->pos();
}

void TabBar::mouseMoveEvent(QMouseEvent *event)
{
    SingleDirectionScrollArea::mouseMoveEvent(event);

    if (!isMovable() || count() <= 1 ||
        !m_itemLayout->geometry().contains(event->pos())) {
        return;
    }

    const int index = currentIndex();
    TabItem *item = tabItem(index);
    if (!item)
        return;

    const int dx = event->pos().x() - m_dragPos.x();

    // 新增:忽略非常小的移动,减少抖动
    if (qAbs(dx) < 2)
        return;

    m_dragPos = event->pos();

    // 第一个标签页不能向左移动
    if (index == 0 && dx < 0 && item->x() <= 0)
        return;

    // 最后一个标签页不能向右移动
    if (index == count() - 1 && dx > 0 &&
        item->geometry().right() >= m_itemLayout->sizeHint().width()) {
        return;
    }

    item->move(item->x() + dx, item->y());
    m_isDragging = true;

    // 向左移动左侧的兄弟项
    if (dx < 0 && index > 0) {
        const int siblingIndex = index - 1;
        if (item->x() < tabItem(siblingIndex)->geometry().center().x())
            swapItem(siblingIndex);
    }
    // 向右移动右侧的兄弟项
    else if (dx > 0 && index < count() - 1) {
        const int siblingIndex = index + 1;
        if (item->geometry().right() >
            tabItem(siblingIndex)->geometry().center().x()) {
            swapItem(siblingIndex);
        }
    }
}

void TabBar::mouseReleaseEvent(QMouseEvent *event)
{
    SingleDirectionScrollArea::mouseReleaseEvent(event);

    if (!isMovable() || !m_isDragging)
        return;

    m_isDragging = false;

    TabItem *item = tabItem(currentIndex());
    if (!item)
        return;

    const int targetX = tabRect(currentIndex()).x();
    const int duration = qAbs(item->x() - targetX) * 250 / item->width();

    // 修改:如果距离很小,直接调整布局,不使用动画
    if (duration < 50) {
        item->move(targetX, item->y());
        adjustLayout();
    } else {
        item->slideTo(targetX, duration);
        connect(item->slideAnimation(), &QPropertyAnimation::finished,
                this, &TabBar::adjustLayout, Qt::UniqueConnection);
    }
}

void TabBar::adjustLayout()
{
    // 防止重复调用
    if (m_isAdjustingLayout)
        return;

    m_isAdjustingLayout = true;

    // 断开信号连接
    if (sender()) {
        sender()->disconnect(this);
    }

    // 停止所有正在运行的动画
    for (TabItem *item : std::as_const(m_items)) {
        if (item->slideAnimation()->state() == QAbstractAnimation::Running) {
            item->slideAnimation()->stop();
        }
    }

    // 重新添加到布局
    for (TabItem *item : std::as_const(m_items)) {
        m_itemLayout->removeWidget(item);
    }

    for (TabItem *item : std::as_const(m_items)) {
        m_itemLayout->addWidget(item, 1);
    }

    m_isAdjustingLayout = false;
}

void TabBar::swapItem(int index)
{
    TabItem *swappedItem = tabItem(index);
    if (!swappedItem)
        return;

    const int targetX = tabRect(currentIndex()).x();

    m_items.swapItemsAt(currentIndex(), index);
    m_currentIndex = index;

    // 修改:使用更短的动画时间,减少拖动延迟感
    swappedItem->slideTo(targetX, 150);
}

void TabBar::forceFinishAdjustment()
{
    // 停止所有动画
    for (TabItem *item : std::as_const(m_items)) {
        if (item->slideAnimation()->state() == QAbstractAnimation::Running) {
            item->slideAnimation()->stop();
        }
    }

    // 如果正在调整,立即完成
    if (m_isAdjustingLayout) {
        m_isAdjustingLayout = false;
    }

    // 确保布局正确
    adjustLayout();
}
