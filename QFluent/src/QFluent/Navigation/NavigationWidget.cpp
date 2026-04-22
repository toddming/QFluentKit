#include "NavigationWidget.h"
#include <QMouseEvent>
#include <QEnterEvent>
#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
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
#include <algorithm>
#include <QVariantAnimation>

#include "Theme.h"
#include "Animation.h"
#include "FluentIcon.h"
#include "QFluent/ImageLabel.h"

// ============================================================================
// NavigationWidget 实现
// ============================================================================

NavigationWidget::NavigationWidget(bool isSelectable, QWidget* parent)
    : QWidget(parent)
    , m_lightTextColor(0, 0, 0)
    , m_darkTextColor(255, 255, 255)
{
    setFixedSize(40, 36);

    setProperty("nodeDepth", 0);
    setProperty("isCompacted", true);
    setProperty("isSelected", false);
    setProperty("isPressed", false);
    setProperty("isEnter", false);
    setProperty("isSelectable", isSelectable);
}

void NavigationWidget::insertChild(int /*index*/, NavigationWidget* /*child*/)
{
    // 基类默认不实现
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void NavigationWidget::enterEvent(QEnterEvent* e) {
    Q_UNUSED(e);
    setProperty("isEnter", true);
    update();
}
#else
void NavigationWidget::enterEvent(QEvent* e) {
    Q_UNUSED(e);
    setProperty("isEnter", true);
    update();
}
#endif

void NavigationWidget::leaveEvent(QEvent* e) {
    Q_UNUSED(e);
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

int NavigationWidget::expandWidth() const
{
    return m_expandWidth;
}

NavigationWidget* NavigationWidget::treeParent() const
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
    setFixedSize(isCompacted ? 40 : m_expandWidth, 36);
    update();
}

void NavigationWidget::setSelected(bool isSelected) {
    if (!property("isSelectable").toBool())
        return;

    setProperty("isSelected", isSelected);
    update();
    emit selectedChanged(isSelected);
}

QColor NavigationWidget::textColor() const {
    return Theme::isDark() ? m_darkTextColor : m_lightTextColor;
}

void NavigationWidget::setLightTextColor(const QColor& color) {
    m_lightTextColor = color;
    update();
}

void NavigationWidget::setDarkTextColor(const QColor& color) {
    m_darkTextColor = color;
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

// ============================================================================
// NavigationPushButton 实现
// ============================================================================

NavigationPushButton::NavigationPushButton(const QString &text, const FluentIconBase &icon,
                                           bool isSelectable, QWidget* parent)
    : NavigationWidget(isSelectable, parent)
    , m_fluentIcon(icon.clone())
    , m_text(text)
{
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
    m_fluentIcon = icon.clone();
    update();
}

FluentIconBase* NavigationPushButton::fluentIcon() const
{
    return m_fluentIcon.get();
}

void NavigationPushButton::setIndicatorColor(const QColor& light, const QColor& dark) {
    m_lightIndicatorColor = light;
    m_darkIndicatorColor = dark;
    update();
}

QMargins NavigationPushButton::margins() const {
    return QMargins(0, 0, 0, 0);
}

bool NavigationPushButton::canDrawIndicator() const {
    return property("isSelected").toBool();
}

void NavigationPushButton::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);

    if (property("isPressed").toBool())
        painter.setOpacity(0.7);
    if (!isEnabled())
        painter.setOpacity(0.4);

    // 绘制背景
    const int colorValue = Theme::isDark() ? 255 : 0;
    const QMargins m = margins();
    const int leftMargin = m.left();
    const int rightMargin = m.right();

    const QPoint globalPos = mapToGlobal(QPoint(0, 0));
    const QRect globalRect(globalPos, size());

    if (canDrawIndicator()) {
        painter.setBrush(QColor(colorValue, colorValue, colorValue,
                                property("isEnter").toBool() ? 6 : 10));
        painter.drawRoundedRect(rect(), 5, 5);

        // 绘制指示器
        painter.setBrush(Theme::themeColor(Fluent::ThemeColor::PRIMARY));
        painter.drawRoundedRect(leftMargin, 10, 3, 16, 1.5, 1.5);
    } else if (property("isEnter").toBool() && isEnabled() &&
               globalRect.contains(QCursor::pos())) {
        painter.setBrush(QColor(colorValue, colorValue, colorValue, 10));
        painter.drawRoundedRect(rect(), 5, 5);
    }

    // 绘制图标
    if (m_fluentIcon) {
        FluentIconUtils::drawIcon(*m_fluentIcon, &painter,
                                  QRectF(11.5 + leftMargin, 10, 16, 16));
    }

    // 绘制文本
    if (property("isCompacted").toBool())
        return;

    painter.setPen(textColor());

    const int textLeft = m_fluentIcon ? 44 + leftMargin : leftMargin + 16;
    painter.drawText(QRectF(textLeft, 0, width() - 13 - textLeft - rightMargin, height()),
                     Qt::AlignVCenter, text());
}

// ============================================================================
// NavigationToolButton 实现
// ============================================================================

NavigationToolButton::NavigationToolButton(const FluentIconBase &icon, QWidget* parent)
    : NavigationPushButton("", icon, false, parent) {
    setFixedSize(40, 36);
    setProperty("isCompacted", false);
}

void NavigationToolButton::setCompacted(bool /*isCompacted*/) {
    setFixedSize(40, 36);
}

// ============================================================================
// NavigationSeparator 实现
// ============================================================================

NavigationSeparator::NavigationSeparator(QWidget* parent)
    : NavigationWidget(false, parent) {
    setCompacted(true);
}

void NavigationSeparator::setCompacted(bool isCompacted) {
    setFixedSize(isCompacted ? 48 : expandWidth() + 10, 3);
    update();
}

void NavigationSeparator::paintEvent(QPaintEvent* e) {
    Q_UNUSED(e);

    QPainter painter(this);
    const int colorValue = Theme::isDark() ? 255 : 0;
    QPen pen(QColor(colorValue, colorValue, colorValue, 15));
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawLine(0, 1, width(), 1);
}

// ============================================================================
// NavigationTreeItem 实现
// ============================================================================

NavigationTreeItem::NavigationTreeItem(const QString &text, const FluentIconBase &icon,
                                       bool isSelectable, NavigationTreeWidget* parent)
    : NavigationPushButton(text, icon, isSelectable, parent)
{
    m_rotateAnimation = new QPropertyAnimation(this, "arrowAngle", this);
}

void NavigationTreeItem::setExpanded(bool isExpanded) {
    if (m_rotateAnimation) {
        m_rotateAnimation->stop();
        m_rotateAnimation->setEndValue(isExpanded ? 180.0f : 0.0f);
        m_rotateAnimation->setDuration(150);
        m_rotateAnimation->start();
    }
}

void NavigationTreeItem::mouseReleaseEvent(QMouseEvent* e) {
    NavigationPushButton::mouseReleaseEvent(e);

    const bool clickArrow = QRectF(width() - 30, 8, 20, 20).contains(e->localPos());
    NavigationTreeWidget* parentWidget = qobject_cast<NavigationTreeWidget*>(parent());

    if (parentWidget) {
        emit itemClicked(true, clickArrow && !parentWidget->isLeaf());
    }
    update();
}

QMargins NavigationTreeItem::margins() const {
    NavigationTreeWidget* parentWidget = qobject_cast<NavigationTreeWidget*>(parent());
    if (parentWidget) {
        const int rightMargin = parentWidget->treeChildren().empty() ? 0 : 20;
        return QMargins(parentWidget->property("nodeDepth").toInt() * 28, 0, rightMargin, 0);
    }
    return QMargins(0, 0, 0, 0);
}

bool NavigationTreeItem::canDrawIndicator() const {
    NavigationTreeWidget* parentWidget = qobject_cast<NavigationTreeWidget*>(parent());
    if (!parentWidget)
        return false;

    if (parentWidget->isLeaf() || parentWidget->property("isSelected").toBool())
        return parentWidget->property("isSelected").toBool();

    for (auto child : parentWidget->treeChildren()) {
        if (child->itemWidget()->canDrawIndicator() && !child->isVisible())
            return true;
    }

    return false;
}

void NavigationTreeItem::paintEvent(QPaintEvent* e) {
    NavigationPushButton::paintEvent(e);

    NavigationTreeWidget* parentWidget = qobject_cast<NavigationTreeWidget*>(parent());
    if (!parentWidget || property("isCompacted").toBool() || parentWidget->treeChildren().empty())
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    if (property("isPressed").toBool())
        painter.setOpacity(0.7);
    if (!isEnabled())
        painter.setOpacity(0.4);

    painter.translate(width() - 20, 18);
    painter.rotate(m_arrowAngle);

    FluentIcon(Fluent::IconType::ARROW_DOWN).render(&painter, QRectF(-5, -5, 9.6, 9.6));
}

float NavigationTreeItem::arrowAngle() const {
    return m_arrowAngle;
}

void NavigationTreeItem::setArrowAngle(float angle) {
    m_arrowAngle = angle;
    update();
}

// ============================================================================
// NavigationTreeWidget 实现
// ============================================================================

NavigationTreeWidget::NavigationTreeWidget(const QString &text, const FluentIconBase &icon,
                                           bool isSelectable, QWidget* parent)
    : NavigationTreeWidgetBase(isSelectable, parent)
    , m_fluentIcon(icon.clone())
{
    m_itemWidget = new NavigationTreeItem(text, icon, isSelectable, this);
    m_vBoxLayout = new QVBoxLayout(this);
    m_expandAnimation = new QPropertyAnimation(this, "geometry", this);

    initWidget();
}

void NavigationTreeWidget::initWidget() {
    m_vBoxLayout->setSpacing(4);
    m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_vBoxLayout->addWidget(m_itemWidget, 0, Qt::AlignTop);

    connect(m_itemWidget, &NavigationTreeItem::itemClicked,
            this, &NavigationTreeWidget::onItemClicked);

    setAttribute(Qt::WA_TranslucentBackground);

    connect(m_expandAnimation, &QPropertyAnimation::valueChanged,
            this, [this](const QVariant& value) {
        setFixedSize(value.toRect().size());
    });

    connect(m_expandAnimation, &QPropertyAnimation::valueChanged,
            this, &NavigationTreeWidget::expanded);

    connect(m_expandAnimation, &QPropertyAnimation::finished,
            this, [this]() {
        if (parentWidget() && parentWidget()->layout()) {
            parentWidget()->layout()->invalidate();
        }
    });
}

void NavigationTreeWidget::addChild(NavigationWidget* child) {
    insertChild(-1, child);
}

QString NavigationTreeWidget::text() const {
    return m_itemWidget ? m_itemWidget->text() : QString();
}

void NavigationTreeWidget::setText(const QString& text) {
    if (m_itemWidget) {
        m_itemWidget->setText(text);
    }
}

void NavigationTreeWidget::setFluentIcon(const FluentIconBase &icon)
{
    m_fluentIcon = icon.clone();
    if (m_itemWidget) {
        m_itemWidget->setFluentIcon(*m_fluentIcon);
    }
}

FluentIconBase* NavigationTreeWidget::fluentIcon() const
{
    return m_fluentIcon.get();
}

void NavigationTreeWidget::setIndicatorColor(const QColor& light, const QColor& dark) {
    if (m_itemWidget) {
        m_itemWidget->setIndicatorColor(light, dark);
    }
}

void NavigationTreeWidget::setFont(const QFont& font) {
    QWidget::setFont(font);
    if (m_itemWidget) {
        m_itemWidget->setFont(font);
    }
}

NavigationTreeWidget* NavigationTreeWidget::clone() const {
    NavigationTreeWidget* root = new NavigationTreeWidget(
                text(), *m_fluentIcon, property("isSelectable").toBool(), parentWidget());

    root->setSelected(property("isSelected").toBool());
    root->setFixedSize(size());
    root->setProperty("nodeDepth", property("nodeDepth").toInt());

    connect(root, &NavigationTreeWidget::clicked,
            this, &NavigationTreeWidget::clicked);
    connect(this, &NavigationTreeWidget::selectedChanged,
            root, &NavigationTreeWidget::setSelected);

    for (auto child : m_treeChildren) {
        root->addChild(child->clone());
    }

    return root;
}

int NavigationTreeWidget::suitableWidth() const {
    if (!m_itemWidget)
        return 0;

    const QMargins m = m_itemWidget->margins();
    const int leftMargin = m_fluentIcon ? 57 + m.left() : m.left() + 29;
    const int textWidth = m_itemWidget->fontMetrics().boundingRect(text()).width();
    return leftMargin + textWidth + m.right();
}

void NavigationTreeWidget::insertChild(int index, NavigationWidget* child) {
    NavigationTreeWidget* treeChild = qobject_cast<NavigationTreeWidget*>(child);
    if (!treeChild || std::find(m_treeChildren.begin(), m_treeChildren.end(), treeChild)
            != m_treeChildren.end())
        return;

    treeChild->setTreeParent(this);
    treeChild->setProperty("nodeDepth", property("nodeDepth").toInt() + 1);
    treeChild->setVisible(m_isExpanded);

    connect(treeChild->m_expandAnimation, &QPropertyAnimation::valueChanged,
            this, [this]() {
        setFixedSize(sizeHint());
    });

    connect(treeChild->m_expandAnimation, &QPropertyAnimation::valueChanged,
            this, &NavigationTreeWidget::expanded);

    // 递归连接高度变化信号到父级
    NavigationTreeWidget* parentNode = qobject_cast<NavigationTreeWidget*>(treeParent());
    while (parentNode) {
        connect(treeChild->m_expandAnimation, &QPropertyAnimation::valueChanged,
                parentNode, [parentNode]() {
            parentNode->setFixedSize(parentNode->sizeHint());
        });
        parentNode = qobject_cast<NavigationTreeWidget*>(parentNode->treeParent());
    }

    if (index < 0) {
        index = static_cast<int>(m_treeChildren.size());
    }

    m_treeChildren.insert(m_treeChildren.begin() + index, treeChild);
    m_vBoxLayout->insertWidget(index + 1, treeChild, 0, Qt::AlignTop);

    // 调整高度
    if (m_isExpanded) {
        setFixedHeight(height() + treeChild->height() + m_vBoxLayout->spacing());

        NavigationTreeWidget* parentNode = qobject_cast<NavigationTreeWidget*>(treeParent());
        while (parentNode) {
            parentNode->setFixedSize(parentNode->sizeHint());
            parentNode = qobject_cast<NavigationTreeWidget*>(parentNode->treeParent());
        }
    }

    update();
}

void NavigationTreeWidget::removeChild(NavigationWidget* child) {
    NavigationTreeWidget* treeChild = qobject_cast<NavigationTreeWidget*>(child);
    if (!treeChild)
        return;

    auto it = std::find(m_treeChildren.begin(), m_treeChildren.end(), treeChild);
    if (it != m_treeChildren.end()) {
        m_treeChildren.erase(it);
        m_vBoxLayout->removeWidget(treeChild);
    }
}

std::vector<NavigationWidget*> NavigationTreeWidget::childItems() const {
    std::vector<NavigationWidget*> result;
    result.reserve(m_treeChildren.size());
    for (auto child : m_treeChildren) {
        result.push_back(child);
    }
    return result;
}

std::vector<NavigationTreeWidget*> NavigationTreeWidget::treeChildren() const
{
    return m_treeChildren;
}

NavigationTreeItem* NavigationTreeWidget::itemWidget() const
{
    return m_itemWidget;
}

void NavigationTreeWidget::setExpanded(bool isExpanded) {
    setExpanded(isExpanded, false);
}

void NavigationTreeWidget::setExpanded(bool isExpanded, bool animated) {
    if (isExpanded == m_isExpanded)
        return;

    m_isExpanded = isExpanded;
    if (m_itemWidget) {
        m_itemWidget->setExpanded(isExpanded);
    }

    for (auto child : m_treeChildren) {
        child->setVisible(isExpanded);
        child->setFixedSize(child->sizeHint());
    }

    if (animated && m_expandAnimation) {
        m_expandAnimation->stop();
        m_expandAnimation->setStartValue(geometry());
        m_expandAnimation->setEndValue(QRect(pos(), sizeHint()));
        m_expandAnimation->setDuration(120);
        m_expandAnimation->setEasingCurve(QEasingCurve::OutQuad);
        m_expandAnimation->start();
    } else {
        setFixedSize(sizeHint());
    }
}

bool NavigationTreeWidget::isRoot() const {
    return treeParent() == nullptr;
}

bool NavigationTreeWidget::isLeaf() const {
    return m_treeChildren.empty();
}

void NavigationTreeWidget::setSelected(bool isSelected) {
    NavigationWidget::setSelected(isSelected);
    if (m_itemWidget) {
        m_itemWidget->setSelected(isSelected);
    }
}

void NavigationTreeWidget::mouseReleaseEvent(QMouseEvent* e) {
    Q_UNUSED(e);
    // 空实现，不处理鼠标释放事件
}

void NavigationTreeWidget::setCompacted(bool isCompacted) {
    NavigationWidget::setCompacted(isCompacted);
    if (m_itemWidget) {
        m_itemWidget->setCompacted(isCompacted);
    }
    update();
}

void NavigationTreeWidget::onItemClicked(bool triggerByUser, bool clickArrow) {
    if (!property("isCompacted").toBool()) {
        if (property("isSelectable").toBool() && !property("isSelected").toBool() && !clickArrow) {
            setExpanded(true, true);
        } else {
            setExpanded(!m_isExpanded, true);
        }
    }

    if (!clickArrow || property("isCompacted").toBool()) {
        emit clicked(triggerByUser);
    }
}

void NavigationTreeWidget::setExpandWidth(int width) {
    NavigationTreeWidgetBase::setExpandWidth(width);
    if (m_itemWidget) {
        m_itemWidget->setExpandWidth(width);
    }
}

// ============================================================================
// NavigationFlyoutMenu 实现
// ============================================================================

NavigationFlyoutMenu::NavigationFlyoutMenu(NavigationTreeWidget* tree, QWidget* parent)
    : ScrollArea(parent)
    , m_treeWidget(tree)
{
    if (!tree) {
        return;
    }

    setViewportMargins(0, 0, 0, 0);

    m_view = new QWidget(this);
    m_vBoxLayout = new QVBoxLayout(m_view);

    setWidget(m_view);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("ScrollArea { border: none; background: transparent; }");
    m_view->setStyleSheet("QWidget { border: none; background: transparent; }");

    m_vBoxLayout->setSpacing(5);
    m_vBoxLayout->setContentsMargins(5, 8, 5, 8);

    // 添加节点到菜单
    for (auto child : m_treeWidget->treeChildren()) {
        NavigationTreeWidget* node = child->clone();
        connect(node, &NavigationTreeWidget::expanded,
                this, [this]() { adjustViewSize(); });

        m_treeChildren.push_back(node);
        m_vBoxLayout->addWidget(node);
    }

    for (auto node : m_treeChildren) {
        node->setProperty("nodeDepth", node->property("nodeDepth").toInt() - 1);
        node->setCompacted(false);
        if (node->isLeaf()) {
            connect(node, &NavigationTreeWidget::clicked,
                    window(), &QWidget::close);
        }
        initNode(node);
    }

    adjustViewSize(false);
}

void NavigationFlyoutMenu::initNode(NavigationTreeWidget* root) {
    if (!root)
        return;

    for (auto child : root->treeChildren()) {
        child->setProperty("nodeDepth", child->property("nodeDepth").toInt() - 1);
        child->setCompacted(false);

        if (child->isLeaf()) {
            connect(child, &NavigationTreeWidget::clicked,
                    window(), &QWidget::close);
        }

        initNode(child);
    }
}

void NavigationFlyoutMenu::adjustViewSize(bool emitSignal) {
    if (!m_view)
        return;

    const int width = suitableWidth();

    // 调整节点宽度
    for (auto node : visibleTreeNodes()) {
        node->setFixedWidth(width - 10);
        if (node->itemWidget()) {
            node->itemWidget()->setFixedWidth(width - 10);
        }
    }

    m_view->setFixedSize(width, m_view->sizeHint().height());

    QWidget* w = window();
    QWidget* pw = w ? w->parentWidget() : nullptr;
    if (!pw) return;

    const int height = qMin(pw->height() - 48, m_view->height());
    setFixedSize(width, height);

    if (emitSignal)
        emit expanded();
}

int NavigationFlyoutMenu::suitableWidth() const {
    int width = 0;

    for (auto node : visibleTreeNodes()) {
        if (!node->isHidden()) {
            width = qMax(width, node->suitableWidth() + 10);
        }
    }

    QWidget* windowWidget = window()->parentWidget();
    if (windowWidget) {
        return qMin(windowWidget->width() / 2 - 25, width) + 10;
    }
    return width + 10;
}

std::vector<NavigationTreeWidget*> NavigationFlyoutMenu::visibleTreeNodes() const {
    std::vector<NavigationTreeWidget*> nodes;
    std::deque<NavigationTreeWidget*> queue(m_treeChildren.begin(), m_treeChildren.end());

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

// ============================================================================
// NavigationAvatarWidget 实现
// ============================================================================

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
    if (m_avatar) {
        m_avatar->setText(name);
    }
    update();
}

void NavigationAvatarWidget::setAvatar(const QVariant &avatar)
{
    if (!m_avatar)
        return;

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
    } else if (avatar.canConvert<QIcon>()) {
        QIcon icon = avatar.value<QIcon>();
        QList<QSize> sizes = icon.availableSizes();
        if (!sizes.isEmpty()) {
            m_avatar->setImage(icon.pixmap(sizes.first()));
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

    // 绘制悬停背景
    if (property("isEnter").toBool()) {
        const int colorValue = Theme::isDark() ? 255 : 0;
        painter.setBrush(QColor(colorValue, colorValue, colorValue, 10));
        painter.drawRoundedRect(rect(), 5, 5);
    }

    // 在非紧凑模式绘制文本
    if (!property("isCompacted").toBool()) {
        painter.setPen(textColor());
        painter.setFont(font());
        painter.drawText(QRect(44, 0, 255, 36), Qt::AlignVCenter, m_name);
    }
}

// ============================================================================
// NavigationUserCard 实现
// ============================================================================

NavigationUserCard::NavigationUserCard(QWidget *parent)
    : NavigationAvatarWidget("", QVariant(), parent)
{
    // 初始化动画组
    m_animationGroup = new QParallelAnimationGroup(this);

    // Avatar radius 动画
    AvatarWidget *avatar = findChild<AvatarWidget*>();
    if (avatar) {
        m_radiusAnimation = new QPropertyAnimation(avatar, "radius", this);
        m_radiusAnimation->setDuration(m_animationDuration);
        m_radiusAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_animationGroup->addAnimation(m_radiusAnimation);

        m_posAnimation = new QPropertyAnimation(avatar, "pos", this);
        m_posAnimation->setDuration(m_animationDuration);
        m_posAnimation->setEasingCurve(QEasingCurve::OutCubic);

        m_animationGroup->addAnimation(m_posAnimation);
    }

    connect(m_animationGroup, &QParallelAnimationGroup::finished,
            this, QOverload<>::of(&QWidget::update));

    m_sizeAnimation = new QVariantAnimation(this);
    m_sizeAnimation->setDuration(m_animationDuration);
    m_sizeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_sizeAnimation, &QVariantAnimation::valueChanged,
            this, [this](const QVariant &value){
        setFixedSize(value.toSize());
    });
    m_animationGroup->addAnimation(m_sizeAnimation);

    setFixedSize(40, 36);
}

void NavigationUserCard::setAvatarIcon(const QIcon &icon)
{
    AvatarWidget *avatar = findChild<AvatarWidget*>();
    if (avatar) {
        QList<QSize> sizes = icon.availableSizes();
        if (!sizes.isEmpty()) {
            avatar->setImage(icon.pixmap(sizes.first()));
        }
    }
    update();
}

void NavigationUserCard::setAvatarBackgroundColor(const QColor &light, const QColor &dark)
{
    AvatarWidget *avatar = findChild<AvatarWidget*>();
    if (avatar) {
        avatar->setBackgroundColor(light, dark);
    }
    update();
}

QString NavigationUserCard::title() const
{
    return m_title;
}

void NavigationUserCard::setTitle(const QString &title)
{
    m_title = title;
    setName(title);
    update();
}

QString NavigationUserCard::subtitle() const
{
    return m_subtitle;
}

void NavigationUserCard::setSubtitle(const QString &subtitle)
{
    m_subtitle = subtitle;
    update();
}

void NavigationUserCard::setTitleFontSize(int size)
{
    m_titleSize = size;
    update();
}

void NavigationUserCard::setSubtitleFontSize(int size)
{
    m_subtitleSize = size;
    update();
}

void NavigationUserCard::setAnimationDuration(int duration)
{
    m_animationDuration = duration;
    if (m_radiusAnimation) {
        m_radiusAnimation->setDuration(duration);
    }
    if (m_posAnimation) {
        m_posAnimation->setDuration(duration);
    }
    if (m_sizeAnimation) {
        m_sizeAnimation->setDuration(duration);
    }
}

void NavigationUserCard::setCompacted(bool isCompacted)
{
    if (isCompacted == property("isCompacted").toBool())
        return;

    setProperty("isCompacted", isCompacted);

    m_radiusAnimation->setEasingCurve(isCompacted ? QEasingCurve::OutCubic : QEasingCurve::InOutSine);
    m_sizeAnimation->setEasingCurve(isCompacted ? QEasingCurve::OutCubic : QEasingCurve::InOutSine);
    m_posAnimation->setEasingCurve(isCompacted ? QEasingCurve::OutCubic : QEasingCurve::InOutSine);

    AvatarWidget *avatar = findChild<AvatarWidget*>();
    if (!avatar)
        return;

    if (isCompacted) {
        if (m_sizeAnimation) {
            m_sizeAnimation->setStartValue(size());
            m_sizeAnimation->setEndValue(QSize(40, 36));
        }
        if (m_radiusAnimation) {
            m_radiusAnimation->setStartValue(avatar->radius());
            m_radiusAnimation->setEndValue(12);
        }
        if (m_posAnimation) {
            m_posAnimation->setStartValue(avatar->pos());
            m_posAnimation->setEndValue(QPoint(8, 6));
        }
    } else {
        if (m_sizeAnimation) {
            m_sizeAnimation->setStartValue(size());
            m_sizeAnimation->setEndValue(QSize(expandWidth(), 80));
        }
        if (m_radiusAnimation) {
            m_radiusAnimation->setStartValue(avatar->radius());
            m_radiusAnimation->setEndValue(32);
        }
        if (m_posAnimation) {
            m_posAnimation->setStartValue(avatar->pos());
            m_posAnimation->setEndValue(QPoint(16, 8));
        }
    }

    if (m_animationGroup) {
        m_animationGroup->start();
    }
}

float NavigationUserCard::textOpacity() const
{
    return m_textOpacity;
}

void NavigationUserCard::setTextOpacity(float opacity)
{
    m_textOpacity = opacity;
    update();
}

QColor NavigationUserCard::subtitleColor() const
{
    return m_subtitleColor;
}

void NavigationUserCard::setSubtitleColor(const QColor &color)
{
    m_subtitleColor = color;
    update();
}

void NavigationUserCard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform |
                           QPainter::Antialiasing |
                           QPainter::TextAntialiasing);

    if (property("isPressed").toBool()) {
        painter.setOpacity(0.7);
    }

    // 绘制悬停背景
    if (property("isEnter").toBool()) {
        const int colorValue = Theme::isDark() ? 255 : 0;
        painter.setBrush(QColor(colorValue, colorValue, colorValue, 10));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), 5, 5);
    }

    // 在展开模式下绘制文本
    if (!property("isCompacted").toBool() && m_textOpacity > 0) {
        drawText(painter);
    }
}

void NavigationUserCard::drawText(QPainter &painter)
{
    AvatarWidget *avatar = findChild<AvatarWidget*>();
    if (!avatar)
        return;

    const int textX = 16 + static_cast<int>(avatar->radius() * 2) + 12;
    const int textWidth = width() - textX - 16;

    // 绘制标题
    QFont titleFont = font();
    titleFont.setPointSize(m_titleSize);
    titleFont.setBold(true);
    painter.setFont(titleFont);

    QColor color = textColor();
    color.setAlpha(static_cast<int>(255 * m_textOpacity));
    painter.setPen(color);

    const int titleY = height() / 2 - 2;
    painter.drawText(QRectF(textX, 0, textWidth, titleY),
                     Qt::AlignLeft | Qt::AlignBottom,
                     m_title);

    // 绘制副标题
    if (!m_subtitle.isEmpty()) {
        QFont subtitleFont = font();
        subtitleFont.setPointSize(m_subtitleSize);
        painter.setFont(subtitleFont);

        QColor subtitleColor = m_subtitleColor.isValid() ? m_subtitleColor : textColor();
        subtitleColor.setAlpha(static_cast<int>(150 * m_textOpacity));
        painter.setPen(subtitleColor);

        const int subtitleY = height() / 2 + 2;
        painter.drawText(QRectF(textX, subtitleY, textWidth, height() - subtitleY),
                         Qt::AlignLeft | Qt::AlignTop,
                         m_subtitle);
    }
}

// ============================================================================
// NavigationIndicator 实现
// ============================================================================

NavigationIndicator::NavigationIndicator(QWidget *parent)
    : QWidget(parent)
{
    m_scaleSlideAnimation = new ScaleSlideAnimation(this, Qt::Vertical);

    resize(3, 16);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    hide();

    connect(m_scaleSlideAnimation, &ScaleSlideAnimation::valueChanged,
            this, [this](const QVariant &value) {
        setGeometry(value.toRectF().toRect());
    });

    connect(m_scaleSlideAnimation, &ScaleSlideAnimation::finished,
            this, &NavigationIndicator::animationFinished);
}

void NavigationIndicator::startAnimation(const QRectF &startRect, const QRectF &endRect,
                                         bool useCrossFade)
{
    setGeometry(startRect.toRect());
    show();

    if (m_scaleSlideAnimation) {
        m_scaleSlideAnimation->setGeometry(startRect);
        m_scaleSlideAnimation->startAnimation(endRect, useCrossFade);
    }
}

void NavigationIndicator::stopAnimation()
{
    if (m_scaleSlideAnimation) {
        m_scaleSlideAnimation->stopAnimation();
    }
    hide();
}

void NavigationIndicator::setIndicatorColor(const QColor &light, const QColor &dark)
{
    m_lightColor = light;
    m_darkColor = dark;
    update();
}

void NavigationIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    QColor color = Theme::isDark() ? m_darkColor : m_lightColor;
    if (!color.isValid()) {
        color = Theme::themeColor(Fluent::ThemeColor::PRIMARY);
    }

    painter.setBrush(color);
    painter.drawRoundedRect(rect(), 1.5, 1.5);
}

// ============================================================================
// NavigationItemHeader 实现
// ============================================================================

NavigationItemHeader::NavigationItemHeader(const QString &text, QWidget *parent)
    : NavigationWidget(false, parent)
    , m_text(text)
{
    Theme::setFont(this, 12);

    // 设置标题样式的灰色文本颜色
    setLightTextColor(QColor(96, 96, 96));
    setDarkTextColor(QColor(160, 160, 160));

    // 高度动画
    m_heightAnimation = new QPropertyAnimation(this, "maximumHeight", this);
    m_heightAnimation->setDuration(150);
    m_heightAnimation->setEasingCurve(QEasingCurve::OutQuad);
    connect(m_heightAnimation, &QPropertyAnimation::valueChanged,
            this, &NavigationItemHeader::onHeightChanged);

    setCursor(Qt::ArrowCursor);
    setFixedHeight(0);
}

QString NavigationItemHeader::text() const
{
    return m_text;
}

void NavigationItemHeader::setText(const QString &text)
{
    m_text = text;
    update();
}

void NavigationItemHeader::setCompacted(bool isCompacted)
{
    setProperty("isCompacted", isCompacted);

    if (m_heightAnimation) {
        m_heightAnimation->stop();
    }

    if (isCompacted) {
        setFixedWidth(40);
        if (m_heightAnimation) {
            m_heightAnimation->setStartValue(height());
            m_heightAnimation->setEndValue(0);
            connect(m_heightAnimation, &QPropertyAnimation::finished,
                    this, &NavigationItemHeader::onCollapseFinished,
                    Qt::UniqueConnection);
        }
    } else {
        setFixedWidth(expandWidth());
        setVisible(true);
        if (m_heightAnimation) {
            m_heightAnimation->setStartValue(height());
            m_heightAnimation->setEndValue(m_targetHeight);
            disconnect(m_heightAnimation, &QPropertyAnimation::finished,
                       this, &NavigationItemHeader::onCollapseFinished);
        }
    }

    if (m_heightAnimation) {
        m_heightAnimation->start();
    }
    update();
}

void NavigationItemHeader::onCollapseFinished()
{
    setVisible(false);
    if (m_heightAnimation) {
        disconnect(m_heightAnimation, &QPropertyAnimation::finished,
                   this, &NavigationItemHeader::onCollapseFinished);
    }
}

void NavigationItemHeader::onHeightChanged(const QVariant &value)
{
    setFixedHeight(value.toInt());
}

void NavigationItemHeader::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (height() == 0 || !isVisible())
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 根据高度计算透明度
    const qreal opacity = qMin(1.0, static_cast<qreal>(height()) / qMax(1, m_targetHeight));
    painter.setOpacity(opacity);

    if (!property("isCompacted").toBool()) {
        painter.setFont(font());
        painter.setPen(textColor());
        painter.drawText(QRectF(16, 0, width() - 16, height()),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         m_text);
    }
}

void NavigationItemHeader::mousePressEvent(QMouseEvent *e)
{
    e->ignore();
}

void NavigationItemHeader::mouseReleaseEvent(QMouseEvent *e)
{
    e->ignore();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void NavigationItemHeader::enterEvent(QEnterEvent *e) {
    Q_UNUSED(e);
}
#else
void NavigationItemHeader::enterEvent(QEvent *e) {
    Q_UNUSED(e);
}
#endif

void NavigationItemHeader::leaveEvent(QEvent *e)
{
    Q_UNUSED(e);
}
