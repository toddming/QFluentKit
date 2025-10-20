#include "FlowLayout.h"
#include <QWidgetItem>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QVariant>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEvent>
#include <QWidget>
#include <QVector>
#include <QObject>


QT_BEGIN_NAMESPACE

FlowLayout::FlowLayout(QWidget *parent, bool needAni, bool isTight)
    : QLayout(parent)
    , _verticalSpacing(10)
    , _horizontalSpacing(10)
    , _duration(300)
    , _ease(QEasingCurve::Linear)
    , _needAni(needAni)
    , _isTight(isTight)
    , _wParent(nullptr)
    , _isInstalledEventFilter(false)
{
    _debounceTimer = new QTimer(this);
    _debounceTimer->setSingleShot(true);
    // _debounceTimer->timeout.connect(this, [this]() {  });

    connect(_debounceTimer, &QTimer::timeout, this, [this](){
        this->_doLayout(geometry(), true);
    });
    _aniGroup = new QParallelAnimationGroup(this);
    setContentsMargins(0, 0, 0, 0);
}

FlowLayout::~FlowLayout()
{
    qDeleteAll(_items);
    qDeleteAll(_animations);
}

void FlowLayout::addItem(QLayoutItem *item)
{
    _items.append(item);
}

void FlowLayout::insertItem(int index, QLayoutItem *item)
{
    _items.insert(index, item);
}

void FlowLayout::addWidget(QWidget *widget)
{
    addItem(new QWidgetItem(widget));
    _onWidgetAdded(widget);
}

void FlowLayout::insertWidget(int index, QWidget *widget)
{
    insertItem(index, new QWidgetItem(widget));
    widget->setParent(parentWidget());
    _onWidgetAdded(widget, index);
}

void FlowLayout::_onWidgetAdded(QWidget *widget, int index)
{
    if (!_isInstalledEventFilter) {
        if (widget->parentWidget()) {
            _wParent = widget->parentWidget();
            _wParent->installEventFilter(this);
        } else {
            widget->installEventFilter(this);
        }
        _isInstalledEventFilter = true;
    }

    if (!_needAni)
        return;

    QPropertyAnimation *ani = new QPropertyAnimation(widget, "geometry");
    ani->setEndValue(QRect(QPoint(0, 0), widget->size()));
    ani->setDuration(_duration);
    ani->setEasingCurve(QEasingCurve(_ease));
    widget->setProperty("flowAni", QVariant::fromValue(ani));
    _aniGroup->addAnimation(ani);

    if (index == -1)
        _animations.append(ani);
    else
        _animations.insert(index, ani);
}

void FlowLayout::setAnimation(int duration, QEasingCurve::Type ease)
{
    if (!_needAni)
        return;

    _duration = duration;
    _ease = ease;

    for (QPropertyAnimation *ani : std::as_const(_animations)) {
        if (ani) {
            ani->setDuration(duration);
            ani->setEasingCurve(QEasingCurve(ease));
        }
    }
}

int FlowLayout::count() const
{
    return _items.count();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
    if (index >= 0 && index < _items.size())
        return _items.at(index);
    return nullptr;
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index < 0 || index >= _items.size())
        return nullptr;

    QLayoutItem *item = _items.takeAt(index);
    if (!item)
        return nullptr;

    QWidget *widget = item->widget();
    if (widget) {
        QVariant aniVar = widget->property("flowAni");
        if (aniVar.isValid() && aniVar.canConvert<QPropertyAnimation *>()) {
            QPropertyAnimation *ani = aniVar.value<QPropertyAnimation *>();
            _animations.removeOne(ani);
            _aniGroup->removeAnimation(ani);
            ani->deleteLater();
            widget->setProperty("flowAni", QVariant());
        }
    }

    delete item;
    return nullptr;  // 符合 QLayout 规范，返回 nullptr 并删除 item
}

void FlowLayout::removeWidget(QWidget *widget)
{
    for (int i = 0; i < count(); ++i) {
        if (itemAt(i)->widget() == widget) {
            takeAt(i);
            break;
        }
    }
}

void FlowLayout::removeAllWidgets()
{
    while (!_items.isEmpty())
        takeAt(0);
}

void FlowLayout::takeAllWidgets()
{
    while (!_items.isEmpty()) {
        QWidget *w = itemAt(0)->widget();
        takeAt(0);
        if (w)
            w->deleteLater();
    }
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return Qt::Orientation(0);
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    return _calculateHeight(QRect(0, 0, width, 0));
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    for (QLayoutItem *item : std::as_const(_items)) {
        size = size.expandedTo(item->minimumSize());
    }
    QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

void FlowLayout::setVerticalSpacing(int spacing)
{
    _verticalSpacing = spacing;
}

int FlowLayout::verticalSpacing() const
{
    return _verticalSpacing;
}

void FlowLayout::setHorizontalSpacing(int spacing)
{
    _horizontalSpacing = spacing;
}

int FlowLayout::horizontalSpacing() const
{
    return _horizontalSpacing;
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    if (_needAni) {
        _debounceTimer->start(80);
    } else {
        _doLayout(rect, true);
    }
}

bool FlowLayout::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::ParentChange) {
        if (QWidget *widget = qobject_cast<QWidget *>(object)) {
            // 检查 widget 是否在布局中（简化检查）
            for (QLayoutItem *item : std::as_const(_items)) {
                if (item && item->widget() == widget) {
                    _wParent = widget->parentWidget();
                    if (_wParent)
                        _wParent->installEventFilter(this);
                    _isInstalledEventFilter = true;
                    break;
                }
            }
        }
    } else if (event->type() == QEvent::Show && object == _wParent) {
        _doLayout(geometry(), true);
        _isInstalledEventFilter = true;
    }
    return QLayout::eventFilter(object, event);
}

// 新增：const 方法，仅计算高度（用于 heightForWidth）
int FlowLayout::_calculateHeight(const QRect &rect) const
{
    QMargins margin = contentsMargins();
    int x = rect.x() + margin.left();
    int y = rect.y() + margin.top();
    int rowHeight = 0;
    int spaceX = _horizontalSpacing;
    int spaceY = _verticalSpacing;

    for (int i = 0; i < _items.count(); ++i) {
        QLayoutItem *item = _items.at(i);
        if (!item)
            continue;

        QWidget *widget = item->widget();
        if (widget && !widget->isVisible() && _isTight)
            continue;

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > rect.right() - margin.right() && rowHeight > 0) {
            x = rect.x() + margin.left();
            y += rowHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            rowHeight = 0;
        }

        x = nextX;
        rowHeight = qMax(rowHeight, item->sizeHint().height());
    }

    return y + rowHeight + margin.bottom() - rect.y();
}

// 修正：非 const 方法，用于实际布局（调用 _calculateHeight 获取高度，但添加移动逻辑）
int FlowLayout::_doLayout(const QRect &rect, bool move)
{
    bool aniRestart = false;
    QMargins margin = contentsMargins();
    int x = rect.x() + margin.left();
    int y = rect.y() + margin.top();
    int rowHeight = 0;
    int spaceX = _horizontalSpacing;
    int spaceY = _verticalSpacing;

    // 临时存储位置，用于计算高度（复用 _calculateHeight 的逻辑，但这里需要为移动设置位置）
    // 注意：由于 _calculateHeight 是 const，我们在非 const 中模拟相同循环，但添加 move 逻辑
    for (int i = 0; i < _items.count(); ++i) {
        QLayoutItem *item = _items.at(i);
        if (!item)
            continue;

        QWidget *widget = item->widget();
        if (widget && !widget->isVisible() && _isTight)
            continue;

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > rect.right() - margin.right() && rowHeight > 0) {
            x = rect.x() + margin.left();
            y += rowHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            rowHeight = 0;
        }

        if (move) {
            QRect target(x, y, item->sizeHint().width(), item->sizeHint().height());
            if (!_needAni) {
                item->setGeometry(target);
            } else if (i < _animations.size()) {
                QVariant endVar = _animations.at(i)->endValue();
                if (endVar.isValid() && target != endVar.toRect()) {
                    QPropertyAnimation *ani = _animations.at(i);
                    if (ani) {
                        ani->stop();
                        ani->setEndValue(target);
                        aniRestart = true;
                    }
                }
            }
        }

        x = nextX;
        rowHeight = qMax(rowHeight, item->sizeHint().height());
    }

    if (_needAni && aniRestart) {
        _aniGroup->stop();
        _aniGroup->start();
    }

    return y + rowHeight + margin.bottom() - rect.y();  // 返回计算的高度
}

QT_END_NAMESPACE
