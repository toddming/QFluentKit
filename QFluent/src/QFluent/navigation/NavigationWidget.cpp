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

#include "Theme.h"
#include "FluentIcon.h"
#include "QFluent/AvatarWidget.h"

NavigationWidget::NavigationWidget(bool isSelectable, QWidget* parent)
    : QWidget(parent)
    , lightTextColor(0, 0, 0)
    , darkTextColor(255, 255, 255)
    , m_treeParent(nullptr)
{
    setFixedSize(40, 36);
    m_expandWidth = 160;

    setProperty("nodeDepth", 0);
    setProperty("isCompacted", true);
    setProperty("isSelected", false);
    setProperty("isPressed", false);
    setProperty("isEnter", false);
    setProperty("isSelectable", isSelectable);
}

void NavigationWidget::insertChild(int index, NavigationWidget* child)
{

}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void NavigationWidget::enterEvent(QEnterEvent* e) {
    setProperty("isEnter", true);
    update();
}
#else
void NavigationWidget::enterEvent(QEvent* e) {
    setProperty("isEnter", true);
    update();
}
#endif

void NavigationWidget::leaveEvent(QEvent* e) {
    setProperty("isEnter", false);
    setProperty("isPressed", false);

    update();
}

void NavigationWidget::mousePressEvent(QMouseEvent* e) {
    QWidget::mousePressEvent(e);
    setProperty("isPressed", true);
    update();
}

void NavigationWidget::mouseReleaseEvent(QMouseEvent* e) {
    QWidget::mouseReleaseEvent(e);
    setProperty("isPressed", false);
    update();
    emit clicked(true);
}

void NavigationWidget::setTreeParent(NavigationWidget* p)
{
    m_treeParent = p;
}

int NavigationWidget::expandWidth()
{
    return m_expandWidth;
}

NavigationWidget* NavigationWidget::treeParent()
{
    return m_treeParent;
}

void NavigationWidget::click() {
    emit clicked(true);
}

void NavigationWidget::setCompacted(bool isCompacted) {
    if (isCompacted == property("isCompacted").toBool())
        return;

    setProperty("isCompacted", isCompacted);
    if (isCompacted) {
        setFixedSize(40, 36);
    } else {
        setFixedSize(m_expandWidth, 36);
    }
    update();
}

void NavigationWidget::setSelected(bool isSelected) {
    if (!property("isSelectable").toBool())
        return;

    setProperty("isSelected", isSelected);
    update();
    emit selectedChanged(isSelected);
}

QColor NavigationWidget::textColor() {
    return Theme::instance()->isDarkTheme() ? darkTextColor : lightTextColor;
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
    // setFixedSize(m_expandWidth, 36);
}

// NavigationPushButton 实现
NavigationPushButton::NavigationPushButton(const QString &text, const FluentIconBase &icon,
                                           bool isSelectable, QWidget* parent)
    : NavigationWidget(isSelectable, parent), m_fluentIcon(icon.clone()), m_text(text) {

}

QString NavigationPushButton::text() const {
    return m_text;
}

void NavigationPushButton::setText(const QString& text) {
    m_text = text;
    update();
}

void NavigationPushButton::setFluentIcon(const FluentIconBase &icon)
{
    m_fluentIcon.reset(icon.clone());
}

FluentIconBase* NavigationPushButton::fluentIcon() const
{
    return m_fluentIcon.get();
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
    return property("isSelected").toBool();
}


void NavigationPushButton::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);

    if (property("isPressed").toBool())
        painter.setOpacity(0.7);
    if (!isEnabled())
        painter.setOpacity(0.4);

    // 绘制背景
    int c = Theme::instance()->isDarkTheme() ? 255 : 0;
    QMargins m = _margins();
    int pl = m.left(), pr = m.right();
    QPoint globalPos = mapToGlobal(QPoint(0, 0));
    QRect globalRect(globalPos, size());

    if (_canDrawIndicator()) {
        painter.setBrush(QColor(c, c, c, property("isEnter").toBool() ? 6 : 10));
        painter.drawRoundedRect(rect(), 5, 5);

        // 绘制指示器
        painter.setBrush(Theme::instance()->themeColor());
        painter.drawRoundedRect(pl, 10, 3, 16, 1.5, 1.5);
    } else if (property("isEnter").toBool() && isEnabled() && globalRect.contains(QCursor::pos())) {
        painter.setBrush(QColor(c, c, c, 10));
        painter.drawRoundedRect(rect(), 5, 5);
    }

    FluentIconUtils::drawIcon(*m_fluentIcon, &painter, QRectF(11.5 + pl, 10, 16, 16));

    // 绘制文本
    if (property("isCompacted").toBool())
        return;

    painter.setPen(textColor());

    int left = (m_fluentIcon == nullptr) ? pl + 16 : 44 + pl;
    painter.drawText(QRectF(left, 0, width() - 13 - left - pr, height()), Qt::AlignVCenter, text());
}

// NavigationToolButton 实现
NavigationToolButton::NavigationToolButton(const FluentIconBase &icon, QWidget* parent)
    : NavigationPushButton("", icon, false, parent) {
    setFixedSize(40, 36);

    setProperty("isCompacted", false);
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
        setFixedSize(expandWidth() + 10, 3);
    }
    update();
}

void NavigationSeparator::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);
    QPainter painter(this);
    int c = Theme::instance()->isDarkTheme() ? 255 : 0;
    QPen pen(QColor(c, c, c, 15));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawLine(0, 1, width(), 1);
}

// NavigationTreeItem 实现
NavigationTreeItem::NavigationTreeItem(const QString &text, const FluentIconBase &icon,
                                       bool isSelectable, NavigationTreeWidget* parent)
    : NavigationPushButton(text, icon, isSelectable, parent), _arrowAngle(0) {
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
    bool clickArrow = QRectF(width() - 30, 8, 20, 20).contains(e->localPos());
    NavigationTreeWidget* p = qobject_cast<NavigationTreeWidget*>(parent());
    if (p) {
        emit itemClicked(true, clickArrow && !p->isLeaf());
    }
    update();
}

QMargins NavigationTreeItem::_margins() {
    NavigationTreeWidget* p = qobject_cast<NavigationTreeWidget*>(parent());
    if (p) {
        int right = p->treeChildren().empty() ? 0 : 20;
        return QMargins(p->property("nodeDepth").toInt() * 28, 0, right, 0);
    }
    return QMargins(0, 0, 0, 0);
}

bool NavigationTreeItem::_canDrawIndicator() {
    NavigationTreeWidget* p = qobject_cast<NavigationTreeWidget*>(parent());
    if (!p) return false;

    if (p->isLeaf() || p->property("isSelected").toBool())
        return p->property("isSelected").toBool();

    for (auto child : p->treeChildren()) {
        if (child->itemWidget()->_canDrawIndicator() && !child->isVisible())
            return true;
    }

    return false;
}

void NavigationTreeItem::paintEvent(QPaintEvent* e) {
    NavigationPushButton::paintEvent(e);

    NavigationTreeWidget* p = qobject_cast<NavigationTreeWidget*>(parent());
    if (!p || property("isCompacted").toBool() || p->treeChildren().empty())
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    if (property("isPressed").toBool())
        painter.setOpacity(0.7);
    if (!isEnabled())
        painter.setOpacity(0.4);

    painter.translate(width() - 20, 18);
    painter.rotate(_arrowAngle);

    FluentIcon(Fluent::IconType::ARROW_DOWN).render(&painter, QRectF(-5, -5, 9.6, 9.6));
}


float NavigationTreeItem::getArrowAngle() const {
    return _arrowAngle;
}

void NavigationTreeItem::setArrowAngle(float angle) {
    _arrowAngle = angle;
    update();
}

// NavigationTreeWidget 实现
NavigationTreeWidget::NavigationTreeWidget(const QString &text, const FluentIconBase &icon,
                                           bool isSelectable, QWidget* parent)
    : NavigationTreeWidgetBase(isSelectable, parent), isExpanded(false), m_fluentIcon(icon.clone()) {
    m_itemWidget = new NavigationTreeItem(text, icon, isSelectable, this);
    vBoxLayout = new QVBoxLayout(this);
    expandAni = new QPropertyAnimation(this, "geometry", this);

    __initWidget();
}

void NavigationTreeWidget::__initWidget() {
    vBoxLayout->setSpacing(4);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addWidget(m_itemWidget, 0, Qt::AlignTop);

    connect(m_itemWidget, &NavigationTreeItem::itemClicked, this, &NavigationTreeWidget::_onClicked);
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
    return m_itemWidget->text();
}

void NavigationTreeWidget::setText(const QString& text) {
    m_itemWidget->setText(text);
}

void NavigationTreeWidget::setFluentIcon(const FluentIconBase &icon)
{
    m_fluentIcon.reset(icon.clone());
    m_itemWidget->setFluentIcon(*m_fluentIcon);
}

FluentIconBase* NavigationTreeWidget::fluentIcon() const
{
    return m_fluentIcon.get();
}

void NavigationTreeWidget::setIndicatorColor(const QColor& light, const QColor& dark) {
    m_itemWidget->setIndicatorColor(light, dark);
}

void NavigationTreeWidget::setFont(const QFont& font) {
    QWidget::setFont(font);
    m_itemWidget->setFont(font);
}

NavigationTreeWidget* NavigationTreeWidget::clone() {
    NavigationTreeWidget* root = new NavigationTreeWidget(text(), *m_fluentIcon, property("isSelectable").toBool(), parentWidget());
    root->setSelected(property("isSelected").toBool());
    root->setFixedSize(size());
    root->setProperty("nodeDepth", property("nodeDepth").toInt());

    connect(root, &NavigationTreeWidget::clicked, this, &NavigationTreeWidget::clicked);
    connect(this, &NavigationTreeWidget::selectedChanged, root, &NavigationTreeWidget::setSelected);

    for (auto child : m_treeChildren) {
        root->addChild(child->clone());
    }

    return root;
}

int NavigationTreeWidget::suitableWidth() {
    QMargins m = m_itemWidget->_margins();
    int left = (false) ? m.left() + 29 : 57 + m.left();
    int tw = m_itemWidget->fontMetrics().boundingRect(text()).width();
    return left + tw + m.right();
}

void NavigationTreeWidget::insertChild(int index, NavigationWidget* child) {
    NavigationTreeWidget* treeChild = dynamic_cast<NavigationTreeWidget*>(child);
    if (!treeChild || std::find(m_treeChildren.begin(), m_treeChildren.end(), treeChild) != m_treeChildren.end())
        return;

    treeChild->setTreeParent(this);
    treeChild->setProperty("nodeDepth", property("nodeDepth").toInt() + 1);
    treeChild->setVisible(isExpanded);
    connect(treeChild->expandAni, &QPropertyAnimation::valueChanged, [this]() {
        setFixedSize(sizeHint());
    });
    connect(treeChild->expandAni, &QPropertyAnimation::valueChanged, this, &NavigationTreeWidget::expanded);

    // 递归连接高度变化信号到父级
    NavigationTreeWidget* p = dynamic_cast<NavigationTreeWidget*>(treeParent());
    while (p) {
        connect(treeChild->expandAni, &QPropertyAnimation::valueChanged, [p]() {
            p->setFixedSize(p->sizeHint());
        });
        p = dynamic_cast<NavigationTreeWidget*>(p->treeParent());
    }

    if (index < 0) {
        index = static_cast<int>(m_treeChildren.size());
    }

    m_treeChildren.insert(m_treeChildren.begin() + index, treeChild);

    // 修正：在插入布局时要加1，因为itemWidget是布局中的第0个元素
    vBoxLayout->insertWidget(index + 1, treeChild, 0, Qt::AlignTop);

    // 调整高度
    if (isExpanded) {
        setFixedHeight(height() + treeChild->height() + vBoxLayout->spacing());

        NavigationTreeWidget* p = dynamic_cast<NavigationTreeWidget*>(treeParent());
        while (p) {
            p->setFixedSize(p->sizeHint());
            p = dynamic_cast<NavigationTreeWidget*>(p->treeParent());
        }
    }

    update();
}

void NavigationTreeWidget::removeChild(NavigationWidget* child) {
    NavigationTreeWidget* treeChild = dynamic_cast<NavigationTreeWidget*>(child);
    if (!treeChild) return;

    auto it = std::find(m_treeChildren.begin(), m_treeChildren.end(), treeChild);
    if (it != m_treeChildren.end()) {
        m_treeChildren.erase(it);
        vBoxLayout->removeWidget(treeChild);
    }
}

std::vector<NavigationWidget*> NavigationTreeWidget::childItems() {
    std::vector<NavigationWidget*> result;
    for (auto child : m_treeChildren) {
        result.push_back(child);
    }
    return result;
}

std::vector<NavigationTreeWidget*> NavigationTreeWidget::treeChildren()
{
    return m_treeChildren;
}

NavigationTreeItem* NavigationTreeWidget::itemWidget()
{
    return m_itemWidget;
}

void NavigationTreeWidget::setExpanded(bool isExpanded) {
    setExpanded(isExpanded, false);
}

void NavigationTreeWidget::setExpanded(bool isExpanded, bool ani) {
    if (isExpanded == this->isExpanded)
        return;

    this->isExpanded = isExpanded;
    m_itemWidget->setExpanded(isExpanded);

    for (auto child : m_treeChildren) {
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
    return treeParent() == nullptr;
}

bool NavigationTreeWidget::isLeaf() {
    return m_treeChildren.empty();
}

void NavigationTreeWidget::setSelected(bool isSelected) {
    NavigationWidget::setSelected(isSelected);
    m_itemWidget->setSelected(isSelected);
}

void NavigationTreeWidget::mouseReleaseEvent(QMouseEvent* e) {
    // 空实现，不处理鼠标释放事件
}

void NavigationTreeWidget::setCompacted(bool isCompacted) {
    NavigationWidget::setCompacted(isCompacted);
    m_itemWidget->setCompacted(isCompacted);
    update();
}

void NavigationTreeWidget::_onClicked(bool triggerByUser, bool clickArrow) {
    if (!property("isCompacted").toBool()) {
        if (property("isSelectable").toBool() && !property("isSelected").toBool() && !clickArrow) {
            setExpanded(true, true);
        } else {
            setExpanded(!isExpanded, true);
        }
    }

    if (!clickArrow || property("isCompacted").toBool()) {
        emit clicked(triggerByUser);
    }
}

void NavigationTreeWidget::setExpandWidth(int width) {
    NavigationTreeWidgetBase::setExpandWidth(width);
    m_itemWidget->setExpandWidth(width);
}




// NavigationFlyoutMenu 实现
NavigationFlyoutMenu::NavigationFlyoutMenu(NavigationTreeWidget* tree, QWidget* parent)
    : ScrollArea(parent)
    , treeWidget(tree)
{
    setViewportMargins(0, 0, 0, 0);

    view = new QWidget(this);
    vBoxLayout = new QVBoxLayout(view);

    setWidget(view);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("ScrollArea { border: none; background: transparent; }");
    view->setStyleSheet("QWidget { border: none; background: transparent; }");

    vBoxLayout->setSpacing(5);
    vBoxLayout->setContentsMargins(5, 8, 5, 8);

    // 添加节点到菜单
    for (auto child : treeWidget->treeChildren()) {
        NavigationTreeWidget* node = child->clone();
        connect(node, &NavigationTreeWidget::expanded, this, [this]() { _adjustViewSize(); });

        treeChildren.push_back(node);
        vBoxLayout->addWidget(node);
    }

    for (auto node : treeChildren) {
        node->setProperty("nodeDepth", node->property("nodeDepth").toInt() - 1);
        node->setCompacted(false);
        if (node->isLeaf()) {
            connect(node, &NavigationTreeWidget::clicked, window(), &QWidget::close);
        }
        _initNode(node);
    }

    _adjustViewSize(false);
}

void NavigationFlyoutMenu::_initNode(NavigationTreeWidget* root) {
    for (auto c : root->treeChildren()) {
        c->setProperty("nodeDepth", c->property("nodeDepth").toInt() - 1);
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
        node->itemWidget()->setFixedWidth(w - 10);
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

        for (auto child : node->treeChildren()) {
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

QString NavigationAvatarWidget::name() const
{
    return m_name;
}

void NavigationAvatarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    if (property("isPressed").toBool()) {
        painter.setOpacity(0.7);
    }

    // draw background on hover
    if (property("isEnter").toBool()) { // 假设 NavigationWidget 提供 isEnter()
        int c = Theme::instance()->isDarkTheme() ? 255 : 0;
        painter.setBrush(QColor(c, c, c, 10));
        painter.drawRoundedRect(rect(), 5, 5);
    }

    // draw text if not compacted
    if (!property("isCompacted").toBool()) { // 假设 NavigationWidget 提供 isCompacted()
        painter.setPen(textColor()); // 假设提供 textColor()
        painter.setFont(font());
        painter.drawText(QRect(44, 0, 255, 36), Qt::AlignVCenter, m_name);
    }
}


