#include "TeachingTip.h"

#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <QTimer>
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QPixmap>

#include "Theme.h"
#include "Screen.h"
#include "QFluent/ImageLabel.h"

// ============================================================================
// TeachingTipView 实现
// ============================================================================
TeachingTipView::TeachingTipView(const QString& title,
                                 const QString& content,
                                 const QIcon& icon,
                                 const QPixmap& image,
                                 bool isClosable,
                                 TeachingTipTailPosition tailPosition,
                                 QWidget* parent)
    : FlyoutView(title, content, icon, image, isClosable, parent)
    , m_manager(nullptr)
    , m_hBoxLayout(nullptr)
{
    m_manager = TeachingTipManager::make(tailPosition);
    m_hBoxLayout = new QHBoxLayout();
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);
}

void TeachingTipView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    // TeachingTipView 不绘制背景，由 TeachTipBubble 绘制
}

void TeachingTipView::adjustImage()
{
    if (!m_manager || !imageLabel() || !vBoxLayout()) {
        return;
    }

    ImagePosition pos = m_manager->imagePosition();
    if (pos == ImagePosition::TOP || pos == ImagePosition::BOTTOM) {
        FlyoutView::adjustImage();
        return;
    }

    int h = vBoxLayout()->sizeHint().height() - 2;
    imageLabel()->scaledToHeight(h);
}

void TeachingTipView::addImageToLayout()
{
    if (!imageLabel() || !m_manager || !vBoxLayout() || !viewLayout() || !m_hBoxLayout) {
        return;
    }

    imageLabel()->setHidden(imageLabel()->isNull());
    ImagePosition pos = m_manager->imagePosition();

    if (pos == ImagePosition::TOP) {
        imageLabel()->setBorderRadius(8, 8, 0, 0);
        vBoxLayout()->insertWidget(0, imageLabel());
    }
    else if (pos == ImagePosition::BOTTOM) {
        imageLabel()->setBorderRadius(0, 0, 8, 8);
        vBoxLayout()->addWidget(imageLabel());
    }
    else if (pos == ImagePosition::LEFT) {
        QLayoutItem* item = vBoxLayout()->itemAt(0);
        if (item) {
            vBoxLayout()->removeItem(item);
        }
        m_hBoxLayout->addLayout(viewLayout());
        vBoxLayout()->addLayout(m_hBoxLayout);

        imageLabel()->setBorderRadius(8, 0, 8, 0);
        m_hBoxLayout->insertWidget(0, imageLabel());
    }
    else if (pos == ImagePosition::RIGHT) {
        QLayoutItem* item = vBoxLayout()->itemAt(0);
        if (item) {
            vBoxLayout()->removeItem(item);
        }
        m_hBoxLayout->addLayout(viewLayout());
        vBoxLayout()->addLayout(m_hBoxLayout);

        imageLabel()->setBorderRadius(0, 8, 0, 8);
        m_hBoxLayout->addWidget(imageLabel());
    }
}

// ============================================================================
// TeachTipBubble 实现
// ============================================================================
TeachTipBubble::TeachTipBubble(FlyoutViewBase* view,
                               TeachingTipTailPosition tailPosition,
                               QWidget* parent)
    : QWidget(parent)
    , m_manager(nullptr)
    , m_hBoxLayout(nullptr)
    , m_view(view)
{
    m_manager = TeachingTipManager::make(tailPosition);
    m_hBoxLayout = new QHBoxLayout(this);

    if (m_manager) {
        m_manager->doLayout(this);
    }

    if (m_hBoxLayout && m_view) {
        m_hBoxLayout->addWidget(m_view);
    }
}

void TeachTipBubble::setView(QWidget* view)
{
    if (!m_hBoxLayout || !view) {
        return;
    }

    if (m_view) {
        m_hBoxLayout->removeWidget(m_view);
        m_view->deleteLater();
    }

    m_view = qobject_cast<FlyoutViewBase*>(view);
    m_hBoxLayout->addWidget(view);
}

void TeachTipBubble::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    if (!m_manager) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    bool isDark = Theme::instance()->isDarkTheme();
    painter.setBrush(isDark ? QColor(40, 40, 40) : QColor(248, 248, 248));
    painter.setPen(isDark ? QColor(23, 23, 23) : QColor(0, 0, 0, 17));

    m_manager->draw(this, painter);
}

// ============================================================================
// TeachingTip 实现
// ============================================================================
TeachingTip::TeachingTip(FlyoutViewBase* view,
                         QWidget* target,
                         int duration,
                         TeachingTipTailPosition tailPosition,
                         QWidget* parent,
                         bool isDeleteOnClose)
    : QWidget(parent)
    , target(target)
    , duration(duration)
    , m_manager(nullptr)
    , m_hBoxLayout(nullptr)
    , m_opacityAni(nullptr)
    , m_bubble(nullptr)
    , m_shadowEffect(nullptr)
    , m_isDeleteOnClose(isDeleteOnClose)
{
    m_manager = TeachingTipManager::make(tailPosition);
    m_hBoxLayout = new QHBoxLayout(this);
    m_opacityAni = new QPropertyAnimation(this, "windowOpacity", this);
    m_bubble = new TeachTipBubble(view, tailPosition, this);

    if (m_hBoxLayout) {
        m_hBoxLayout->setContentsMargins(15, 8, 15, 20);
        m_hBoxLayout->addWidget(m_bubble);
    }

    setShadowEffect();

    // 设置样式
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    if (parent && parent->window()) {
        parent->window()->installEventFilter(this);
    }
}

void TeachingTip::setShadowEffect(int blurRadius, const QPoint& offset)
{
    if (!m_bubble) {
        return;
    }

    QColor color = Theme::instance()->isDarkTheme()
        ? QColor(0, 0, 0, 80)
        : QColor(0, 0, 0, 30);

    m_shadowEffect = new QGraphicsDropShadowEffect(m_bubble);
    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(offset);
    m_shadowEffect->setColor(color);
    m_bubble->setGraphicsEffect(m_shadowEffect);
}

void TeachingTip::fadeOut()
{
    if (!m_opacityAni) {
        return;
    }

    m_opacityAni->setDuration(167);
    m_opacityAni->setStartValue(1.0);
    m_opacityAni->setEndValue(0.0);
    connect(m_opacityAni, &QPropertyAnimation::finished, this, &TeachingTip::close);
    m_opacityAni->start();
}

void TeachingTip::showEvent(QShowEvent* event)
{
    if (duration >= 0) {
        QTimer::singleShot(duration, this, &TeachingTip::fadeOut);
    }

    if (m_manager) {
        move(m_manager->position(this));
    }

    adjustSize();

    if (m_opacityAni) {
        m_opacityAni->setDuration(167);
        m_opacityAni->setStartValue(0.0);
        m_opacityAni->setEndValue(1.0);
        m_opacityAni->start();
    }

    QWidget::showEvent(event);
}

void TeachingTip::closeEvent(QCloseEvent* event)
{
    if (m_isDeleteOnClose) {
        deleteLater();
    }
    QWidget::closeEvent(event);
}

bool TeachingTip::eventFilter(QObject* obj, QEvent* event)
{
    QWidget* p = qobject_cast<QWidget*>(parent());
    if (p && obj == p->window()) {
        QEvent::Type type = event->type();
        if (type == QEvent::Resize ||
            type == QEvent::WindowStateChange ||
            type == QEvent::Move) {
            if (m_manager) {
                move(m_manager->position(this));
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void TeachingTip::setView(FlyoutViewBase* view)
{
    if (m_bubble) {
        m_bubble->setView(view);
    }
}

void TeachingTip::addWidget(QWidget* widget, int stretch, Qt::Alignment align)
{
    FlyoutViewBase* v = view();
    if (v && widget) {
        v->addWidget(widget, stretch, align);
    }
}

TeachingTip* TeachingTip::make(FlyoutViewBase* view,
                               QWidget* target,
                               int duration,
                               TeachingTipTailPosition tailPosition,
                               QWidget* parent,
                               bool isDeleteOnClose)
{
    if (!view || !target) {
        return nullptr;
    }

    TeachingTip* tip = new TeachingTip(view, target, duration, tailPosition,
                                       parent, isDeleteOnClose);
    tip->show();
    return tip;
}

TeachingTip* TeachingTip::create(QWidget* target,
                                 const QString& title,
                                 const QString& content,
                                 const QIcon& icon,
                                 const QPixmap& image,
                                 bool isClosable,
                                 int duration,
                                 TeachingTipTailPosition tailPosition,
                                 QWidget* parent,
                                 bool isDeleteOnClose)
{
    if (!target) {
        return nullptr;
    }

    TeachingTipView* view = new TeachingTipView(title, content, icon, image,
                                                isClosable, tailPosition);
    TeachingTip* tip = make(view, target, duration, tailPosition, parent, isDeleteOnClose);

    if (tip && view) {
        connect(view, &TeachingTipView::closed, tip, &TeachingTip::close);
    }

    return tip;
}

// ============================================================================
// PopupTeachingTip 实现
// ============================================================================
PopupTeachingTip::PopupTeachingTip(FlyoutViewBase* view,
                                   QWidget* target,
                                   int duration,
                                   TeachingTipTailPosition tailPosition,
                                   QWidget* parent,
                                   bool isDeleteOnClose)
    : TeachingTip(view, target, duration, tailPosition, parent, isDeleteOnClose)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
}

// ============================================================================
// TeachingTipManager 实现
// ============================================================================
TeachingTipManager::TeachingTipManager(QObject* parent)
    : QObject(parent)
{
}

void TeachingTipManager::doLayout(TeachTipBubble* tip)
{
    tip->hBoxLayout()->setContentsMargins(0, 0, 0, 0);
}

ImagePosition TeachingTipManager::imagePosition() const
{
    return ImagePosition::TOP;
}

QPoint TeachingTipManager::position(TeachingTip* tip)
{
    QPoint p = pos(tip);
    int x = p.x();
    int y = p.y();

    QRect rect = Screen::currentScreenGeometry();
    x = qMax(rect.left(), qMin(p.x(), rect.right() - tip->width() - 4));
    y = qMax(rect.top(), qMin(p.y(), rect.bottom() - tip->height() - 4));

    return QPoint(x, y);
}

void TeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    QRect rect = tip->rect().adjusted(1, 1, -1, -1);
    painter.drawRoundedRect(rect, 8, 8);
}

QPoint TeachingTipManager::pos(TeachingTip* tip)
{
    return tip->pos();
}

TeachingTipManager* TeachingTipManager::make(TeachingTipTailPosition position)
{
    switch (position) {
        case TeachingTipTailPosition::TOP:
            return new TopTailTeachingTipManager();
        case TeachingTipTailPosition::BOTTOM:
            return new BottomTailTeachingTipManager();
        case TeachingTipTailPosition::LEFT:
            return new LeftTailTeachingTipManager();
        case TeachingTipTailPosition::RIGHT:
            return new RightTailTeachingTipManager();
        case TeachingTipTailPosition::TOP_LEFT:
            return new TopLeftTailTeachingTipManager();
        case TeachingTipTailPosition::TOP_RIGHT:
            return new TopRightTailTeachingTipManager();
        case TeachingTipTailPosition::BOTTOM_LEFT:
            return new BottomLeftTailTeachingTipManager();
        case TeachingTipTailPosition::BOTTOM_RIGHT:
            return new BottomRightTailTeachingTipManager();
        case TeachingTipTailPosition::LEFT_TOP:
            return new LeftTopTailTeachingTipManager();
        case TeachingTipTailPosition::LEFT_BOTTOM:
            return new LeftBottomTailTeachingTipManager();
        case TeachingTipTailPosition::RIGHT_TOP:
            return new RightTopTailTeachingTipManager();
        case TeachingTipTailPosition::RIGHT_BOTTOM:
            return new RightBottomTailTeachingTipManager();
        case TeachingTipTailPosition::NONE:
        default:
            return new TeachingTipManager();
    }
}

// ============================================================================
// TopTailTeachingTipManager 实现
// ============================================================================
TopTailTeachingTipManager::TopTailTeachingTipManager(QObject* parent)
    : TeachingTipManager(parent)
{
}

void TopTailTeachingTipManager::doLayout(TeachTipBubble* tip)
{
    tip->hBoxLayout()->setContentsMargins(0, 8, 0, 0);
}

ImagePosition TopTailTeachingTipManager::imagePosition() const
{
    return ImagePosition::BOTTOM;
}

void TopTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    int w = tip->width();
    int h = tip->height();
    int pt = tip->hBoxLayout()->contentsMargins().top();

    QPainterPath path;
    path.addRoundedRect(1, pt, w - 2, h - pt - 1, 8, 8);
    
    QPolygonF polygon;
    polygon << QPointF(w / 2.0 - 7, pt) << QPointF(w / 2.0, 1) << QPointF(w / 2.0 + 7, pt);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint TopTailTeachingTipManager::pos(TeachingTip* tip)
{
    QWidget* target = tip->target;
    QPoint p = target->mapToGlobal(QPoint(0, target->height()));
    int x = p.x() + target->width() / 2 - tip->sizeHint().width() / 2;
    int y = p.y() - tip->layout()->contentsMargins().top();
    return QPoint(x, y);
}

// ============================================================================
// BottomTailTeachingTipManager 实现
// ============================================================================
BottomTailTeachingTipManager::BottomTailTeachingTipManager(QObject* parent)
    : TeachingTipManager(parent)
{
}

void BottomTailTeachingTipManager::doLayout(TeachTipBubble* tip)
{
    tip->hBoxLayout()->setContentsMargins(0, 0, 0, 8);
}

void BottomTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    int w = tip->width();
    int h = tip->height();
    int pb = tip->hBoxLayout()->contentsMargins().bottom();

    QPainterPath path;
    path.addRoundedRect(1, 1, w - 2, h - pb - 1, 8, 8);
    
    QPolygonF polygon;
    polygon << QPointF(w / 2.0 - 7, h - pb) << QPointF(w / 2.0, h - 1) << QPointF(w / 2.0 + 7, h - pb);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint BottomTailTeachingTipManager::pos(TeachingTip* tip)
{
    QWidget* target = tip->target;
    QPoint p = target->mapToGlobal(QPoint());
    int x = p.x() + target->width() / 2 - tip->sizeHint().width() / 2;
    int y = p.y() - tip->sizeHint().height() + tip->layout()->contentsMargins().bottom();
    return QPoint(x, y);
}

// ============================================================================
// LeftTailTeachingTipManager 实现
// ============================================================================
LeftTailTeachingTipManager::LeftTailTeachingTipManager(QObject* parent)
    : TeachingTipManager(parent)
{
}

void LeftTailTeachingTipManager::doLayout(TeachTipBubble* tip)
{
    tip->hBoxLayout()->setContentsMargins(8, 0, 0, 0);
}

ImagePosition LeftTailTeachingTipManager::imagePosition() const
{
    return ImagePosition::RIGHT;
}

void LeftTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    int w = tip->width();
    int h = tip->height();
    int pl = 8;

    QPainterPath path;
    path.addRoundedRect(pl, 1, w - pl - 2, h - 2, 8, 8);
    
    QPolygonF polygon;
    polygon << QPointF(pl, h / 2.0 - 7) << QPointF(1, h / 2.0) << QPointF(pl, h / 2.0 + 7);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint LeftTailTeachingTipManager::pos(TeachingTip* tip)
{
    QWidget* target = tip->target;
    QMargins m = tip->layout()->contentsMargins();
    QPoint p = target->mapToGlobal(QPoint(target->width(), 0));
    int x = p.x() - m.left();
    int y = p.y() - tip->view()->sizeHint().height() / 2 + target->height() / 2 - m.top();
    return QPoint(x, y);
}

// ============================================================================
// RightTailTeachingTipManager 实现
// ============================================================================
RightTailTeachingTipManager::RightTailTeachingTipManager(QObject* parent)
    : TeachingTipManager(parent)
{
}

void RightTailTeachingTipManager::doLayout(TeachTipBubble* tip)
{
    tip->hBoxLayout()->setContentsMargins(0, 0, 8, 0);
}

ImagePosition RightTailTeachingTipManager::imagePosition() const
{
    return ImagePosition::LEFT;
}

void RightTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    int w = tip->width();
    int h = tip->height();
    int pr = 8;

    QPainterPath path;
    path.addRoundedRect(1, 1, w - pr - 1, h - 2, 8, 8);
    
    QPolygonF polygon;
    polygon << QPointF(w - pr, h / 2.0 - 7) << QPointF(w - 1, h / 2.0) << QPointF(w - pr, h / 2.0 + 7);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint RightTailTeachingTipManager::pos(TeachingTip* tip)
{
    QWidget* target = tip->target;
    QMargins m = tip->layout()->contentsMargins();
    QPoint p = target->mapToGlobal(QPoint(0, 0));
    int x = p.x() - tip->sizeHint().width() + m.right();
    int y = p.y() - tip->view()->sizeHint().height() / 2 + target->height() / 2 - m.top();
    return QPoint(x, y);
}

// ============================================================================
// TopLeftTailTeachingTipManager 实现
// ============================================================================
TopLeftTailTeachingTipManager::TopLeftTailTeachingTipManager(QObject* parent)
    : TopTailTeachingTipManager(parent)
{
}

void TopLeftTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    if (!tip || !tip->hBoxLayout()) {
        return;
    }

    int w = tip->width();
    int h = tip->height();
    int pt = tip->hBoxLayout()->contentsMargins().top();

    QPainterPath path;
    path.addRoundedRect(1, pt, w - 2, h - pt - 1, 8, 8);

    QPolygonF polygon;
    polygon << QPointF(20, pt) << QPointF(27, 1) << QPointF(34, pt);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint TopLeftTailTeachingTipManager::pos(TeachingTip* tip)
{
    if (!tip || !tip->target || !tip->layout()) {
        return QPoint();
    }

    QWidget* target = tip->target;
    QPoint p = target->mapToGlobal(QPoint(0, target->height()));
    int x = p.x() - tip->layout()->contentsMargins().left();
    int y = p.y() - tip->layout()->contentsMargins().top();

    return QPoint(x, y);
}

// ============================================================================
// TopRightTailTeachingTipManager 实现
// ============================================================================
TopRightTailTeachingTipManager::TopRightTailTeachingTipManager(QObject* parent)
    : TopTailTeachingTipManager(parent)
{
}

void TopRightTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    if (!tip || !tip->hBoxLayout()) {
        return;
    }

    int w = tip->width();
    int h = tip->height();
    int pt = tip->hBoxLayout()->contentsMargins().top();

    QPainterPath path;
    path.addRoundedRect(1, pt, w - 2, h - pt - 1, 8, 8);

    QPolygonF polygon;
    polygon << QPointF(w - 20, pt) << QPointF(w - 27, 1) << QPointF(w - 34, pt);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint TopRightTailTeachingTipManager::pos(TeachingTip* tip)
{
    if (!tip || !tip->target || !tip->layout()) {
        return QPoint();
    }

    QWidget* target = tip->target;
    QPoint p = target->mapToGlobal(QPoint(target->width(), target->height()));
    int x = p.x() - tip->sizeHint().width() + tip->layout()->contentsMargins().left();
    int y = p.y() - tip->layout()->contentsMargins().top();

    return QPoint(x, y);
}

// ============================================================================
// BottomLeftTailTeachingTipManager 实现
// ============================================================================
BottomLeftTailTeachingTipManager::BottomLeftTailTeachingTipManager(QObject* parent)
    : BottomTailTeachingTipManager(parent)
{
}

void BottomLeftTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    if (!tip || !tip->hBoxLayout()) {
        return;
    }

    int w = tip->width();
    int h = tip->height();
    int pb = tip->hBoxLayout()->contentsMargins().bottom();

    QPainterPath path;
    path.addRoundedRect(1, 1, w - 2, h - pb - 1, 8, 8);

    QPolygonF polygon;
    polygon << QPointF(20, h - pb) << QPointF(27, h - 1) << QPointF(34, h - pb);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint BottomLeftTailTeachingTipManager::pos(TeachingTip* tip)
{
    if (!tip || !tip->target || !tip->layout()) {
        return QPoint();
    }

    QWidget* target = tip->target;
    QPoint p = target->mapToGlobal(QPoint());
    int x = p.x() - tip->layout()->contentsMargins().left();
    int y = p.y() - tip->sizeHint().height() + tip->layout()->contentsMargins().bottom();

    return QPoint(x, y);
}

// ============================================================================
// BottomRightTailTeachingTipManager 实现
// ============================================================================
BottomRightTailTeachingTipManager::BottomRightTailTeachingTipManager(QObject* parent)
    : BottomTailTeachingTipManager(parent)
{
}

void BottomRightTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    if (!tip || !tip->hBoxLayout()) {
        return;
    }

    int w = tip->width();
    int h = tip->height();
    int pb = tip->hBoxLayout()->contentsMargins().bottom();

    QPainterPath path;
    path.addRoundedRect(1, 1, w - 2, h - pb - 1, 8, 8);

    QPolygonF polygon;
    polygon << QPointF(w - 20, h - pb)
            << QPointF(w - 27, h - 1)
            << QPointF(w - 34, h - pb);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint BottomRightTailTeachingTipManager::pos(TeachingTip* tip)
{
    if (!tip || !tip->target || !tip->layout()) {
        return QPoint();
    }

    QWidget* target = tip->target;
    QPoint p = target->mapToGlobal(QPoint(target->width(), 0));
    int x = p.x() - tip->sizeHint().width() + tip->layout()->contentsMargins().left();
    int y = p.y() - tip->sizeHint().height() + tip->layout()->contentsMargins().bottom();

    return QPoint(x, y);
}

// ============================================================================
// LeftTopTailTeachingTipManager 实现
// ============================================================================
LeftTopTailTeachingTipManager::LeftTopTailTeachingTipManager(QObject* parent)
    : LeftTailTeachingTipManager(parent)
{
}

ImagePosition LeftTopTailTeachingTipManager::imagePosition() const
{
    return ImagePosition::BOTTOM;
}

void LeftTopTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    if (!tip) {
        return;
    }

    int w = tip->width();
    int h = tip->height();
    int pl = 8;

    QPainterPath path;
    path.addRoundedRect(pl, 1, w - pl - 2, h - 2, 8, 8);

    QPolygonF polygon;
    polygon << QPointF(pl, 10) << QPointF(1, 17) << QPointF(pl, 24);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint LeftTopTailTeachingTipManager::pos(TeachingTip* tip)
{
    if (!tip || !tip->target || !tip->layout()) {
        return QPoint();
    }

    QWidget* target = tip->target;
    QMargins m = tip->layout()->contentsMargins();
    QPoint p = target->mapToGlobal(QPoint(target->width(), 0));
    int x = p.x() - m.left();
    int y = p.y() - m.top();

    return QPoint(x, y);
}

// ============================================================================
// LeftBottomTailTeachingTipManager 实现
// ============================================================================
LeftBottomTailTeachingTipManager::LeftBottomTailTeachingTipManager(QObject* parent)
    : LeftTailTeachingTipManager(parent)
{
}

ImagePosition LeftBottomTailTeachingTipManager::imagePosition() const
{
    return ImagePosition::TOP;
}

void LeftBottomTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    if (!tip) {
        return;
    }

    int w = tip->width();
    int h = tip->height();
    int pl = 9;

    QPainterPath path;
    path.addRoundedRect(pl, 1, w - pl - 1, h - 2, 8, 8);

    QPolygonF polygon;
    polygon << QPointF(pl, h - 10) << QPointF(1, h - 17) << QPointF(pl, h - 24);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint LeftBottomTailTeachingTipManager::pos(TeachingTip* tip)
{
    if (!tip || !tip->target || !tip->layout()) {
        return QPoint();
    }

    QWidget* target = tip->target;
    QMargins m = tip->layout()->contentsMargins();
    QPoint p = target->mapToGlobal(QPoint(target->width(), target->height()));
    int x = p.x() - m.left();
    int y = p.y() - tip->sizeHint().height() + m.bottom();

    return QPoint(x, y);
}

// ============================================================================
// RightTopTailTeachingTipManager 实现
// ============================================================================
RightTopTailTeachingTipManager::RightTopTailTeachingTipManager(QObject* parent)
    : RightTailTeachingTipManager(parent)
{
}

ImagePosition RightTopTailTeachingTipManager::imagePosition() const
{
    return ImagePosition::BOTTOM;
}

void RightTopTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    if (!tip) {
        return;
    }

    int w = tip->width();
    int h = tip->height();
    int pr = 8;

    QPainterPath path;
    path.addRoundedRect(1, 1, w - pr - 1, h - 2, 8, 8);

    QPolygonF polygon;
    polygon << QPointF(w - pr, 10) << QPointF(w - 1, 17) << QPointF(w - pr, 24);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint RightTopTailTeachingTipManager::pos(TeachingTip* tip)
{
    if (!tip || !tip->target || !tip->layout()) {
        return QPoint();
    }

    QWidget* target = tip->target;
    QMargins m = tip->layout()->contentsMargins();
    QPoint p = target->mapToGlobal(QPoint(0, 0));
    int x = p.x() - tip->sizeHint().width() + m.right();
    int y = p.y() - m.top();

    return QPoint(x, y);
}

// ============================================================================
// RightBottomTailTeachingTipManager 实现
// ============================================================================
RightBottomTailTeachingTipManager::RightBottomTailTeachingTipManager(QObject* parent)
    : RightTailTeachingTipManager(parent)
{
}

ImagePosition RightBottomTailTeachingTipManager::imagePosition() const
{
    return ImagePosition::TOP;
}

void RightBottomTailTeachingTipManager::draw(TeachTipBubble* tip, QPainter& painter)
{
    if (!tip) {
        return;
    }

    int w = tip->width();
    int h = tip->height();
    int pr = 8;

    QPainterPath path;
    path.addRoundedRect(1, 1, w - pr - 1, h - 2, 8, 8);

    QPolygonF polygon;
    polygon << QPointF(w - pr, 10) << QPointF(w - 1, 17) << QPointF(w - pr, 24);
    path.addPolygon(polygon);

    painter.drawPath(path.simplified());
}

QPoint RightBottomTailTeachingTipManager::pos(TeachingTip* tip)
{
    if (!tip || !tip->target || !tip->layout()) {
        return QPoint();
    }

    QWidget* target = tip->target;
    QMargins m = tip->layout()->contentsMargins();
    QPoint p = target->mapToGlobal(QPoint(0, target->height()));
    int x = p.x() - tip->sizeHint().width() + m.right();
    int y = p.y() - tip->sizeHint().height() + m.bottom();

    return QPoint(x, y);
}
