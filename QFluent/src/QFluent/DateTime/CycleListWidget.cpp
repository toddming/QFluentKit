#include "CycleListWidget.h"
#include <QPainter>
#include <QWheelEvent>
#include <QPropertyAnimation>

#include "Theme.h"
#include "QFluent/Scrollbar/ScrollBar.h"

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
        FluentIconUtils::drawIcon(*m_fluentIcon, &painter, QRectF(x, y, w, h), Fluent::ThemeMode::AUTO, QIcon::Off, attrs);
    } else {
        FluentIconUtils::drawIcon(*m_fluentIcon, &painter, QRectF(x, y, w, h));
    }
}


// CycleListWidget 实现
CycleListWidget::CycleListWidget(const QStringList& items, const QSize& itemSize,
                                 Qt::Alignment align, QWidget* parent)
    : QListWidget(parent), m_itemSize(itemSize), m_align(align),
      m_scrollDuration(250), m_visibleNumber(9), m_currentIndex(0), m_isCycle(false)
{
    m_originItems = items;
    
    m_upButton = new ScrollButton(FluentIcon(Fluent::IconType::CARE_UP_SOLID), this);
    m_downButton = new ScrollButton(FluentIcon(Fluent::IconType::CARE_DOWN_SOLID), this);
    
    m_vScrollBar = new SmoothScrollBar(Qt::Vertical, this);
    
    setItems(items);
    
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_vScrollBar->setScrollAnimation(m_scrollDuration);
    m_vScrollBar->setForceHidden(true);
    
    setViewportMargins(0, 0, 0, 0);
    setFixedSize(itemSize.width() + 8, itemSize.height() * m_visibleNumber);
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    m_upButton->hide();
    m_downButton->hide();
    
    connect(m_upButton, &QToolButton::clicked, this, &CycleListWidget::scrollUp);
    connect(m_downButton, &QToolButton::clicked, this, &CycleListWidget::scrollDown);
    connect(this, &QListWidget::itemClicked, this, &CycleListWidget::onItemClicked);
    
    installEventFilter(this);
}

void CycleListWidget::setItems(const QStringList& items)
{
    clear();
    createItems(items);
}

void CycleListWidget::createItems(const QStringList& items)
{
    int N = items.size();
    m_isCycle = N > m_visibleNumber;
    
    if (m_isCycle) {
        for (int i = 0; i < 2; ++i) {
            addColumnItems(items);
        }
        m_currentIndex = items.size();
        QListWidget::scrollToItem(item(m_currentIndex - m_visibleNumber/2),
                                  QAbstractItemView::PositionAtTop);
    } else {
        int n = m_visibleNumber / 2;
        QStringList empty;
        for (int i = 0; i < n; ++i) empty << "";
        
        addColumnItems(empty, true);
        addColumnItems(items);
        addColumnItems(empty, true);
        
        m_currentIndex = n;
    }
}

void CycleListWidget::addColumnItems(const QStringList& items, bool disabled)
{
    for (const QString& i : items) {
        QListWidgetItem* item = new QListWidgetItem(i, this);
        item->setSizeHint(m_itemSize);
        item->setTextAlignment(m_align | Qt::AlignVCenter);
        if (disabled) {
            item->setFlags(Qt::NoItemFlags);
        }
        addItem(item);
    }
}

void CycleListWidget::onItemClicked(QListWidgetItem* item)
{
    setCurrentIndex(row(item));
    scrollToItem(item);
}

void CycleListWidget::setSelectedItem(const QString& text)
{
    if (text.isEmpty()) return;
    
    QList<QListWidgetItem*> items = findItems(text, Qt::MatchExactly);
    if (items.isEmpty()) return;
    
    if (items.size() >= 2) {
        setCurrentIndex(row(items[1]));
    } else {
        setCurrentIndex(row(items[0]));
    }
    
    QMetaObject::invokeMethod(this, [this]() {
        QListWidget::scrollToItem(currentItem(), QAbstractItemView::PositionAtCenter);
    }, Qt::QueuedConnection);
}

void CycleListWidget::scrollToItem(QListWidgetItem* item, QAbstractItemView::ScrollHint hint)
{
    int index = row(item);
    int y = item->sizeHint().height() * (index - m_visibleNumber / 2);
    m_vScrollBar->scrollTo(y);
    
    clearSelection();
    item->setSelected(false);
    
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
void CycleListWidget::enterEvent(QEvent *e)
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
        int n = m_visibleNumber / 2;
        m_currentIndex = qMax(n, qMin(n + m_originItems.size() - 1, index));
    } else {
        int N = count() / 2;
        int m = (m_visibleNumber + 1) / 2;
        m_currentIndex = index;
        
        if (index >= count() - m) {
            m_currentIndex = N + index - count();
            QListWidget::scrollToItem(item(m_currentIndex - 1), PositionAtCenter);
        } else if (index <= m - 1) {
            m_currentIndex = N + index;
            QListWidget::scrollToItem(item(N + index + 1), PositionAtCenter);
        }
    }
}
