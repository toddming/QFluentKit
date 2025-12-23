#include "ScrollBar.h"

#include <QAbstractItemView>
#include <QAbstractScrollArea>
#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include "FluentIcon.h"
#include "SmoothScroll.h"
#include "Theme.h"

// ============================================================================
// ArrowButton 实现
// ============================================================================

ArrowButton::ArrowButton(const FluentIconBase &icon, QWidget* parent)
    : QToolButton(parent)
    , m_fluentIcon(icon.clone())
{
    setFixedSize(10, 10);
}

void ArrowButton::setOpacity(qreal opacity) {
    m_opacity = qBound(0.0, opacity, 1.0);
    update();
}

void ArrowButton::setLightColor(const QColor& color) {
    m_lightColor = color;
    update();
}

void ArrowButton::setDarkColor(const QColor& color) {
    m_darkColor = color;
    update();
}

void ArrowButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QColor color = Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
    painter.setOpacity(m_opacity * color.alphaF());

    int size = isDown() ? 7 : 8;
    qreal x = (width() - size) / 2.0;

    QHash<QString, QString> attributes;
    attributes["fill"] = color.name();
    FluentIconUtils::drawIcon(*m_fluentIcon, &painter, QRectF(x, x, size, size), 
                             Fluent::ThemeMode::AUTO, QIcon::Off, attributes);
}

// ============================================================================
// ScrollBarGroove 实现
// ============================================================================

ScrollBarGroove::ScrollBarGroove(Qt::Orientation orient, QWidget* parent) 
    : QWidget(parent) 
{
    m_opacityAnimation = new QPropertyAnimation(this, "opacity", this);

    if (orient == Qt::Vertical) {
        setFixedWidth(12);
        m_upButton = new ArrowButton(FluentIcon(Fluent::IconType::CARE_UP_SOLID), this);
        m_downButton = new ArrowButton(FluentIcon(Fluent::IconType::CARE_DOWN_SOLID), this);
        
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_upButton, 0, Qt::AlignHCenter);
        layout->addStretch(1);
        layout->addWidget(m_downButton, 0, Qt::AlignHCenter);
        layout->setContentsMargins(0, 3, 0, 3);
    } else {
        setFixedHeight(12);
        m_upButton = new ArrowButton(FluentIcon(Fluent::IconType::CARE_LEFT_SOLID), this);
        m_downButton = new ArrowButton(FluentIcon(Fluent::IconType::CARE_RIGHT_SOLID), this);
        
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->addWidget(m_upButton, 0, Qt::AlignVCenter);
        layout->addStretch(1);
        layout->addWidget(m_downButton, 0, Qt::AlignVCenter);
        layout->setContentsMargins(3, 0, 3, 0);
    }

    setOpacity(0.0);
}

void ScrollBarGroove::setLightBackgroundColor(const QColor& color) {
    m_lightBackgroundColor = color;
    update();
}

void ScrollBarGroove::setDarkBackgroundColor(const QColor& color) {
    m_darkBackgroundColor = color;
    update();
}

void ScrollBarGroove::fadeIn() {
    if (!m_opacityAnimation) return;
    
    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(m_opacity);
    m_opacityAnimation->setEndValue(1.0);
    m_opacityAnimation->setDuration(150);
    m_opacityAnimation->start();
}

void ScrollBarGroove::fadeOut() {
    if (!m_opacityAnimation) return;
    
    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(m_opacity);
    m_opacityAnimation->setEndValue(0.0);
    m_opacityAnimation->setDuration(150);
    m_opacityAnimation->start();
}

void ScrollBarGroove::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);
    painter.setPen(Qt::NoPen);

    QColor brushColor = Theme::instance()->isDarkTheme() 
                       ? m_darkBackgroundColor 
                       : m_lightBackgroundColor;
    painter.setBrush(brushColor);
    painter.drawRoundedRect(rect(), 6, 6);
}

void ScrollBarGroove::setOpacity(qreal opacity) {
    m_opacity = qBound(0.0, opacity, 1.0);
    
    if (m_upButton) {
        m_upButton->setOpacity(m_opacity);
    }
    if (m_downButton) {
        m_downButton->setOpacity(m_opacity);
    }
    
    update();
    emit opacityChanged(m_opacity);
}

ArrowButton* ScrollBarGroove::getUpButton() const {
    return m_upButton;
}

ArrowButton* ScrollBarGroove::getDownButton() const {
    return m_downButton;
}

QPropertyAnimation* ScrollBarGroove::getOpacityAnimation() const {
    return m_opacityAnimation;
}

qreal ScrollBarGroove::getOpacity() const {
    return m_opacity;
}

// ============================================================================
// ScrollBarHandle 实现
// ============================================================================

ScrollBarHandle::ScrollBarHandle(Qt::Orientation orient, QWidget* parent) 
    : QWidget(parent)
    , m_orientation(orient) 
{
    m_opacityAnimation = new QPropertyAnimation(this, "opacity", this);
    
    if (orient == Qt::Vertical) {
        setFixedWidth(3);
    } else {
        setFixedHeight(3);
    }
}

void ScrollBarHandle::setLightColor(const QColor& color) {
    m_lightColor = color;
    update();
}

void ScrollBarHandle::setDarkColor(const QColor& color) {
    m_darkColor = color;
    update();
}

void ScrollBarHandle::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    qreal radius = (m_orientation == Qt::Vertical ? width() : height()) / 2.0;
    painter.setOpacity(m_opacity);
    
    QColor brushColor = Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
    painter.setBrush(brushColor);
    painter.drawRoundedRect(rect(), radius, radius);
}

void ScrollBarHandle::fadeIn() {
    if (!m_opacityAnimation) return;
    
    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(m_opacity);
    m_opacityAnimation->setEndValue(1.0);
    m_opacityAnimation->setDuration(150);
    m_opacityAnimation->start();
}

void ScrollBarHandle::fadeOut() {
    if (!m_opacityAnimation) return;
    
    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(m_opacity);
    m_opacityAnimation->setEndValue(0.0);
    m_opacityAnimation->setDuration(150);
    m_opacityAnimation->start();
}

void ScrollBarHandle::setOpacity(qreal opacity) {
    m_opacity = qBound(0.0, opacity, 1.0);
    update();
}

qreal ScrollBarHandle::getOpacity() const {
    return m_opacity;
}

// ============================================================================
// ScrollBar 实现
// ============================================================================

ScrollBar::ScrollBar(Qt::Orientation orient, QAbstractScrollArea* parent) 
    : QWidget(parent)
    , m_orientation(orient)
{
    m_groove = new ScrollBarGroove(orient, this);
    m_handle = new ScrollBarHandle(orient, this);
    initWidget(parent);
}

void ScrollBar::initWidget(QAbstractScrollArea* parent) {
    if (!parent) return;
    
    // 连接槽按钮信号
    if (m_groove) {
        if (ArrowButton* upBtn = m_groove->getUpButton()) {
            connect(upBtn, &QToolButton::clicked, this, &ScrollBar::onPageUp);
        }
        if (ArrowButton* downBtn = m_groove->getDownButton()) {
            connect(downBtn, &QToolButton::clicked, this, &ScrollBar::onPageDown);
        }
        if (QPropertyAnimation* ani = m_groove->getOpacityAnimation()) {
            connect(ani, &QPropertyAnimation::valueChanged, 
                   this, &ScrollBar::onOpacityAnimationValueChanged);
        }
    }

    // 设置伙伴滚动条
    if (m_orientation == Qt::Vertical) {
        m_partnerBar = parent->verticalScrollBar();
        parent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        m_partnerBar = parent->horizontalScrollBar();
        parent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    if (m_partnerBar) {
        connect(m_partnerBar, &QScrollBar::rangeChanged, 
                this, [this](int min, int max) { setRange(min, max); });
        connect(m_partnerBar, &QScrollBar::valueChanged, 
                this, &ScrollBar::onValueChanged);
        connect(this, &ScrollBar::valueChanged, 
                m_partnerBar, &QScrollBar::setValue);
                
        setRange(m_partnerBar->minimum(), m_partnerBar->maximum());
    }

    parent->installEventFilter(this);
    setVisible(maximum() > 0 && !m_isForceHidden);
    adjustPos(parent->size());
}

void ScrollBar::onPageUp() {
    setValue(value() - pageStep());
}

void ScrollBar::onPageDown() {
    setValue(value() + pageStep());
}

void ScrollBar::onValueChanged(int value) {
    setVal(value);
}

int ScrollBar::value() const {
    return m_value;
}

void ScrollBar::setVal(int value) {
    if (value == m_value) return;
    
    value = qBound(minimum(), value, maximum());
    m_value = value;
    
    emit valueChanged(value);
    adjustHandlePos();
}

void ScrollBar::setValue(int value) {
    setVal(value);
}

int ScrollBar::minimum() const {
    return m_minimum;
}

void ScrollBar::setMinimum(int min) {
    if (m_minimum == min) return;
    m_minimum = min;
    adjustHandleSize();
    adjustHandlePos();
}

int ScrollBar::maximum() const {
    return m_maximum;
}

void ScrollBar::setMaximum(int max) {
    if (m_maximum == max) return;
    m_maximum = max;
    setVisible(max > 0 && !m_isForceHidden);
    adjustHandleSize();
    adjustHandlePos();
}

void ScrollBar::setRange(int min, int max) {
    if (m_minimum == min && m_maximum == max) return;
    
    m_minimum = min;
    m_maximum = max;
    
    setVisible(max > 0 && !m_isForceHidden);
    adjustHandleSize();
    adjustHandlePos();
    
    emit rangeChanged(min, max);
}

Qt::Orientation ScrollBar::orientation() const {
    return m_orientation;
}

int ScrollBar::pageStep() const {
    return m_pageStep;
}

void ScrollBar::setPageStep(int step) {
    m_pageStep = qMax(1, step);
}

int ScrollBar::singleStep() const {
    return m_singleStep;
}

void ScrollBar::setSingleStep(int step) {
    m_singleStep = qMax(1, step);
}

bool ScrollBar::isSliderDown() const {
    return m_isPressed;
}

void ScrollBar::setSliderDown(bool isDown) {
    m_isPressed = isDown;
}

void ScrollBar::setHandleColor(const QColor& light, const QColor& dark) {
    if (m_handle) {
        m_handle->setLightColor(light);
        m_handle->setDarkColor(dark);
    }
}

void ScrollBar::setArrowColor(const QColor& light, const QColor& dark) {
    if (!m_groove) return;
    
    if (ArrowButton* upBtn = m_groove->getUpButton()) {
        upBtn->setLightColor(light);
        upBtn->setDarkColor(dark);
    }
    if (ArrowButton* downBtn = m_groove->getDownButton()) {
        downBtn->setLightColor(light);
        downBtn->setDarkColor(dark);
    }
}

void ScrollBar::setGrooveColor(const QColor& light, const QColor& dark) {
    if (m_groove) {
        m_groove->setLightBackgroundColor(light);
        m_groove->setDarkBackgroundColor(dark);
    }
}

void ScrollBar::setHandleDisplayMode(Fluent::ScrollBarHandleDisplayMode mode) {
    m_handleDisplayMode = mode;
}

void ScrollBar::expand() {
    if (m_isExpanded || !m_groove) return;
    
    m_isExpanded = true;
    m_groove->fadeIn();
}

void ScrollBar::collapse() {
    if (!m_isExpanded || !m_groove) return;
    
    m_isExpanded = false;
    m_groove->fadeOut();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ScrollBar::enterEvent(QEnterEvent *event) {
#else
void ScrollBar::enterEvent(QEvent *event) {
#endif
    m_isEnter = true;
    expand();
    QWidget::enterEvent(event);
}

void ScrollBar::leaveEvent(QEvent* event) {
    m_isEnter = false;
    collapse();
    QWidget::leaveEvent(event);
}

bool ScrollBar::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Resize) {
        QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(obj);
        if (scrollArea) {
            adjustPos(scrollArea->size());
        }
    }
    return QWidget::eventFilter(obj, event);
}

void ScrollBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    
    if (m_groove) {
        m_groove->resize(size());
    }
    
    adjustHandleSize();
    adjustHandlePos();
}

void ScrollBar::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    
    if (!m_handle || event->button() != Qt::LeftButton) return;

    m_isPressed = true;
    m_pressedPos = event->pos();

    if (m_handle->geometry().contains(event->pos())) {
        emit sliderPressed();
        return;
    }

    // 点击槽区域时跳转
    if (isSlideRegion(event->pos())) {
        int valuePos = 0;
        
        if (orientation() == Qt::Vertical) {
            valuePos = event->pos().y() - m_padding;
        } else {
            valuePos = event->pos().x() - m_padding;
        }
        
        int newValue = static_cast<int>(
            static_cast<qreal>(valuePos) / qMax(slideLength(), 1) * maximum()
        );
        setValue(newValue);
        emit sliderPressed();
    }
}

void ScrollBar::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    m_isPressed = false;
    emit sliderReleased();
}

void ScrollBar::mouseMoveEvent(QMouseEvent* event) {
    if (!m_isPressed) return;

    int delta = (orientation() == Qt::Vertical) 
               ? (event->pos().y() - m_pressedPos.y())
               : (event->pos().x() - m_pressedPos.x());
               
    int slideLen = slideLength();
    if (slideLen <= 0) return;
    
    int deltaValue = static_cast<int>(
        static_cast<qreal>(delta) / slideLen * (maximum() - minimum())
    );
    
    setValue(value() + deltaValue);
    m_pressedPos = event->pos();
    emit sliderMoved();
}

void ScrollBar::adjustPos(const QSize& size) {
    if (orientation() == Qt::Vertical) {
        resize(12, size.height() - 2);
        move(size.width() - 13, 1);
    } else {
        resize(size.width() - 2, 12);
        move(1, size.height() - 13);
    }
}

void ScrollBar::adjustHandleSize() {
    QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(parent());
    if (!scrollArea || !m_handle) return;

    int total = 0;
    int size = 0;
    
    if (orientation() == Qt::Vertical) {
        total = maximum() - minimum() + scrollArea->height();
        size = static_cast<int>(grooveLength() * scrollArea->height() / qMax(total, 1));
        m_handle->setFixedHeight(qMax(30, size));
    } else {
        total = maximum() - minimum() + scrollArea->width();
        size = static_cast<int>(grooveLength() * scrollArea->width() / qMax(total, 1));
        m_handle->setFixedWidth(qMax(30, size));
    }
}

void ScrollBar::adjustHandlePos() {
    if (!m_handle) return;
    
    int totalRange = qMax(maximum() - minimum(), 1);
    int delta = static_cast<int>(
        value() / static_cast<qreal>(totalRange) * slideLength()
    );
    
    if (orientation() == Qt::Vertical) {
        int x = width() - m_handle->width() - 3;
        m_handle->move(x, m_padding + delta);
    } else {
        int y = height() - m_handle->height() - 3;
        m_handle->move(m_padding + delta, y);
    }
}

int ScrollBar::grooveLength() const {
    return (orientation() == Qt::Vertical ? height() : width()) - 2 * m_padding;
}

int ScrollBar::slideLength() const {
    if (!m_handle) return 0;
    
    int handleSize = (orientation() == Qt::Vertical) 
                    ? m_handle->height() 
                    : m_handle->width();
    return grooveLength() - handleSize;
}

bool ScrollBar::isSlideRegion(const QPoint& pos) const {
    if (orientation() == Qt::Vertical) {
        return m_padding <= pos.y() && pos.y() <= height() - m_padding;
    } else {
        return m_padding <= pos.x() && pos.x() <= width() - m_padding;
    }
}

void ScrollBar::onOpacityAnimationValueChanged(const QVariant& value) {
    Q_UNUSED(value)
    
    if (!m_groove || !m_handle) return;
    
    qreal opacity = m_groove->getOpacity();
    
    if (orientation() == Qt::Vertical) {
        m_handle->setFixedWidth(static_cast<int>(3 + opacity * 3));
    } else {
        m_handle->setFixedHeight(static_cast<int>(3 + opacity * 3));
    }
    
    adjustHandlePos();
}

void ScrollBar::setForceHidden(bool isHidden) {
    m_isForceHidden = isHidden;
    setVisible(maximum() > 0 && !isHidden);
}

void ScrollBar::wheelEvent(QWheelEvent* event) {
    QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(parent());
    if (scrollArea && scrollArea->viewport()) {
        QApplication::sendEvent(scrollArea->viewport(), event);
    }
}

// ============================================================================
// SmoothScrollBar 实现
// ============================================================================

SmoothScrollBar::SmoothScrollBar(Qt::Orientation orient, QAbstractScrollArea* parent) 
    : ScrollBar(orient, parent) 
{
    m_animation = new QPropertyAnimation(this, "val", this);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    m_animation->setDuration(m_duration);
    m_valueInternal = value();
}

void SmoothScrollBar::setValue(int value, bool useAnimation) {
    if (value == ScrollBar::value()) return;
    
    if (m_animation) {
        m_animation->stop();
    }
    
    if (!useAnimation) {
        ScrollBar::setVal(value);
        return;
    }

    if (!m_animation) return;
    
    int deltaValue = qAbs(value - ScrollBar::value());
    if (deltaValue < 50) {
        m_animation->setDuration(static_cast<int>(m_duration * deltaValue / 70.0));
    } else {
        m_animation->setDuration(m_duration);
    }

    m_animation->setStartValue(ScrollBar::value());
    m_animation->setEndValue(value);
    m_animation->start();
}

void SmoothScrollBar::scrollValue(int value, bool useAnimation) {
    m_valueInternal += value;
    m_valueInternal = qBound(minimum(), m_valueInternal, maximum());
    setValue(m_valueInternal, useAnimation);
}

void SmoothScrollBar::scrollTo(int value, bool useAnimation) {
    m_valueInternal = qBound(minimum(), value, maximum());
    setValue(m_valueInternal, useAnimation);
}

void SmoothScrollBar::resetValue(int value) {
    m_valueInternal = value;
}

void SmoothScrollBar::mousePressEvent(QMouseEvent* event) {
    if (m_animation) {
        m_animation->stop();
    }
    
    ScrollBar::mousePressEvent(event);
    m_valueInternal = ScrollBar::value();
}

void SmoothScrollBar::mouseMoveEvent(QMouseEvent* event) {
    if (m_animation) {
        m_animation->stop();
    }
    
    ScrollBar::mouseMoveEvent(event);
    m_valueInternal = ScrollBar::value();
}

void SmoothScrollBar::setScrollAnimation(int duration, QEasingCurve::Type easing) {
    m_duration = duration;
    
    if (m_animation) {
        m_animation->setDuration(duration);
        m_animation->setEasingCurve(easing);
    }
}

// ============================================================================
// SmoothScrollDelegate 实现
// ============================================================================

SmoothScrollDelegate::SmoothScrollDelegate(QAbstractScrollArea* parent, bool useAnimation)
    : QObject(parent)
    , m_useAnimation(useAnimation)
{
    if (!parent) return;

    m_verticalScrollBar = new SmoothScrollBar(Qt::Vertical, parent);
    m_horizontalScrollBar = new SmoothScrollBar(Qt::Horizontal, parent);
    m_verticalSmoothScroll = new SmoothScroll(parent, Qt::Vertical);
    m_horizontalSmoothScroll = new SmoothScroll(parent, Qt::Horizontal);

    // 设置像素级滚动
    if (QAbstractItemView* view = qobject_cast<QAbstractItemView*>(parent)) {
        view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    }

    // 特殊处理 QListView
    if (QListView* listView = qobject_cast<QListView*>(parent)) {
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        if (QScrollBar* hBar = listView->horizontalScrollBar()) {
            hBar->setStyleSheet("QScrollBar:horizontal{height: 0px}");
        }
    }

    if (parent->viewport()) {
        parent->viewport()->installEventFilter(this);
    }
}

bool SmoothScrollDelegate::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() != QEvent::Wheel) {
        return QObject::eventFilter(obj, event);
    }

    QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);

    if (!m_verticalScrollBar || !m_horizontalScrollBar) {
        return false;
    }

    // 检查是否已到达边界
    const bool verticalAtEnd =
        (wheelEvent->angleDelta().y() < 0 &&
         m_verticalScrollBar->value() == m_verticalScrollBar->maximum()) ||
        (wheelEvent->angleDelta().y() > 0 &&
         m_verticalScrollBar->value() == m_verticalScrollBar->minimum());

    const bool horizontalAtEnd =
        (wheelEvent->angleDelta().x() < 0 &&
         m_horizontalScrollBar->value() == m_horizontalScrollBar->maximum()) ||
        (wheelEvent->angleDelta().x() > 0 &&
         m_horizontalScrollBar->value() == m_horizontalScrollBar->minimum());

    if (verticalAtEnd || horizontalAtEnd) {
        return false;
    }

    // 处理滚动
    if (wheelEvent->angleDelta().y() != 0) {
        if (!m_useAnimation && m_verticalSmoothScroll) {
            m_verticalSmoothScroll->wheelEvent(wheelEvent);
        } else if (m_verticalScrollBar) {
            m_verticalScrollBar->scrollValue(-wheelEvent->angleDelta().y());
        }
    } else if (wheelEvent->angleDelta().x() != 0) {
        if (!m_useAnimation && m_horizontalSmoothScroll) {
            m_horizontalSmoothScroll->wheelEvent(wheelEvent);
        } else if (m_horizontalScrollBar) {
            m_horizontalScrollBar->scrollValue(-wheelEvent->angleDelta().x());
        }
    }

    event->accept();
    return true;
}

SmoothScroll* SmoothScrollDelegate::verticalSmoothScroll() const {
    return m_verticalSmoothScroll;
}

SmoothScroll* SmoothScrollDelegate::horizontalSmoothScroll() const {
    return m_horizontalSmoothScroll;
}

SmoothScrollBar* SmoothScrollDelegate::verticalScrollBar() const {
    return m_verticalScrollBar;
}

SmoothScrollBar* SmoothScrollDelegate::horizontalScrollBar() const {
    return m_horizontalScrollBar;
}

void SmoothScrollDelegate::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(parent());
    if (scrollArea) {
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    if (m_verticalScrollBar) {
        m_verticalScrollBar->setForceHidden(policy == Qt::ScrollBarAlwaysOff);
    }
}

void SmoothScrollDelegate::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(parent());
    if (scrollArea) {
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    if (m_horizontalScrollBar) {
        m_horizontalScrollBar->setForceHidden(policy == Qt::ScrollBarAlwaysOff);
    }
}
