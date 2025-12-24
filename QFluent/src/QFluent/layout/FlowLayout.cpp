#include "FlowLayout.h"
#include <QWidget>
#include <QWidgetItem>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEvent>
#include <QVariant>
#include <QtMath>

QT_BEGIN_NAMESPACE

// 匿名命名空间，用于内部常量
namespace {
    constexpr int DEFAULT_VERTICAL_SPACING = 10;
    constexpr int DEFAULT_HORIZONTAL_SPACING = 10;
    constexpr int DEFAULT_ANIMATION_DURATION = 300;
    constexpr int LAYOUT_DEBOUNCE_MS = 80;
    const char *ANIMATION_PROPERTY_NAME = "flowLayoutAnimation";
}

FlowLayout::FlowLayout(QWidget *parent, bool enableAnimation, bool tightMode)
    : QLayout(parent)
    , m_animationGroup(nullptr)
    , m_verticalSpacing(DEFAULT_VERTICAL_SPACING)
    , m_horizontalSpacing(DEFAULT_HORIZONTAL_SPACING)
    , m_animationDuration(DEFAULT_ANIMATION_DURATION)
    , m_easingCurve(QEasingCurve::Linear)
    , m_animationEnabled(enableAnimation)
    , m_tightMode(tightMode)
    , m_eventFilterInstalled(false)
    , m_layoutDebounceTimer(nullptr)
    , m_parentWidget(nullptr)
{
    // 设置默认边距
    setContentsMargins(0, 0, 0, 0);

    // 创建防抖定时器
    m_layoutDebounceTimer = new QTimer(this);
    m_layoutDebounceTimer->setSingleShot(true);
    m_layoutDebounceTimer->setInterval(LAYOUT_DEBOUNCE_MS);

    connect(m_layoutDebounceTimer, &QTimer::timeout, this, [this]() {
        doLayout(geometry(), true);
    });

    // 仅在需要动画时创建动画组
    if (m_animationEnabled) {
        m_animationGroup = new QParallelAnimationGroup(this);
    }
}

FlowLayout::~FlowLayout()
{
    // 清理所有布局项
    qDeleteAll(m_items);
    m_items.clear();

    // 清理所有动画（如果存在）
    if (!m_animations.isEmpty()) {
        qDeleteAll(m_animations);
        m_animations.clear();
    }

    // 定时器和动画组会自动通过 QObject 父子关系删除
}

void FlowLayout::addItem(QLayoutItem *item)
{
    if (!item) {
        return;
    }

    m_items.append(item);

    // 如果是控件项，进行额外处理
    if (QWidget *widget = item->widget()) {
        onWidgetAdded(widget);
    }
}

void FlowLayout::insertItem(int index, QLayoutItem *item)
{
    if (!item) {
        return;
    }

    // 确保索引有效
    if (index < 0 || index > m_items.size()) {
        index = m_items.size();
    }

    m_items.insert(index, item);

    // 如果是控件项，进行额外处理
    if (QWidget *widget = item->widget()) {
        onWidgetAdded(widget, index);
    }
}

void FlowLayout::addWidget(QWidget *widget)
{
    if (!widget) {
        return;
    }

    addItem(new QWidgetItem(widget));
}

void FlowLayout::insertWidget(int index, QWidget *widget)
{
    if (!widget) {
        return;
    }

    // 确保控件的父窗口正确
    if (widget->parent() != parentWidget()) {
        widget->setParent(parentWidget());
    }

    insertItem(index, new QWidgetItem(widget));
}

void FlowLayout::onWidgetAdded(QWidget *widget, int index)
{
    if (!widget) {
        return;
    }

    // 安装事件过滤器（仅一次）
    if (!m_eventFilterInstalled) {
        if (QWidget *parent = widget->parentWidget()) {
            m_parentWidget = parent;
            m_parentWidget->installEventFilter(this);
            m_eventFilterInstalled = true;
        } else {
            // 如果控件暂时没有父窗口，监听其父窗口变化
            widget->installEventFilter(this);
        }
    }

    // 仅在启用动画时创建动画对象
    if (!m_animationEnabled) {
        return;
    }

    // 创建位置动画
    QPropertyAnimation *animation = new QPropertyAnimation(widget, "geometry", this);
    animation->setEndValue(QRect(QPoint(0, 0), widget->size()));
    animation->setDuration(m_animationDuration);
    animation->setEasingCurve(QEasingCurve(m_easingCurve));

    // 将动画关联到控件
    widget->setProperty(ANIMATION_PROPERTY_NAME, QVariant::fromValue(animation));

    // 添加到动画组
    if (m_animationGroup) {
        m_animationGroup->addAnimation(animation);
    }

    // 添加到动画列表
    if (index == -1 || index >= m_animations.size()) {
        m_animations.append(animation);
    } else {
        m_animations.insert(index, animation);
    }
}

void FlowLayout::setAnimation(int duration, QEasingCurve::Type easing)
{
    if (!m_animationEnabled) {
        return;
    }

    m_animationDuration = qMax(0, duration);
    m_easingCurve = easing;

    // 更新所有现有动画
    for (QPropertyAnimation *animation : qAsConst(m_animations)) {
        if (animation) {
            animation->setDuration(m_animationDuration);
            animation->setEasingCurve(QEasingCurve(easing));
        }
    }
}

int FlowLayout::count() const
{
    return m_items.count();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    if (index >= 0 && index < m_items.size()) {
        return m_items.at(index);
    }
    return nullptr;
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index < 0 || index >= m_items.size()) {
        return nullptr;
    }

    QLayoutItem *item = m_items.takeAt(index);
    if (!item) {
        return nullptr;
    }

    // 清理关联的动画
    if (QWidget *widget = item->widget()) {
        cleanupAnimation(widget);
    }

    // 返回布局项，调用者负责删除
    return item;
}

void FlowLayout::cleanupAnimation(QWidget *widget)
{
    if (!widget || !m_animationEnabled) {
        return;
    }

    QVariant animationVariant = widget->property(ANIMATION_PROPERTY_NAME);
    if (!animationVariant.isValid() || !animationVariant.canConvert<QPropertyAnimation *>()) {
        return;
    }

    QPropertyAnimation *animation = animationVariant.value<QPropertyAnimation *>();
    if (!animation) {
        return;
    }

    // 从列表和动画组中移除
    m_animations.removeOne(animation);

    if (m_animationGroup) {
        m_animationGroup->removeAnimation(animation);
    }

    // 删除动画对象
    animation->deleteLater();

    // 清除控件属性
    widget->setProperty(ANIMATION_PROPERTY_NAME, QVariant());
}

void FlowLayout::removeWidget(QWidget *widget)
{
    if (!widget) {
        return;
    }

    for (int i = 0; i < count(); ++i) {
        QLayoutItem *item = itemAt(i);
        if (item && item->widget() == widget) {
            QLayoutItem *taken = takeAt(i);
            delete taken;
            break;
        }
    }
}

void FlowLayout::removeAllWidgets()
{
    while (!m_items.isEmpty()) {
        QLayoutItem *item = takeAt(0);
        delete item;
    }
}

void FlowLayout::takeAllWidgets()
{
    while (!m_items.isEmpty()) {
        QLayoutItem *item = takeAt(0);
        if (item) {
            if (QWidget *widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
    }
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return Qt::Orientations();
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    return calculateHeight(QRect(0, 0, width, 0));
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;

    for (const QLayoutItem *item : qAsConst(m_items)) {
        if (item) {
            size = size.expandedTo(item->minimumSize());
        }
    }

    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(),
                  margins.top() + margins.bottom());

    return size;
}

void FlowLayout::setVerticalSpacing(int spacing)
{
    m_verticalSpacing = qMax(0, spacing);
}

int FlowLayout::verticalSpacing() const
{
    return m_verticalSpacing;
}

void FlowLayout::setHorizontalSpacing(int spacing)
{
    m_horizontalSpacing = qMax(0, spacing);
}

int FlowLayout::horizontalSpacing() const
{
    return m_horizontalSpacing;
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);

    if (m_animationEnabled && m_layoutDebounceTimer) {
        // 使用防抖机制避免频繁布局
        m_layoutDebounceTimer->start();
    } else {
        // 直接执行布局
        doLayout(rect, true);
    }
}

bool FlowLayout::eventFilter(QObject *object, QEvent *event)
{
    if (!event) {
        return QLayout::eventFilter(object, event);
    }

    if (event->type() == QEvent::ParentChange) {
        // 处理控件父窗口变化
        QWidget *widget = qobject_cast<QWidget *>(object);
        if (widget) {
            // 检查该控件是否在布局中
            for (const QLayoutItem *item : qAsConst(m_items)) {
                if (item && item->widget() == widget) {
                    QWidget *newParent = widget->parentWidget();
                    if (newParent && newParent != m_parentWidget) {
                        m_parentWidget = newParent;
                        m_parentWidget->installEventFilter(this);
                        m_eventFilterInstalled = true;
                    }
                    break;
                }
            }
        }
    } else if (event->type() == QEvent::Show && object == m_parentWidget) {
        // 父窗口显示时重新布局
        doLayout(geometry(), true);
    }

    return QLayout::eventFilter(object, event);
}

int FlowLayout::calculateHeight(const QRect &rect) const
{
    const QMargins margins = contentsMargins();
    int x = rect.x() + margins.left();
    int y = rect.y() + margins.top();
    int lineHeight = 0;
    const int maxWidth = rect.right() - margins.right();

    for (const QLayoutItem *item : qAsConst(m_items)) {
        if (!item) {
            continue;
        }

        // 紧凑模式下跳过不可见的控件
        const QWidget *widget = item->widget();
        if (m_tightMode && widget && !widget->isVisible()) {
            continue;
        }

        const QSize itemSize = item->sizeHint();
        const int nextX = x + itemSize.width();

        // 检查是否需要换行
        if (nextX > maxWidth && lineHeight > 0) {
            x = rect.x() + margins.left();
            y += lineHeight + m_verticalSpacing;
            lineHeight = 0;
        }

        x += itemSize.width() + m_horizontalSpacing;
        lineHeight = qMax(lineHeight, itemSize.height());
    }

    return y + lineHeight + margins.bottom() - rect.y();
}

int FlowLayout::doLayout(const QRect &rect, bool applyGeometry)
{
    const QMargins margins = contentsMargins();
    int x = rect.x() + margins.left();
    int y = rect.y() + margins.top();
    int lineHeight = 0;
    const int maxWidth = rect.right() - margins.right();
    bool animationNeedsRestart = false;

    for (int i = 0; i < m_items.count(); ++i) {
        QLayoutItem *item = m_items.at(i);
        if (!item) {
            continue;
        }

        // 紧凑模式下跳过不可见的控件
        QWidget *widget = item->widget();
        if (m_tightMode && widget && !widget->isVisible()) {
            continue;
        }

        const QSize itemSize = item->sizeHint();
        const int nextX = x + itemSize.width();

        // 检查是否需要换行
        if (nextX > maxWidth && lineHeight > 0) {
            x = rect.x() + margins.left();
            y += lineHeight + m_verticalSpacing;
            lineHeight = 0;
        }

        // 应用几何位置
        if (applyGeometry) {
            const QRect targetGeometry(x, y, itemSize.width(), itemSize.height());

            if (!m_animationEnabled) {
                // 直接设置位置
                item->setGeometry(targetGeometry);
            } else if (i < m_animations.size() && m_animations.at(i)) {
                // 使用动画
                QPropertyAnimation *animation = m_animations.at(i);
                const QVariant currentEndValue = animation->endValue();

                // 仅当目标位置改变时才重启动画
                if (!currentEndValue.isValid() ||
                    currentEndValue.toRect() != targetGeometry) {
                    animation->stop();
                    animation->setEndValue(targetGeometry);
                    animationNeedsRestart = true;
                }
            }
        }

        x += itemSize.width() + m_horizontalSpacing;
        lineHeight = qMax(lineHeight, itemSize.height());
    }

    // 启动动画组
    if (m_animationEnabled && animationNeedsRestart && m_animationGroup) {
        m_animationGroup->stop();
        m_animationGroup->start();
    }

    return y + lineHeight + margins.bottom() - rect.y();
}

QT_END_NAMESPACE
