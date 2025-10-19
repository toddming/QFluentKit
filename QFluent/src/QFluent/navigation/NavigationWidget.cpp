#include "NavigationWidget.h"
#include <QMouseEvent>
#include <QEnterEvent>
#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QPainter>
#include <QVBoxLayout>
#include <QEvent>
#include <QColor>
#include <QMargins>
#include <QFont>
#include <QPoint>
#include <QPixmap>
#include <QImage>
#include <vector>
#include <deque>

#include "Icon.h"
#include "Theme.h"
#include "AvatarWidget.h"

NavigationWidget::NavigationWidget(bool isSelectable, QWidget* parent)
    : QWidget(parent), isCompacted(true), isSelected(false), isPressed(false),
      isEnter(false), isSelectable(isSelectable), treeParent(nullptr), nodeDepth(0),
      lightTextColor(0, 0, 0), darkTextColor(255, 255, 255) {
    setFixedSize(40, 36);
}

void NavigationWidget::insertChild(int index, NavigationWidget* child)
{

}

void NavigationWidget::enterEvent(QEnterEvent* e) {
    isEnter = true;
    update();
}

void NavigationWidget::leaveEvent(QEvent* e) {
    isEnter = false;
    isPressed = false;
    update();
}

void NavigationWidget::mousePressEvent(QMouseEvent* e) {
    QWidget::mousePressEvent(e);
    isPressed = true;
    update();
}

void NavigationWidget::mouseReleaseEvent(QMouseEvent* e) {
    QWidget::mouseReleaseEvent(e);
    isPressed = false;
    update();
    emit clicked(true);
}

void NavigationWidget::click() {
    emit clicked(true);
}

void NavigationWidget::setCompacted(bool isCompacted) {
    if (isCompacted == this->isCompacted)
        return;

    this->isCompacted = isCompacted;
    if (isCompacted) {
        setFixedSize(40, 36);
    } else {
        setFixedSize(m_expandWidth, 36);
    }

    update();
}

void NavigationWidget::setSelected(bool isSelected) {
    if (!isSelectable)
        return;

    this->isSelected = isSelected;
    update();
    emit selectedChanged(isSelected);
}

QColor NavigationWidget::textColor() {
    return Theme::instance()->isDarkMode() ? darkTextColor : lightTextColor;
}

void NavigationWidget::setLightTextColor(const QColor& color) {
    lightTextColor = color;
    update();
}

void NavigationWidget::setDarkTextColor(const QColor& color) {
    darkTextColor = color;
    update();
}

void NavigationWidget::setTextColor(const QColor& light, const QColor& dark) {
    setLightTextColor(light);
    setDarkTextColor(dark);
}

void NavigationWidget::setExpandWidth(int width) {
    if (width <= 42) {
        return;
    }

    m_expandWidth = width;
}

// NavigationPushButton 实现
NavigationPushButton::NavigationPushButton(IconType::FLuentIcon icon, const QString& text,
                                         bool isSelectable, QWidget* parent)
    : NavigationWidget(isSelectable, parent), m_icon(icon), m_text(text) {
    // setFont(this);
}

QString NavigationPushButton::text() const {
    return m_text;
}

void NavigationPushButton::setText(const QString& text) {
    m_text = text;
    update();
}

QIcon NavigationPushButton::icon() const {
    return QIcon();
}

void NavigationPushButton::setIcon(IconType::FLuentIcon icon) {
    m_icon = icon;
    update();
}

void NavigationPushButton::setIndicatorColor(const QColor& light, const QColor& dark) {
    lightIndicatorColor = light;
    darkIndicatorColor = dark;
    update();
}

QMargins NavigationPushButton::_margins() {
    return QMargins(0, 0, 0, 0);
}

bool NavigationPushButton::_canDrawIndicator() {
    return isSelected;
}

void NavigationPushButton::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);

    if (isPressed)
        painter.setOpacity(0.7);
    if (!isEnabled())
        painter.setOpacity(0.4);

    // 绘制背景
    int c = Theme::instance()->isDarkMode() ? 255 : 0;
    QMargins m = _margins();
    int pl = m.left(), pr = m.right();
    QPoint globalPos = mapToGlobal(QPoint(0, 0));
    QRect globalRect(globalPos, size());

    if (_canDrawIndicator()) {
        painter.setBrush(QColor(c, c, c, isEnter ? 6 : 10));
        painter.drawRoundedRect(rect(), 5, 5);

        // 绘制指示器
        painter.setBrush(Theme::instance()->themeColor());
        painter.drawRoundedRect(pl, 10, 3, 16, 1.5, 1.5);
    } else if (isEnter && isEnabled() && globalRect.contains(QCursor::pos())) {
        painter.setBrush(QColor(c, c, c, 10));
        painter.drawRoundedRect(rect(), 5, 5);
    }

    Icon::drawSvgIcon(&painter, m_icon, QRectF(11.5 + pl, 10, 16, 16));

    // 绘制文本
    if (isCompacted)
        return;

    painter.setPen(textColor());

    int left = (m_icon == IconType::FLuentIcon::NONE) ? pl + 16 : 44 + pl;
    painter.drawText(QRectF(left, 0, width() - 13 - left - pr, height()), Qt::AlignVCenter, text());
}

// NavigationToolButton 实现
NavigationToolButton::NavigationToolButton(IconType::FLuentIcon icon, QWidget* parent)
    : NavigationPushButton(icon, "", false, parent) {
    setFixedSize(40, 36);
    isCompacted = false;
}

void NavigationToolButton::setCompacted(bool isCompacted) {
    setFixedSize(40, 36);
}

// NavigationSeparator 实现
NavigationSeparator::NavigationSeparator(QWidget* parent)
    : NavigationWidget(false, parent) {
    setCompacted(true);
}

void NavigationSeparator::setCompacted(bool isCompacted) {
    if (isCompacted) {
        setFixedSize(48, 3);
    } else {
        setFixedSize(m_expandWidth + 10, 3);
    }
    update();
}

void NavigationSeparator::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    int c = 1 ? 255 : 0;
    QPen pen(QColor(c, c, c, 15));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawLine(0, 1, width(), 1);
}

// NavigationTreeItem 实现
NavigationTreeItem::NavigationTreeItem(IconType::FLuentIcon icon, const QString& text,
                                     bool isSelectable, NavigationTreeWidget* parent)
    : NavigationPushButton(icon, text, isSelectable, parent), _arrowAngle(0) {
    rotateAni = new QPropertyAnimation(this, "arrowAngle", this);
}

void NavigationTreeItem::setExpanded(bool isExpanded) {
    rotateAni->stop();
    rotateAni->setEndValue(isExpanded ? 180.0f : 0.0f);
    rotateAni->setDuration(150);
    rotateAni->start();
}

void NavigationTreeItem::mouseReleaseEvent(QMouseEvent* e) {
    NavigationPushButton::mouseReleaseEvent(e);
    bool clickArrow = QRectF(width() - 30, 8, 20, 20).contains(e->position());
    NavigationTreeWidget* p = qobject_cast<NavigationTreeWidget*>(parent());
    if (p) {
        emit itemClicked(true, clickArrow && !p->isLeaf());
    }
    update();
}

QMargins NavigationTreeItem::_margins() {
    NavigationTreeWidget* p = qobject_cast<NavigationTreeWidget*>(parent());
    if (p) {
        int right = p->treeChildren.empty() ? 0 : 20;
        return QMargins(p->nodeDepth * 28, 0, right, 0);
    }
    return QMargins(0, 0, 0, 0);
}

bool NavigationTreeItem::_canDrawIndicator() {
    NavigationTreeWidget* p = qobject_cast<NavigationTreeWidget*>(parent());
    if (!p) return false;

    if (p->isLeaf() || p->isSelected)
        return p->isSelected;

    for (auto child : p->treeChildren) {
        if (child->itemWidget->_canDrawIndicator() && !child->isVisible())
            return true;
    }

    return false;
}

void NavigationTreeItem::paintEvent(QPaintEvent* e) {
    NavigationPushButton::paintEvent(e);

    NavigationTreeWidget* p = qobject_cast<NavigationTreeWidget*>(parent());
    if (!p || isCompacted || p->treeChildren.empty())
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    if (isPressed)
        painter.setOpacity(0.7);
    if (!isEnabled())
        painter.setOpacity(0.4);

    painter.translate(width() - 20, 18);
    painter.rotate(_arrowAngle);

    Icon::drawSvgIcon(&painter, IconType::FLuentIcon::ARROW_DOWN, QRectF(-5, -5, 9.6, 9.6));
}


float NavigationTreeItem::getArrowAngle() const {
    return _arrowAngle;
}

void NavigationTreeItem::setArrowAngle(float angle) {
    _arrowAngle = angle;
    update();
}

// NavigationTreeWidget 实现
NavigationTreeWidget::NavigationTreeWidget(IconType::FLuentIcon icon, const QString& text,
                                         bool isSelectable, QWidget* parent)
    : NavigationTreeWidgetBase(isSelectable, parent), isExpanded(false), m_icon(icon) {
    itemWidget = new NavigationTreeItem(icon, text, isSelectable, this);
    vBoxLayout = new QVBoxLayout(this);
    expandAni = new QPropertyAnimation(this, "geometry", this);

    __initWidget();
}

void NavigationTreeWidget::__initWidget() {
    vBoxLayout->setSpacing(4);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addWidget(itemWidget, 0, Qt::AlignTop);

    connect(itemWidget, &NavigationTreeItem::itemClicked, this, &NavigationTreeWidget::_onClicked);
    setAttribute(Qt::WA_TranslucentBackground);
    connect(expandAni, &QPropertyAnimation::valueChanged, [this](const QVariant& value) {
        setFixedSize(value.toRect().size());
    });
    connect(expandAni, &QPropertyAnimation::valueChanged, this, &NavigationTreeWidget::expanded);
    connect(expandAni, &QPropertyAnimation::finished, [this]() {
        if (parentWidget() && parentWidget()->layout()) {
            parentWidget()->layout()->invalidate();
        }
    });
}

void NavigationTreeWidget::addChild(NavigationWidget* child) {
    insertChild(-1, child);
}

QString NavigationTreeWidget::text() const {
    return itemWidget->text();
}

QIcon NavigationTreeWidget::icon() const {
    return itemWidget->icon();
}

void NavigationTreeWidget::setText(const QString& text) {
    itemWidget->setText(text);
}

void NavigationTreeWidget::setIcon(IconType::FLuentIcon icon) {
    itemWidget->setIcon(icon);
}

void NavigationTreeWidget::setIndicatorColor(const QColor& light, const QColor& dark) {
    itemWidget->setIndicatorColor(light, dark);
}

void NavigationTreeWidget::setFont(const QFont& font) {
    QWidget::setFont(font);
    itemWidget->setFont(font);
}

NavigationTreeWidget* NavigationTreeWidget::clone() {
    NavigationTreeWidget* root = new NavigationTreeWidget(m_icon, text(), isSelectable, parentWidget());
    root->setSelected(isSelected);
    root->setFixedSize(size());
    root->setTextColor(lightTextColor, darkTextColor);
    // root->setIndicatorColor(itemWidget->lightIndicatorColor, itemWidget->darkIndicatorColor);
    root->nodeDepth = nodeDepth;

    connect(this, &NavigationTreeWidget::clicked, root, &NavigationTreeWidget::clicked);
    connect(this, &NavigationTreeWidget::selectedChanged, root, &NavigationTreeWidget::setSelected);

    for (auto child : treeChildren) {
        root->addChild(child->clone());
    }

    return root;
}

int NavigationTreeWidget::suitableWidth() {
    QMargins m = itemWidget->_margins();
    int left = icon().isNull() ? m.left() + 29 : 57 + m.left();
    int tw = itemWidget->fontMetrics().boundingRect(text()).width();
    return left + tw + m.right();
}

void NavigationTreeWidget::insertChild(int index, NavigationWidget* child) {
    NavigationTreeWidget* treeChild = dynamic_cast<NavigationTreeWidget*>(child);
    if (!treeChild || std::find(treeChildren.begin(), treeChildren.end(), treeChild) != treeChildren.end())
        return;

    treeChild->treeParent = this;
    treeChild->nodeDepth = nodeDepth + 1;
    treeChild->setVisible(isExpanded);
    connect(treeChild->expandAni, &QPropertyAnimation::valueChanged, [this]() {
        setFixedSize(sizeHint());
    });
    connect(treeChild->expandAni, &QPropertyAnimation::valueChanged, this, &NavigationTreeWidget::expanded);

    // 递归连接高度变化信号到父级
    NavigationTreeWidget* p = dynamic_cast<NavigationTreeWidget*>(treeParent);
    while (p) {
        connect(treeChild->expandAni, &QPropertyAnimation::valueChanged, [p]() {
            p->setFixedSize(p->sizeHint());
        });
        p = dynamic_cast<NavigationTreeWidget*>(p->treeParent);
    }

    if (index < 0) {
        index = static_cast<int>(treeChildren.size());
    }

    // 修正：在插入 treeChildren 时不要加1
    treeChildren.insert(treeChildren.begin() + index, treeChild);

    // 修正：在插入布局时要加1，因为itemWidget是布局中的第0个元素
    vBoxLayout->insertWidget(index + 1, treeChild, 0, Qt::AlignTop);

    // 调整高度
    if (isExpanded) {
        setFixedHeight(height() + treeChild->height() + vBoxLayout->spacing());

        NavigationTreeWidget* p = dynamic_cast<NavigationTreeWidget*>(treeParent);
        while (p) {
            p->setFixedSize(p->sizeHint());
            p = dynamic_cast<NavigationTreeWidget*>(p->treeParent);
        }
    }

    update();
}

void NavigationTreeWidget::removeChild(NavigationWidget* child) {
    NavigationTreeWidget* treeChild = dynamic_cast<NavigationTreeWidget*>(child);
    if (!treeChild) return;

    auto it = std::find(treeChildren.begin(), treeChildren.end(), treeChild);
    if (it != treeChildren.end()) {
        treeChildren.erase(it);
        vBoxLayout->removeWidget(treeChild);
    }
}

std::vector<NavigationWidget*> NavigationTreeWidget::childItems() {
    std::vector<NavigationWidget*> result;
    for (auto child : treeChildren) {
        result.push_back(child);
    }
    return result;
}

void NavigationTreeWidget::setExpanded(bool isExpanded) {
    setExpanded(isExpanded, false);
}

void NavigationTreeWidget::setExpanded(bool isExpanded, bool ani) {
    if (isExpanded == this->isExpanded)
        return;

    this->isExpanded = isExpanded;
    itemWidget->setExpanded(isExpanded);

    for (auto child : treeChildren) {
        child->setVisible(isExpanded);
        child->setFixedSize(child->sizeHint());
    }

    if (ani) {
        expandAni->stop();
        expandAni->setStartValue(geometry());
        expandAni->setEndValue(QRect(pos(), sizeHint()));
        expandAni->setDuration(120);
        expandAni->setEasingCurve(QEasingCurve::OutQuad);
        expandAni->start();
    } else {
        setFixedSize(sizeHint());
    }
}

bool NavigationTreeWidget::isRoot() {
    return treeParent == nullptr;
}

bool NavigationTreeWidget::isLeaf() {
    return treeChildren.empty();
}

void NavigationTreeWidget::setSelected(bool isSelected) {
    NavigationWidget::setSelected(isSelected);
    itemWidget->setSelected(isSelected);
}

void NavigationTreeWidget::mouseReleaseEvent(QMouseEvent* e) {
    // 空实现，不处理鼠标释放事件
}

void NavigationTreeWidget::setCompacted(bool isCompacted) {
    NavigationWidget::setCompacted(isCompacted);
    itemWidget->setCompacted(isCompacted);
    update();
}

void NavigationTreeWidget::_onClicked(bool triggerByUser, bool clickArrow) {
    if (!isCompacted) {
        if (isSelectable && !isSelected && !clickArrow) {
            setExpanded(true, true);
        } else {
            setExpanded(!isExpanded, true);
        }
    }

    if (!clickArrow || isCompacted) {
        emit clicked(triggerByUser);
    }
}





// NavigationFlyoutMenu 实现
NavigationFlyoutMenu::NavigationFlyoutMenu(NavigationTreeWidget* tree, QWidget* parent)
    : QScrollArea(parent), treeWidget(tree) {
    view = new QWidget(this);
    vBoxLayout = new QVBoxLayout(view);

    setWidget(view);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("QScrollArea { border: none; background: transparent; }");
    view->setStyleSheet("QWidget { border: none; background: transparent; }");

    vBoxLayout->setSpacing(5);
    vBoxLayout->setContentsMargins(5, 8, 5, 8);

    // 添加节点到菜单
    for (auto child : treeWidget->treeChildren) {
        NavigationTreeWidget* node = child->clone();
        connect(node, &NavigationTreeWidget::expanded, this, [this]() { _adjustViewSize(); });

        treeChildren.push_back(node);
        vBoxLayout->addWidget(node);
    }

    _initNode(this->treeWidget);
    _adjustViewSize(false);
}

void NavigationFlyoutMenu::_initNode(NavigationTreeWidget* root) {
    for (auto c : root->treeChildren) {
        c->nodeDepth -= 1;
        c->setCompacted(false);

        if (c->isLeaf()) {
            connect(c, &NavigationTreeWidget::clicked, window(), &QWidget::close);
        }

        _initNode(c);
    }
}

void NavigationFlyoutMenu::_adjustViewSize(bool emitSignal) {
    int w = _suitableWidth();

    // 调整节点宽度
    for (auto node : visibleTreeNodes()) {
        node->setFixedWidth(w - 10);
        node->itemWidget->setFixedWidth(w - 10);
    }

    view->setFixedSize(w, view->sizeHint().height());

    int h = qMin(window()->parentWidget()->height() - 48, view->height());
    setFixedSize(w, h);

    if (emitSignal)
        emit expanded();
}

int NavigationFlyoutMenu::_suitableWidth() {
    int w = 0;

    for (auto node : visibleTreeNodes()) {
        if (!node->isHidden()) {
            w = qMax(w, node->suitableWidth() + 10);
        }
    }

    QWidget* window = this->window()->parentWidget();
    return qMin(window->width() / 2 - 25, w) + 10;
}

std::vector<NavigationTreeWidget*> NavigationFlyoutMenu::visibleTreeNodes() {
    std::vector<NavigationTreeWidget*> nodes;
    std::deque<NavigationTreeWidget*> queue(treeChildren.begin(), treeChildren.end());

    while (!queue.empty()) {
        NavigationTreeWidget* node = queue.front();
        queue.pop_front();
        nodes.push_back(node);

        for (auto child : node->treeChildren) {
            if (!child->isHidden()) {
                queue.push_back(child);
            }
        }
    }

    return nodes;
}









NavigationAvatarWidget::NavigationAvatarWidget(const QString &name,
                                               const QVariant &avatar,
                                               QWidget *parent)
    : NavigationWidget(false, parent)
    , m_name(name)
{
    m_avatar = new AvatarWidget(this);
    m_avatar->setRadius(12);
    m_avatar->setText(name);
    m_avatar->move(8, 6);

    if (avatar.isValid()) {
        setAvatar(avatar);
    }
}

void NavigationAvatarWidget::setName(const QString &name)
{
    m_name = name;
    m_avatar->setText(name);
    update();
}

void NavigationAvatarWidget::setAvatar(const QVariant &avatar)
{
    if (avatar.canConvert<QPixmap>()) {
        m_avatar->setImage(avatar.value<QPixmap>());
    } else if (avatar.canConvert<QImage>()) {
        m_avatar->setImage(avatar.value<QImage>());
    } else if (avatar.canConvert<QString>()) {
        QString path = avatar.toString();
        QPixmap pixmap(path);
        if (!pixmap.isNull()) {
            m_avatar->setImage(pixmap);
        }
    }
    m_avatar->setRadius(12);
    update();
}

void NavigationAvatarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    if (isPressed) { // 假设 NavigationWidget 提供 isPressed()
        painter.setOpacity(0.7);
    }

    // draw background on hover
    if (isEnter) { // 假设 NavigationWidget 提供 isEnter()
        int c = Theme::instance()->isDarkMode() ? 255 : 0;
        painter.setBrush(QColor(c, c, c, 10));
        painter.drawRoundedRect(rect(), 5, 5);
    }

    // draw text if not compacted
    if (!isCompacted) { // 假设 NavigationWidget 提供 isCompacted()
        painter.setPen(textColor()); // 假设提供 textColor()
        painter.setFont(font());
        painter.drawText(QRect(44, 0, 255, 36), Qt::AlignVCenter, m_name);
    }
}


