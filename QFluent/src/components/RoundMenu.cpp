#include "RoundMenu.h"
#include <QApplication>
#include <QPainter>
#include <QScreen>
#include <QHoverEvent>
#include <QStyledItemDelegate>
#include <QScrollBar>
#include <QDebug>
#include <QVBoxLayout>
#include <QStyleOption>

#include "../common/Screen.h"
#include "scrollbar/ScrollBar.h"


///////////////////////MenuItemDelegate类/////////////////////
MenuItemDelegate::MenuItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void MenuItemDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    if (!isSeparator(index)) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // ===== 分隔符绘制逻辑 =====
    painter->save();

    // 根据主题设置分隔符颜色（亮色主题：黑色25%透明度；暗色主题：白色25%透明度）
    int c = sTheme->isDarkMode() ? 255 : 0;
    QPen pen(QColor(c, c, c, 25), 1);
    pen.setCosmetic(true);  // 保持1像素不受缩放影响
    painter->setPen(pen);

    // 计算分隔符位置（垂直居中+4px偏移）
    QRect rect = option.rect;
    int yPos = rect.y() + 4;  // 垂直居中偏移调整

    // 绘制横贯菜单宽度的分隔线（+12px扩展确保覆盖边距）
    painter->drawLine(0, yPos, rect.width() + 12, yPos);

    painter->restore();
}

bool MenuItemDelegate::isSeparator(const QModelIndex &index) const
{
    QVariant data = index.data(Qt::DecorationRole);
    return data.isValid() && data.toString() == "separator";
}


/////////////////////////ShortcutMenuItemDelegate/////////////////////////

ShortcutMenuItemDelegate::ShortcutMenuItemDelegate(QObject *parent)
    : MenuItemDelegate(parent)
{
}

void ShortcutMenuItemDelegate::paint(QPainter *painter,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    // 1. 先调用基类绘制基础菜单项
    MenuItemDelegate::paint(painter, option, index);

    // 2. 跳过分隔符项
    if (isSeparator(index))
        return;

    // 3. 获取关联的 QAction 对象
    QVariant actionData = index.data(Qt::UserRole);
    if (!actionData.isValid())
        return;

    QAction *action = qvariant_cast<QAction*>(actionData);
    if (!action || action->shortcut().isEmpty())
        return;

    // 4. 保存绘图状态
    painter->save();

    // 5. 处理禁用状态的透明度
    if (!(option.state & QStyle::State_Enabled)) {
        painter->setOpacity(sTheme->isDarkMode() ? 0.5 : 0.6);  // 深色/浅色主题不同透明度
    }

    // 6. 设置字体和颜色
    QFont font = painter->font();
    painter->setFont(font);
    painter->setPen(sTheme->isDarkMode() ?
                        QColor(255, 255, 255, 200) :  // 深色主题：白色80%透明度
                        QColor(0, 0, 0, 153));        // 浅色主题：黑色60%透明度

    // 7. 计算快捷键文本位置
    QFontMetrics fm(font);
    QString shortcut = action->shortcut().toString(QKeySequence::NativeText);
    int shortcutWidth = fm.horizontalAdvance(shortcut);

    // 8. 坐标变换：将原点平移到右侧
    painter->translate(option.rect.width() - shortcutWidth - 20, 0);

    // 9. 绘制快捷键文本（左对齐+垂直居中）
    QRectF textRect(0, option.rect.y(),
                    shortcutWidth, option.rect.height());
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, shortcut);

    // 10. 恢复绘图状态
    painter->restore();
}

//////////////////IndicatorMenuItemDelegate///////////////////////////

IndicatorMenuItemDelegate::IndicatorMenuItemDelegate(QObject *parent)
    : MenuItemDelegate(parent)
{

}

void IndicatorMenuItemDelegate::paint(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{

    MenuItemDelegate::paint(painter, option, index);
    if (!(option.state & QStyle::State_Selected))
        return;

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    painter->setPen(Qt::NoPen);
    painter->setBrush(sTheme->themeColor());
    qreal y_offset = (option.rect.height() - 15) / 2.0 + 2;
    painter->drawRoundedRect(6.0, option.rect.y() + y_offset, 3.0, 15.0, 1.5, 1.5);

    painter->restore();
}

// ====================== MenuActionListWidget 实现 ======================
MenuActionListWidget::MenuActionListWidget(QWidget* parent)
    : QListWidget(parent) {

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ScrollBar* scrollBar = new ScrollBar(Qt::Vertical, this);
    setVerticalScrollBar(scrollBar);

    setItemDelegate(new ShortcutMenuItemDelegate(this));

    setViewportMargins(0, 6, 0, 6);
    setTextElideMode(Qt::ElideNone);
    setMouseTracking(true);
    setIconSize(QSize(14, 14));

    // setProperty("transparent", true);
    setAttribute(Qt::WA_Hover, true);
    installEventFilter(this);
}

void MenuActionListWidget::setItemHeight(int height) {
    if (m_itemHeight == height) return;
    m_itemHeight = height;
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem* item = this->item(i);
        item->setSizeHint(QSize(item->sizeHint().width(), height));
    }
    adjustSize();
}

void MenuActionListWidget::setMaxVisibleItems(int num) {
    m_maxVisibleItems = num;
    adjustSize();
}

int MenuActionListWidget::maxVisibleItems() const {
    return m_maxVisibleItems;
}

void MenuActionListWidget::adjustSize(const QPoint& pos, MenuAnimationType aniType) {
    QSize size(0, 0);

    // 计算内容大小
    for (int i = 0; i < count(); ++i) {
        QSize itemSize = item(i)->sizeHint();
        size.setWidth(qMax(itemSize.width(), size.width()));
        size.setHeight(size.height() + itemSize.height());
    }

    QPoint point = availableViewSize(pos, aniType);
    int w = point.x();
    int h = point.y();

    QMargins m = viewportMargins();
    size += QSize(m.left()+m.right()+2, m.top()+m.bottom());
    size.setHeight(qMin(h, size.height()+3));
    size.setWidth(qMax(qMin(w, size.width()), minimumWidth()));

    // 限制最大可见项
    if (m_maxVisibleItems > 0) {
        int maxHeight = m_maxVisibleItems * m_itemHeight + m.top() + m.bottom() +  3;

        if (size.height() > maxHeight) {
            size.setHeight(maxHeight);
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    setFixedSize(size);
}

void MenuActionListWidget::wheelEvent(QWheelEvent* e) {
    QScrollBar* vBar = verticalScrollBar();
    int step = e->angleDelta().y() / 10;
    vBar->setValue(vBar->value() - step);
    e->accept();
}

int MenuActionListWidget::heightForAnimation(const QPoint &pos, MenuAnimationType aniType)
{
    int ih = itemsHeight();
    int sh = 0;
    QRect rect = Screen::getCurrentScreenGeometry();

    switch (aniType) {
    case MenuAnimationType::DROP_DOWN:
        sh = qMax(rect.bottom() - pos.y() - 10, 1);
        break;
    case MenuAnimationType::PULL_UP:
        sh = qMax(pos.y() - rect.top() - 28, 1);
        break;
    case MenuAnimationType::FADE_IN_DROP_DOWN:
        sh = qMax(rect.bottom() - pos.y() - 10, 1);
        break;
    case MenuAnimationType::FADE_IN_PULL_UP:
        sh = qMax(pos.y() - rect.top() - 28, 1);
        break;
    default:
        sh = rect.height() - 100;
        break;
    }

    return qMin(ih, sh);
}

int MenuActionListWidget::itemHeight() const
{
    return m_itemHeight;
}

int MenuActionListWidget::itemsHeight() const
{
    int maxVisible = maxVisibleItems();

    int N = (maxVisible < 0) ? count() : qMin(maxVisible, count());

    int h = 0;
    for (int i = 0; i < N; ++i) {
        h += item(i)->sizeHint().height();
    }
    QMargins m = viewportMargins();
    return h + m.top() + m.bottom();
}

QPoint MenuActionListWidget::availableViewSize(const QPoint &pos, MenuAnimationType aniType)
{
    QRect rect = Screen::getCurrentScreenGeometry();
    QPoint point;

    switch (aniType) {
    case MenuAnimationType::DROP_DOWN:
        point.setX(rect.width() - 100);
        point.setY(qMax(rect.bottom() - pos.y() - 10, 1));
        break;
    case MenuAnimationType::PULL_UP:
        point.setX(rect.width() - 100);
        point.setY(qMax(pos.y() - rect.top() - 28, 1));
        break;
    case MenuAnimationType::FADE_IN_DROP_DOWN:
        point.setX(rect.width() - 100);
        point.setY(qMax(rect.bottom() - pos.y() - 10, 1));
        break;
    case MenuAnimationType::FADE_IN_PULL_UP:
        point.setX(rect.width() - 100);
        point.setY(qMax(pos.y() - rect.top() - 28, 1));
        break;
    default:
        point.setX(rect.width() - 100);
        point.setY(rect.height() - 100);
        break;
    }
    return point;
}


// ====================== RoundMenu 实现 ======================
RoundMenu::RoundMenu(const QString& title, QWidget* parent)
    : QMenu(title, parent),
      m_view(new MenuActionListWidget(this)),
      m_layout(new QHBoxLayout(this)),
      m_showTimer(new QTimer(this)) {

    initWidgets();
    m_showTimer->setSingleShot(true);
    m_showTimer->setInterval(400);
    connect(m_showTimer, &QTimer::timeout, this, &RoundMenu::onShowMenuTimeout);
}

RoundMenu::~RoundMenu() {
    QList<QListWidgetItem*> items = m_view->findItems("", Qt::MatchContains);
    for (QListWidgetItem* item : items) {
        if (auto menu = item->data(Qt::UserRole).value<RoundMenu*>()) {
            delete menu;
        }
    }
}

void RoundMenu::initWidgets() {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);


    setMouseTracking(true);

    setItemHeight(36); // 设置合适的项目高度
    setMaxVisibleItems(5); // 设置最大可见项数

    sTheme->registerWidget(this, Theme::StyleSheetType::MENU);

    // 设置布局
    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(12, 8, 12, 20);
    setLayout(m_layout);

    // 连接信号
    connect(m_view, &QListWidget::itemClicked, this, &RoundMenu::handleItemClicked);
    connect(m_view, &QListWidget::itemEntered, this, &RoundMenu::handleItemEntered);
}

void RoundMenu::setItemHeight(int height) {
    m_view->setItemHeight(height);
}

void RoundMenu::setMaxVisibleItems(int num) {
    m_view->setMaxVisibleItems(num);
}


void RoundMenu::setOpacity(qreal opacity) {
    if (qFuzzyCompare(m_opacity, opacity)) return;
    m_opacity = 1;
}

void RoundMenu::addAction(QAction* action) {
    QListWidgetItem *item = createActionItem(action);
    m_view->addItem(item);;
    adjustMenuSize();
}

void RoundMenu::insertAction(QAction* before, QAction* action) {

    for (int i = 0; i < m_view->count(); ++i) {
        QListWidgetItem* item = m_view->item(i);
        if (item->data(Qt::UserRole).value<QAction*>() == before) {
            createActionItem(action);
            QListWidgetItem* newItem = m_view->takeItem(m_view->count() - 1);
            m_view->insertItem(i, newItem);
            return;
        }
    }

    // 如果没找到before项，添加到末尾
    addAction(action);
}

void RoundMenu::removeAction(QAction* action) {
    for (int i = 0; i < m_view->count(); ++i) {
        QListWidgetItem* item = m_view->item(i);
        if (item->data(Qt::UserRole).value<QAction*>() == action) {
            delete m_view->takeItem(i);
            return;
        }
    }
}

void RoundMenu::addMenu(RoundMenu* menu) {
    createSubMenuItem(menu);
    adjustMenuSize();
}

void RoundMenu::addSeparator() {
    QMargins m = m_view->contentsMargins();
    int w = m_view->width()-m.left()-m.right();

    QListWidgetItem* separator = new QListWidgetItem;
    separator->setFlags(Qt::NoItemFlags);
    separator->setSizeHint(QSize(w, 9));
    separator->setData(Qt::DecorationRole, "separator");
    m_view->addItem(separator);
    adjustMenuSize();
    adjustSize();
}

void RoundMenu::clear() {
    m_view->clear();
}

QListWidgetItem* RoundMenu::createActionItem(QAction* action, QAction* before) {

    if (!before) {
        m_actions.append(action);
        QMenu::addAction(action);
    } else if (m_actions.contains(before)) {
        int index = m_actions.indexOf(before);
        m_actions.insert(index, action);
        QMenu::insertAction(before, action);
    } else {
        qDebug() << "`before` is not in the action list";
    }
    QListWidgetItem* item = new QListWidgetItem(action->icon(), action->text());

    item->setSizeHint(QSize(adjustItemText(item, action), m_view->itemHeight()));
    if (!action->isEnabled()) {
        item->setFlags(Qt::NoItemFlags);
    }

    item->setData(Qt::UserRole, QVariant::fromValue(action));
    action->setProperty("item", QVariant::fromValue(item));
    action->setProperty("index", QVariant::fromValue(m_actions.indexOf(action)));

    connect(action, &QAction::changed, this, &RoundMenu::onActionChanged);

    return item;
}

void RoundMenu::onActionChanged()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QListWidgetItem* item = qvariant_cast<QListWidgetItem*>(action->property("item"));
    // item.setIcon(self._createItemIcon(action))

    adjustItemText(item, action);

    if (action->isEnabled()) {
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    } else {
        item->setFlags(Qt::NoItemFlags);
    }
    view()->adjustSize();
    adjustSize();
}


void RoundMenu::createSubMenuItem(RoundMenu* menu) {
    QListWidgetItem* item = new QListWidgetItem(menu->icon(), menu->title());
    item->setData(Qt::UserRole, QVariant::fromValue(menu));
    // 同样使用固定的项目高度
    item->setSizeHint(QSize(120, m_view->itemHeight()));
    m_view->addItem(item);

    // 设置父子关系
    menu->m_parentMenu = this;
    menu->m_menuItem = item;
    menu->m_isSubMenu = true;
}


void RoundMenu::exec(const QPoint& pos, bool animate, MenuAnimationType aniType) {
    if (!animate) aniType = MenuAnimationType::NONE;

    m_animationManager = MenuAnimationManager::make(this, aniType);
    view()->adjustSize(pos, aniType);
    adjustMenuSize();


    show();
    m_animationManager->exec(pos);
}

void RoundMenu::closeParentMenu() {
    if (m_parentMenu) {
        m_parentMenu->close();
    }
}


void RoundMenu::adjustMenuSize() {
    QSize viewSize = m_view->size();
    setFixedSize(viewSize.width() + m_layout->contentsMargins().left() +
                 m_layout->contentsMargins().right(),
                 viewSize.height() + m_layout->contentsMargins().top() +
                 m_layout->contentsMargins().bottom());
}

MenuActionListWidget* RoundMenu::view()
{
    return m_view;
}

void RoundMenu::closeEvent(QCloseEvent* e) {
    Q_UNUSED(e);
    emit closed();
    if (m_parentMenu) {
        m_parentMenu->close();
    }
    m_view->clearSelection();
    m_view->setCurrentItem(nullptr);
    m_view->clearFocus();
}

void RoundMenu::handleItemClicked(QListWidgetItem* item) {
    if (QVariant v = item->data(Qt::UserRole); v.canConvert<QAction*>()) {
        QAction* action = v.value<QAction*>();
        if (action->isEnabled()) {
            action->trigger();

            if (m_view) {
                QRect itemRect = m_view->visualItemRect(item);
                QPointF localPos = itemRect.center();
                QPointF globalPos = m_view->mapToGlobal(localPos.toPoint());
                QHoverEvent hoverLeave(QEvent::HoverLeave, localPos, globalPos, localPos);
                QApplication::sendEvent(m_view->viewport(), &hoverLeave);
                m_view->update();
            }
            close();
        }
    } else if (v.canConvert<RoundMenu*>()) {
        RoundMenu* subMenu = v.value<RoundMenu*>();
        if (subMenu && !subMenu->isVisible()) {
            QPoint globalPos = m_view->visualItemRect(item)
                    .bottomRight() + QPoint(5, 0);
            globalPos = m_view->mapToGlobal(globalPos);
            subMenu->exec(globalPos);
        }
    }
}

void RoundMenu::handleItemEntered(QListWidgetItem* item) {
    m_lastHoverItem = item;
    if (item->data(Qt::UserRole).canConvert<RoundMenu*>()) {
        m_showTimer->start();
    }
}

void RoundMenu::onShowMenuTimeout() {
    if (!m_lastHoverItem) return;

    if (QVariant v = m_lastHoverItem->data(Qt::UserRole);
            v.canConvert<RoundMenu*>()) {
        RoundMenu* subMenu = v.value<RoundMenu*>();
        if (subMenu && !subMenu->isVisible()) {
            QRect itemRect = m_view->visualItemRect(m_lastHoverItem);
            QPoint globalPos = itemRect.topRight() + QPoint(5, 0);
            globalPos = m_view->mapToGlobal(globalPos);
            subMenu->exec(globalPos);
        }
    }
}

void RoundMenu::setIcon(const QIcon &icon)
{
    _icon = icon;
}

QList<QAction*> RoundMenu::menuActions()
{
    return m_actions;
}

void RoundMenu::setDefaultAction(QAction *action)
{
    if (!m_actions.contains(action)) {
        return;
    }
    QListWidgetItem* item = qvariant_cast<QListWidgetItem*>(action->property("item"));
    if (item) {
        m_view->setCurrentItem(item);
    }
}

bool RoundMenu::hasItemIcon() const
{
    for (QAction *action : m_actions) {
        if (!action->icon().isNull()) return true;
    }
    return false;
}

QIcon RoundMenu::createItemIcon(QAction *action) const
{
    if (!action) return QIcon();
    return QIcon();
    // QIcon icon = createIconFromResource(action->icon());
    // if (icon.isNull() && hasItemIcon()) {
    //     QPixmap pixmap(m_view->iconSize());
    //     pixmap.fill(Qt::transparent);
    //     return QIcon(pixmap); // 占位透明图标
    // }
    // return icon;
}

int RoundMenu::longestShortcutWidth() const
{
    QFontMetrics fm(this->font());
    int maxWidth = 0;
    for (QAction *action : m_actions) {
        if (action->shortcut().isEmpty()) continue;
        maxWidth = qMax(maxWidth, fm.horizontalAdvance(action->shortcut().toString(QKeySequence::NativeText)));
    }
    return maxWidth;
}

int RoundMenu::adjustItemText(QListWidgetItem *item, QAction *action)
{
    if (!item || !action) return 0;

    QString text = action->text();
    // 去除快捷键符号（&）
    text.remove('&');

    QFontMetrics fm(item->font());
    int shortcutWidth = longestShortcutWidth();
    if (shortcutWidth > 0) {
        shortcutWidth += 22; // 保留空间
    }

    int width;
    bool hasIcon = hasItemIcon();
    // QIcon itemIcon = createItemIcon(action);
    // item->setIcon(itemIcon);

    if (!hasIcon) {
        width = 40 + fm.horizontalAdvance(text) + shortcutWidth;
    } else {
        // 添加空格以分隔图标和文本
        text = " " + text;
        int space = 4 - fm.horizontalAdvance(" ");
        width = 60 + fm.horizontalAdvance(text) + shortcutWidth + space;
    }


    return width;
}

void RoundMenu::paintEvent(QPaintEvent *)
{

}

void RoundMenu::hideMenu(bool isHideBySystem)
{
    view()->clearSelection();
    if (m_isSubMenu) {
        hide();
    } else {
        close();
    }
}

void RoundMenu::mouseMoveEvent(QMouseEvent* e) {
    if (!m_isSubMenu) {
        return;
    }

    QPoint pos = e->globalPosition().toPoint();

    // get the rect of menu item
    QMargins margin = m_view->contentsMargins();
    QRect rect = m_view->visualItemRect(m_menuItem)
                     .translated(m_view->mapToGlobal(QPoint(0, 0)));
    rect.translate(margin.left(), margin.top() + 2);

    if (m_view->geometry().contains(pos) &&
        !rect.contains(pos) &&
        !this->geometry().contains(pos)) {
        m_view->clearSelection();
        hideMenu(false);
    }
}

void RoundMenu::mousePressEvent(QMouseEvent* e) {
    QWidget* w = this->childAt(e->pos());
    if (w != m_view && !m_view->isAncestorOf(w)) {
        hideMenu(true);
    }
}


void RoundMenu::addWidget(QWidget *widget, bool selectable)
{
    auto action = new QAction();
    action->setSeparator(selectable);

    auto item = createActionItem(action);
    item->setSizeHint(widget->size());

    m_view->addItem(item);
    m_view->setItemWidget(item, widget);
    m_view->adjustSize();

    if (!selectable) {
        item->setFlags(Qt::NoItemFlags);
    }

    adjustMenuSize();
    adjustSize();
}
