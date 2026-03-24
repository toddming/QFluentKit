#include "CycleListWidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QKeyEvent>

#include "Theme.h"
#include "QFluent/ScrollBar.h"

// ScrollButton 实现
ScrollButton::ScrollButton(const FluentIconBase &icon, QWidget* parent)
    : QToolButton(parent)
    , m_isPressed(false)
    , m_fluentIcon(icon.clone())
{
    installEventFilter(this);
}

bool ScrollButton::eventFilter(QObject* obj, QEvent* e)
{
    if (obj == this) {
        if (e->type() == QEvent::MouseButtonPress) {
            m_isPressed = true;
            update();
        } else if (e->type() == QEvent::MouseButtonRelease) {
            m_isPressed = false;
            update();
        }
    }
    return QToolButton::eventFilter(obj, e);
}

void ScrollButton::paintEvent(QPaintEvent* e)
{
    QToolButton::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = m_isPressed ? 8 : 10;
    int h = m_isPressed ? 8 : 10;
    qreal x = (width() - w) / 2.0;
    qreal y = (height() - h) / 2.0;

    if (!Theme::instance()->isDarkTheme()) {
        QHash<QString, QString> attrs;
        attrs["fill"] = "#5e5e5e";
        FluentIconUtils::drawIcon(*m_fluentIcon, &painter, QRectF(x, y, w, h),
                                 Fluent::ThemeMode::AUTO, false, attrs);
    } else {
        FluentIconUtils::drawIcon(*m_fluentIcon, &painter, QRectF(x, y, w, h));
    }
}


// CycleListWidget 实现
CycleListWidget::CycleListWidget(const QStringList& items, const QSize& itemSize,
                                 Qt::Alignment align, QWidget* parent)
    : QListWidget(parent)
    , m_itemSize(itemSize)
    , m_align(align)
    , m_upButton(nullptr)
    , m_downButton(nullptr)
    , m_visibleNumber(9)
    , m_currentIndex(0)
    , m_isCycle(false)
{
    m_originItems = items;

    // 创建滚动按钮（设置this为父对象，Qt会自动管理内存）
    m_upButton = new ScrollButton(FluentIcon(Fluent::IconType::CARE_UP_SOLID), this);
    m_downButton = new ScrollButton(FluentIcon(Fluent::IconType::CARE_DOWN_SOLID), this);

    // 设置自定义滚动条（Qt会接管所有权）
    setVerticalScrollBar(new ScrollBar(this));

    // 初始化列表项
    setItems(items);

    // 配置滚动模式
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // 设置视图边距和大小
    setViewportMargins(0, 0, 0, 0);
    setFixedSize(itemSize.width() + 8, itemSize.height() * m_visibleNumber);

    // 隐藏滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 初始时隐藏滚动按钮
    m_upButton->hide();
    m_downButton->hide();

    // 连接信号槽
    connect(m_upButton, &QToolButton::clicked, this, &CycleListWidget::scrollUp);
    connect(m_downButton, &QToolButton::clicked, this, &CycleListWidget::scrollDown);
    connect(this, &QListWidget::itemClicked, this, &CycleListWidget::onItemClicked);

    installEventFilter(this);
}

void CycleListWidget::setItems(const QStringList& items)
{
    clear();
    m_originItems = items;
    createItems(items);
}

void CycleListWidget::createItems(const QStringList& items)
{
    int N = items.size();
    m_isCycle = N > m_visibleNumber;

    if (m_isCycle) {
        // 循环模式：添加两列相同的项以实现无限滚动效果
        for (int i = 0; i < 2; ++i) {
            addColumnItems(items);
        }
        m_currentIndex = items.size();
        QListWidget::scrollToItem(item(m_currentIndex - m_visibleNumber / 2),
                                  QAbstractItemView::PositionAtTop);
    } else {
        // 非循环模式：在顶部和底部添加空白占位符
        int n = m_visibleNumber / 2;
        QStringList empty;
        for (int i = 0; i < n; ++i) {
            empty << "";
        }

        addColumnItems(empty, true);
        addColumnItems(items);
        addColumnItems(empty, true);

        m_currentIndex = n;
    }
}

void CycleListWidget::addColumnItems(const QStringList& items, bool disabled)
{
    for (const QString& text : items) {
        QListWidgetItem* listItem = new QListWidgetItem(text, this);
        listItem->setSizeHint(m_itemSize);
        listItem->setTextAlignment(m_align | Qt::AlignVCenter);
        if (disabled) {
            listItem->setFlags(Qt::NoItemFlags);
        }
        addItem(listItem);
    }
}

void CycleListWidget::onItemClicked(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    setCurrentIndex(row(item));
    scrollToItem(item);
}

void CycleListWidget::setSelectedItem(const QString& text)
{
    if (text.isEmpty()) {
        return;
    }

    QList<QListWidgetItem*> items = findItems(text, Qt::MatchExactly);
    if (items.isEmpty()) {
        return;
    }

    // 如果有多个相同项（循环模式），选择第二个
    if (items.size() >= 2) {
        setCurrentIndex(row(items[1]));
    } else {
        setCurrentIndex(row(items[0]));
    }

    // 使用延迟调用确保滚动在UI更新后执行
    QMetaObject::invokeMethod(this, [this]() {
        QListWidget::scrollToItem(currentItem(), QAbstractItemView::PositionAtCenter);
    }, Qt::QueuedConnection);
}

void CycleListWidget::scrollToItem(QListWidgetItem* item, QAbstractItemView::ScrollHint hint)
{
    if (!item) {
        return;
    }

    int index = row(item);
    int y = item->sizeHint().height() * (index - m_visibleNumber / 2);
    verticalScrollBar()->setValue(y);

    // 清除选中状态
    clearSelection();

    emit currentItemChanged(item);
}

void CycleListWidget::wheelEvent(QWheelEvent* e)
{
    if (e->angleDelta().y() < 0) {
        scrollDown();
    } else {
        scrollUp();
    }
}

void CycleListWidget::scrollDown()
{
    setCurrentIndex(m_currentIndex + 1);
    scrollToItem(currentItem());
}

void CycleListWidget::scrollUp()
{
    setCurrentIndex(m_currentIndex - 1);
    scrollToItem(currentItem());
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void CycleListWidget::enterEvent(QEnterEvent* e)
{
    m_upButton->show();
    m_downButton->show();
    QListWidget::enterEvent(e);
}
#else
void CycleListWidget::enterEvent(QEvent* e)
{
    m_upButton->show();
    m_downButton->show();
    QListWidget::enterEvent(e);
}
#endif

void CycleListWidget::leaveEvent(QEvent* e)
{
    m_upButton->hide();
    m_downButton->hide();
    QListWidget::leaveEvent(e);
}

void CycleListWidget::resizeEvent(QResizeEvent* e)
{
    // 调整滚动按钮的位置和大小
    m_upButton->resize(width(), 34);
    m_downButton->resize(width(), 34);
    m_downButton->move(0, height() - 34);
    QListWidget::resizeEvent(e);
}

bool CycleListWidget::eventFilter(QObject* obj, QEvent* e)
{
    if (obj == this && e->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
        if (keyEvent->key() == Qt::Key_Down) {
            scrollDown();
            return true;
        } else if (keyEvent->key() == Qt::Key_Up) {
            scrollUp();
            return true;
        }
    }
    return QListWidget::eventFilter(obj, e);
}

QListWidgetItem* CycleListWidget::currentItem()
{
    return item(m_currentIndex);
}

void CycleListWidget::setCurrentIndex(int index)
{
    if (!m_isCycle) {
        // 非循环模式：限制索引在有效范围内
        int n = m_visibleNumber / 2;
        m_currentIndex = qMax(n, qMin(n + m_originItems.size() - 1, index));
    } else {
        // 循环模式：实现无限滚动
        int N = count() / 2;
        int m = (m_visibleNumber + 1) / 2;
        m_currentIndex = index;

        // 如果接近底部，跳转到上半部分的对应位置
        if (index >= count() - m) {
            m_currentIndex = N + index - count();
            QListWidget::scrollToItem(item(m_currentIndex - 1), PositionAtCenter);
        }
        // 如果接近顶部，跳转到下半部分的对应位置
        else if (index <= m - 1) {
            m_currentIndex = N + index;
            QListWidget::scrollToItem(item(N + index + 1), PositionAtCenter);
        }
    }
}
