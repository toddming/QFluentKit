#include "TabView.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QApplication>
#include <QFontMetrics>
#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QColor>
#include <QIcon>
#include <QPoint>

#include "StyleSheet.h"

// ==================== TabToolButton ====================

TabToolButton::TabToolButton(const FluentIconBase& icon, QWidget* parent)
    : TransparentToolButton(icon, parent) {
    setFixedSize(32, 24);
    setIconSize(QSize(12, 12));
}

TabToolButton::TabToolButton(QWidget* parent)
    : TransparentToolButton(parent) {
    setFixedSize(32, 24);
    setIconSize(QSize(12, 12));
}


void TabToolButton::drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme) {
    QString color = Theme::instance()->isDarkTheme() ? "#eaeaea" : "#484848";
    QMap<QString, QString> attrs;
    attrs["fill"] = color;
    FluentIconUtils::drawIcon(*fluentIcon(), painter, rect, Fluent::ThemeMode::AUTO,  QIcon::Off, attrs);
}

// ==================== TabItem ====================

TabItem::TabItem(const QString& text, QWidget* parent, const QIcon& icon)
    : QPushButton(text, parent),
      borderRadius(5),
      isSelected(false),
      isShadowEnabled(true),
      closeButtonDisplayMode(TabCloseButtonDisplayMode::ALWAYS),
      lightSelectedBackgroundColor(249, 249, 249),
      darkSelectedBackgroundColor(40, 40, 40),
      m_icon(icon) {

    closeButton = new TabToolButton(FluentIcon(Fluent::IconType::CLOSE), this);
    shadowEffect = new QGraphicsDropShadowEffect(this);
    slideAni = new QPropertyAnimation(this, "pos", this);

    postInit();
}

void TabItem::postInit() {
    initWidget();
}

void TabItem::initWidget() {
    // setFont(this, 12);
    setFixedHeight(36);
    // setMaximumWidth(240);
    // setMinimumWidth(64);
    setAttribute(Qt::WA_LayoutUsesWidgetRect);

    closeButton->setIconSize(QSize(10, 10));

    shadowEffect->setBlurRadius(5);
    shadowEffect->setOffset(0, 1);
    setGraphicsEffect(shadowEffect);
    setSelected(false);

    connect(closeButton, &TabToolButton::clicked, this, &TabItem::closed);
}

void TabItem::slideTo(int x, int duration) {
    slideAni->setStartValue(pos());
    slideAni->setEndValue(QPoint(x, y()));
    slideAni->setDuration(duration);
    slideAni->setEasingCurve(QEasingCurve::InOutQuad);
    slideAni->start();
}

void TabItem::setShadowEnabled(bool isEnabled) {
    if (isEnabled == isShadowEnabled)
        return;

    isShadowEnabled = isEnabled;
    shadowEffect->setColor(QColor(0, 0, 0, 50 * canShowShadow()));
}

bool TabItem::canShowShadow() const {
    return isSelected && isShadowEnabled;
}

void TabItem::setRouteKey(const QString& key) {
    m_routeKey = key;
}

QString TabItem::routeKey() const {
    return m_routeKey;
}

void TabItem::setBorderRadius(int radius) {
    borderRadius = radius;
    update();
}

void TabItem::setSelected(bool selected) {
    isSelected = selected;
    shadowEffect->setColor(QColor(0, 0, 0, 50 * canShowShadow()));
    update();

    if (selected)
        raise();

    if (closeButtonDisplayMode == TabCloseButtonDisplayMode::ON_HOVER)
        closeButton->setVisible(selected);
}

void TabItem::setCloseButtonDisplayMode(TabCloseButtonDisplayMode mode) {
    if (mode == closeButtonDisplayMode)
        return;

    closeButtonDisplayMode = mode;

    if (mode == TabCloseButtonDisplayMode::NEVER)
        closeButton->hide();
    else if (mode == TabCloseButtonDisplayMode::ALWAYS)
        closeButton->show();
    else
        closeButton->setVisible(isHover || isSelected);
}

void TabItem::setTextColor(const QColor& color) {
    textColor = color;
    update();
}

void TabItem::setSelectedBackgroundColor(const QColor& light, const QColor& dark) {
    lightSelectedBackgroundColor = light;
    darkSelectedBackgroundColor = dark;
    update();
}

QPropertyAnimation* TabItem::getSlideAni()
{
    return slideAni;
}

void TabItem::resizeEvent(QResizeEvent* event) {
    QPushButton::resizeEvent(event);
    closeButton->move(width() - 6 - closeButton->width(),
                      height() / 2 - closeButton->height() / 2);
}

void TabItem::enterEvent(QEnterEvent* event) {
    QPushButton::enterEvent(event);
    isHover = true;
    if (closeButtonDisplayMode == TabCloseButtonDisplayMode::ON_HOVER)
        closeButton->show();
}

void TabItem::leaveEvent(QEvent* event) {
    QPushButton::leaveEvent(event);
    isHover = false;
    if (closeButtonDisplayMode == TabCloseButtonDisplayMode::ON_HOVER && !isSelected)
        closeButton->hide();
}

void TabItem::mousePressEvent(QMouseEvent* event) {
    QPushButton::mousePressEvent(event);
    isPressed = true;
    forwardMouseEvent(event);
}

void TabItem::mouseMoveEvent(QMouseEvent* event) {
    QPushButton::mouseMoveEvent(event);
    forwardMouseEvent(event);
}

void TabItem::mouseReleaseEvent(QMouseEvent* event) {
    QPushButton::mouseReleaseEvent(event);
    isPressed = false;
    forwardMouseEvent(event);
}

void TabItem::forwardMouseEvent(QMouseEvent* event) {
    QPoint pos = mapToParent(event->pos());
    QMouseEvent* newEvent = new QMouseEvent(event->type(), pos, event->button(),
                                            event->buttons(), event->modifiers());
    QApplication::sendEvent(parentWidget(), newEvent);
    delete newEvent;
}

QSize TabItem::sizeHint() const {
    return QSize(maximumWidth(), 36);
}

void TabItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (isSelected)
        drawSelectedBackground(&painter);
    else
        drawNotSelectedBackground(&painter);

    // Draw icon
    if (!isSelected)
        painter.setOpacity(Theme::instance()->isDarkTheme() ? 0.79 : 0.61);

    // drawIcon(m_icon, &painter, QRectF(10, 10, 16, 16));
    m_icon.paint(&painter, 10, 10, 16, 16, Qt::AlignCenter);
    // Draw text
    drawText(&painter);
}

void TabItem::drawSelectedBackground(QPainter* painter) {
    int w = width();
    int h = height();
    int r = borderRadius;
    int d = 2 * r;

    bool isDark = Theme::instance()->isDarkTheme();

    // Draw top border
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
        if (isPressed)
            topBorderColor = QColor(255, 255, 255, 18);
        else if (isHover)
            topBorderColor = QColor(255, 255, 255, 13);
    } else {
        topBorderColor = QColor(0, 0, 0, 16);
    }

    painter->strokePath(path, topBorderColor);

    // Draw bottom border
    QPainterPath bottomPath;
    bottomPath.arcMoveTo(1, h - d - 1, d, d, 225);
    bottomPath.arcTo(1, h - d - 1, d, d, 225, 45);
    bottomPath.lineTo(w - r - 1, h - 1);
    bottomPath.arcTo(w - d - 1, h - d - 1, d, d, 270, 45);

    QColor bottomBorderColor = topBorderColor;
    if (!isDark)
        bottomBorderColor = QColor(0, 0, 0, 63);

    painter->strokePath(bottomPath, bottomBorderColor);

    // Draw background
    painter->setPen(Qt::NoPen);
    QRect rect = this->rect().adjusted(1, 1, -1, -1);
    painter->setBrush(isDark ? darkSelectedBackgroundColor : lightSelectedBackgroundColor);
    painter->drawRoundedRect(rect, r, r);
}

void TabItem::drawNotSelectedBackground(QPainter* painter) {
    if (!isPressed && !isHover)
        return;

    bool isDark = Theme::instance()->isDarkTheme();
    QColor color;

    if (isPressed)
        color = isDark ? QColor(255, 255, 255, 12) : QColor(0, 0, 0, 7);
    else
        color = isDark ? QColor(255, 255, 255, 15) : QColor(0, 0, 0, 10);

    painter->setBrush(color);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect().adjusted(1, 1, -1, -1), borderRadius, borderRadius);
}

void TabItem::drawText(QPainter* painter) {
    QFontMetrics fm(font());
    int tw = fm.horizontalAdvance(text());

    QRectF rect;
    if (m_icon.isNull()) {
        int dw = closeButton->isVisible() ? 47 : 20;
        rect = QRectF(10, 0, width() - dw, height());
    } else {
        int dw = closeButton->isVisible() ? 70 : 45;
        rect = QRectF(33, 0, width() - dw, height());
    }

    QPen pen;
    QColor color = Theme::instance()->isDarkTheme() ? Qt::white : Qt::black;
    if (textColor.isValid())
        color = textColor;

    qreal rw = rect.width();

    if (tw > rw) {
        QLinearGradient gradient(rect.x(), 0, tw + rect.x(), 0);
        gradient.setColorAt(0, color);
        gradient.setColorAt(qMax(0.0, (rw - 10) / tw), color);
        gradient.setColorAt(qMax(0.0, rw / tw), Qt::transparent);
        gradient.setColorAt(1, Qt::transparent);
        pen.setBrush(QBrush(gradient));
    } else {
        pen.setColor(color);
    }

    painter->setPen(pen);
    painter->setFont(font());
    painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, text());
}

// ==================== TabBar ====================

TabBar::TabBar(QWidget* parent)
    : SingleDirectionScrollArea(parent, Qt::Horizontal),
      m_currentIndex(-1),
      m_isMovable(true),
      m_isScrollable(false),
      m_isTabShadowEnabled(true),
      m_tabMaxWidth(240),
      m_tabMinWidth(64),
      isDraging(false),
      lightSelectedBackgroundColor(249, 249, 249),
      darkSelectedBackgroundColor(40, 40, 40),
      closeButtonDisplayMode(TabCloseButtonDisplayMode::ALWAYS) {

    view = new QWidget(this);
    hBoxLayout = new QHBoxLayout(view);
    itemLayout = new QHBoxLayout();
    widgetLayout = new QHBoxLayout();
    addButton = new TabToolButton(FluentIcon(Fluent::IconType::ADD), this);

    initWidget();
}

void TabBar::initWidget() {
    setFixedHeight(46);
    setWidget(view);
    setWidgetResizable(true);
    setViewportMargins(0, 0, 0, 0);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    hBoxLayout->setSizeConstraint(QHBoxLayout::SetMaximumSize);

    connect(addButton, &TabToolButton::clicked, this, &TabBar::tabAddRequested);

    view->setObjectName("view");
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::TAB_VIEW);
    StyleSheetManager::instance()->registerWidget(view, Fluent::ThemeStyle::TAB_VIEW);

    initLayout();
}

void TabBar::initLayout() {
    hBoxLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    itemLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    widgetLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    itemLayout->setContentsMargins(5, 5, 5, 5);
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);

    itemLayout->setSizeConstraint(QHBoxLayout::SetMinAndMaxSize);

    hBoxLayout->setSpacing(0);
    itemLayout->setSpacing(0);

    hBoxLayout->addLayout(itemLayout);
    hBoxLayout->addSpacing(3);

    widgetLayout->addWidget(addButton, 0, Qt::AlignLeft);
    hBoxLayout->addLayout(widgetLayout);
    hBoxLayout->addStretch(1);
}

void TabBar::setAddButtonVisible(bool isVisible) {
    addButton->setVisible(isVisible);
}

TabItem* TabBar::addTab(const QString& routeKey, const QString& text, const QIcon& icon,
                        std::function<void()> onClick) {
    return insertTab(-1, routeKey, text, icon, onClick);
}

TabItem* TabBar::insertTab(int index, const QString& routeKey, const QString& text,
                           const QIcon& icon, std::function<void()> onClick) {
    if (itemMap.contains(routeKey))
        throw std::runtime_error(QString("The route key `%1` is duplicated.").arg(routeKey).toStdString());

    if (index == -1)
        index = items.size();

    // Adjust current index
    if (index <= currentIndex() && currentIndex() >= 0)
        m_currentIndex++;

    TabItem* item = new TabItem(text, view, icon);
    item->setRouteKey(routeKey);

    // Set the size of tab
    int w = isScrollable() ? tabMaximumWidth() : tabMinimumWidth();
    item->setMinimumWidth(w);
    item->setMaximumWidth(tabMaximumWidth());

    item->setShadowEnabled(isTabShadowEnabled());
    item->setCloseButtonDisplayMode(closeButtonDisplayMode);
    item->setSelectedBackgroundColor(lightSelectedBackgroundColor, darkSelectedBackgroundColor);

    connect(item, &TabItem::pressed, this, &TabBar::onItemPressed);
    connect(item, &TabItem::closed, this, [this, item]() {
        emit tabCloseRequested(items.indexOf(item));
    });

    if (onClick) {
        connect(item, &TabItem::pressed, this, onClick);
    }

    itemLayout->insertWidget(index, item, 1);
    items.insert(index, item);
    itemMap[routeKey] = item;

    if (items.size() == 1)
        setCurrentIndex(0);

    return item;
}

void TabBar::removeTab(int index) {
    if (index < 0 || index >= items.size())
        return;

    // Adjust current index
    if (index < currentIndex()) {
        m_currentIndex--;
    } else if (index == currentIndex()) {
        if (currentIndex() > 0) {
            setCurrentIndex(currentIndex() - 1);
            emit currentChanged(currentIndex());
        } else if (items.size() == 1) {
            m_currentIndex = -1;
        } else {
            setCurrentIndex(1);
            m_currentIndex = 0;
            emit currentChanged(0);
        }
    }

    // Remove tab
    TabItem* item = items.takeAt(index);
    itemMap.remove(item->routeKey());
    hBoxLayout->removeWidget(item);
    item->deleteLater();

    update();
}

void TabBar::removeTabByKey(const QString& routeKey) {
    if (!itemMap.contains(routeKey))
        return;

    removeTab(items.indexOf(tab(routeKey)));
}

void TabBar::setCurrentIndex(int index) {
    if (index == m_currentIndex)
        return;

    if (currentIndex() >= 0)
        items[currentIndex()]->setSelected(false);

    m_currentIndex = index;
    items[index]->setSelected(true);
}

void TabBar::setCurrentTab(const QString& routeKey) {
    if (!itemMap.contains(routeKey))
        return;

    setCurrentIndex(items.indexOf(tab(routeKey)));
}

int TabBar::currentIndex() const {
    return m_currentIndex;
}

TabItem* TabBar::currentTab() {
    return tabItem(currentIndex());
}

void TabBar::onItemPressed() {
    for (TabItem* item : items)
        item->setSelected(item == sender());

    int index = items.indexOf(qobject_cast<TabItem*>(sender()));
    emit tabBarClicked(index);

    if (index != currentIndex()) {
        setCurrentIndex(index);
        emit currentChanged(index);
    }
}

void TabBar::setCloseButtonDisplayMode(TabCloseButtonDisplayMode mode) {
    if (mode == closeButtonDisplayMode)
        return;

    closeButtonDisplayMode = mode;
    for (TabItem* item : items)
        item->setCloseButtonDisplayMode(mode);
}

TabItem* TabBar::tabItem(int index) {
    if (index >= 0 && index < items.size())
        return items[index];
    return nullptr;
}

TabItem* TabBar::tab(const QString& routeKey) {
    return itemMap.value(routeKey, nullptr);
}

QRect TabBar::tabRegion() const {
    return itemLayout->geometry();
}

QRect TabBar::tabRect(int index) {
    if (index < 0 || index >= items.size())
        return QRect();

    int x = 0;
    for (int i = 0; i < index; i++)
        x += tabItem(i)->width();

    QRect rect = tabItem(index)->geometry();
    rect.moveLeft(x);
    return rect;
}

QString TabBar::tabText(int index) {
    TabItem* item = tabItem(index);
    return item ? item->text() : QString();
}

QIcon TabBar::tabIcon(int index) {
    TabItem* item = tabItem(index);
    return item ? item->icon() : QIcon();
}

QString TabBar::tabToolTip(int index) {
    TabItem* item = tabItem(index);
    return item ? item->toolTip() : QString();
}

void TabBar::setTabsClosable(bool isClosable) {
    setCloseButtonDisplayMode(isClosable ? TabCloseButtonDisplayMode::ALWAYS
                                         : TabCloseButtonDisplayMode::NEVER);
}

bool TabBar::tabsClosable() const {
    return closeButtonDisplayMode != TabCloseButtonDisplayMode::NEVER;
}

void TabBar::setTabIcon(int index, const QIcon& icon) {
    TabItem* item = tabItem(index);
    if (item)
        item->setIcon(icon);
}

void TabBar::setTabText(int index, const QString& text) {
    TabItem* item = tabItem(index);
    if (item)
        item->setText(text);
}

void TabBar::setTabVisible(int index, bool isVisible) {
    TabItem* item = tabItem(index);
    if (!item)
        return;

    item->setVisible(isVisible);

    if (isVisible && currentIndex() < 0) {
        setCurrentIndex(0);
    } else if (!isVisible) {
        if (currentIndex() > 0) {
            setCurrentIndex(currentIndex() - 1);
            emit currentChanged(currentIndex());
        } else if (items.size() == 1) {
            m_currentIndex = -1;
        } else {
            setCurrentIndex(1);
            m_currentIndex = 0;
            emit currentChanged(0);
        }
    }
}

void TabBar::setTabTextColor(int index, const QColor& color) {
    TabItem* item = tabItem(index);
    if (item)
        item->setTextColor(color);
}

void TabBar::setTabToolTip(int index, const QString& toolTip) {
    TabItem* item = tabItem(index);
    if (item)
        item->setToolTip(toolTip);
}

void TabBar::setTabSelectedBackgroundColor(const QColor& light, const QColor& dark) {
    lightSelectedBackgroundColor = light;
    darkSelectedBackgroundColor = dark;

    for (TabItem* item : items)
        item->setSelectedBackgroundColor(light, dark);
}

void TabBar::setTabShadowEnabled(bool isEnabled) {
    if (isEnabled == isTabShadowEnabled())
        return;

    m_isTabShadowEnabled = isEnabled;
    for (TabItem* item : items)
        item->setShadowEnabled(isEnabled);
}

bool TabBar::isTabShadowEnabled() const {
    return m_isTabShadowEnabled;
}

void TabBar::paintEvent(QPaintEvent* event) {
    SingleDirectionScrollArea::paintEvent(event);

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw separators
    QColor color = Theme::instance()->isDarkTheme() ? QColor(255, 255, 255, 21) : QColor(0, 0, 0, 15);
    painter.setPen(color);

    for (int i = 0; i < items.size(); i++) {
        TabItem* item = items[i];
        bool canDraw = !(item->isHover || item->isSelected);

        if (i < items.size() - 1) {
            TabItem* nextItem = items[i + 1];
            if (nextItem->isHover || nextItem->isSelected)
                canDraw = false;
        }

        if (canDraw) {
            int x = item->geometry().right();
            int y = height() / 2 - 8;
            painter.drawLine(x, y, x, y + 16);
        }
    }
}

void TabBar::setMovable(bool movable) {
    m_isMovable = movable;
}

bool TabBar::isMovable() const {
    return m_isMovable;
}

void TabBar::setScrollable(bool scrollable) {
    m_isScrollable = scrollable;
    int w = scrollable ? m_tabMaxWidth : m_tabMinWidth;
    for (TabItem* item : items)
        item->setMinimumWidth(w);
}

void TabBar::setTabMaximumWidth(int width) {
    if (width == m_tabMaxWidth)
        return;

    m_tabMaxWidth = width;
    for (TabItem* item : items)
        item->setMaximumWidth(width);
}

void TabBar::setTabMinimumWidth(int width) {
    if (width == m_tabMinWidth)
        return;

    m_tabMinWidth = width;

    if (!isScrollable()) {
        for (TabItem* item : items)
            item->setMinimumWidth(width);
    }
}

int TabBar::tabMaximumWidth() const {
    return m_tabMaxWidth;
}

int TabBar::tabMinimumWidth() const {
    return m_tabMinWidth;
}

bool TabBar::isScrollable() const {
    return m_isScrollable;
}

int TabBar::count() const {
    return items.size();
}

void TabBar::mousePressEvent(QMouseEvent* event) {
    SingleDirectionScrollArea::mousePressEvent(event);

    if (!isMovable() || event->button() != Qt::LeftButton ||
        !itemLayout->geometry().contains(event->pos()))
        return;

    dragPos = event->pos();
}

void TabBar::mouseMoveEvent(QMouseEvent* event) {
    SingleDirectionScrollArea::mouseMoveEvent(event);

    if (!isMovable() || count() <= 1 || !itemLayout->geometry().contains(event->pos()))
        return;

    int index = currentIndex();
    TabItem* item = tabItem(index);
    int dx = event->pos().x() - dragPos.x();
    dragPos = event->pos();

    // First tab can't move left
    if (index == 0 && dx < 0 && item->x() <= 0)
        return;

    // Last tab can't move right
    if (index == count() - 1 && dx > 0 &&
        item->geometry().right() >= itemLayout->sizeHint().width())
        return;

    item->move(item->x() + dx, item->y());
    isDraging = true;

    if (dx < 0 && index > 0) {
        int siblingIndex = index - 1;
        if (item->x() < tabItem(siblingIndex)->geometry().center().x())
            swapItem(siblingIndex);
    }
    // Move the right sibling item to left
    else if (dx > 0 && index < count() - 1) {
        int siblingIndex = index + 1;
        if (item->geometry().right() > tabItem(siblingIndex)->geometry().center().x())
            swapItem(siblingIndex);
    }
}

void TabBar::mouseReleaseEvent(QMouseEvent* event) {
    SingleDirectionScrollArea::mouseReleaseEvent(event);

    if (!isMovable() || !isDraging)
        return;

    isDraging = false;

    TabItem* item = tabItem(currentIndex());
    int x = tabRect(currentIndex()).x();
    int duration = qAbs(item->x() - x) * 250 / item->width();
    item->slideTo(x, duration);
    connect(item->getSlideAni(), &QPropertyAnimation::finished, this, &TabBar::adjustLayout);
}

void TabBar::adjustLayout() {
    sender()->disconnect();

    for (TabItem* item : items)
        itemLayout->removeWidget(item);

    for (TabItem* item : items)
        itemLayout->addWidget(item);
}

void TabBar::swapItem(int index) {
    TabItem* swappedItem = tabItem(index);
    int x = tabRect(currentIndex()).x();

    items.swapItemsAt(currentIndex(), index);
    m_currentIndex = index;
    swappedItem->slideTo(x);
}
