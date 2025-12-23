#include "ScrollBar.h"
#include "SmoothScroll.h"
#include "FluentIcon.h"
#include "Theme.h"

#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QPropertyAnimation>
#include <QTimer>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QAbstractItemView>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>

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
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    const QColor color = Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
    painter.setOpacity(m_opacity * color.alphaF());

    const int iconSize = isDown() ? 7 : 8;
    const qreal x = (width() - iconSize) / 2.0;

    QHash<QString, QString> attributes;
    attributes["fill"] = color.name();

    if (m_fluentIcon) {
        FluentIconUtils::drawIcon(*m_fluentIcon, &painter,
                                  QRectF(x, x, iconSize, iconSize),
                                  Fluent::ThemeMode::AUTO,
                                  QIcon::Off,
                                  attributes);
    }
}

// ============================================================================
// ScrollBarGroove 实现
// ============================================================================

ScrollBarGroove::ScrollBarGroove(Qt::Orientation orientation, QWidget* parent)
    : QWidget(parent)
{
    m_opacityAnimation = new QPropertyAnimation(this, "opacity", this);

    if (orientation == Qt::Vertical) {
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
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);
    painter.setPen(Qt::NoPen);

    const QColor brushColor = Theme::instance()->isDarkTheme()
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

qreal ScrollBarGroove::opacity() const {
    return m_opacity;
}

ArrowButton* ScrollBarGroove::upButton() const {
    return m_upButton;
}

ArrowButton* ScrollBarGroove::downButton() const {
    return m_downButton;
}

QPropertyAnimation* ScrollBarGroove::opacityAnimation() const {
    return m_opacityAnimation;
}

// ============================================================================
// ScrollBarHandle 实现
// ============================================================================

ScrollBarHandle::ScrollBarHandle(Qt::Orientation orientation, QWidget* parent)
    : QWidget(parent)
    , m_orientation(orientation)
{
    m_opacityAnimation = new QPropertyAnimation(this, "opacity", this);

    if (orientation == Qt::Vertical) {
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
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    const qreal radius = (m_orientation == Qt::Vertical ? width() : height()) / 2.0;
    painter.setOpacity(m_opacity);

    const QColor brushColor = Theme::instance()->isDarkTheme() ? m_darkColor : m_lightColor;
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

qreal ScrollBarHandle::opacity() const {
    return m_opacity;
}

// ============================================================================
// ScrollBar 实现
// ============================================================================

ScrollBar::ScrollBar(Qt::Orientation orientation, QAbstractScrollArea* parent)
    : QWidget(parent)
    , m_orientation(orientation)
{
    m_groove = new ScrollBarGroove(orientation, this);
    m_handle = new ScrollBarHandle(orientation, this);
    initWidget(parent);
}

void ScrollBar::initWidget(QAbstractScrollArea* parent) {
    if (!parent) return;

    // 连接按钮信号
    if (m_groove && m_groove->upButton()) {
        connect(m_groove->upButton(), &QToolButton::clicked,
                this, &ScrollBar::onPageUp);
    }
    if (m_groove && m_groove->downButton()) {
        connect(m_groove->downButton(), &QToolButton::clicked,
                this, &ScrollBar::onPageDown);
    }
    if (m_groove && m_groove->opacityAnimation()) {
        connect(m_groove->opacityAnimation(), &QPropertyAnimation::valueChanged,
                this, &ScrollBar::onOpacityAnimationValueChanged);
    }

    // 设置伙伴滚动条
    if (m_orientation == Qt::Vertical) {
        m_partnerScrollBar = parent->verticalScrollBar();
        parent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        m_partnerScrollBar = parent->horizontalScrollBar();
        parent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    if (m_partnerScrollBar) {
        connect(m_partnerScrollBar, &QScrollBar::rangeChanged,
                this, [this](int min, int max) {
                    setRange(min, max);
                    emit rangeChanged(min, max);
                });
        connect(m_partnerScrollBar, &QScrollBar::valueChanged,
                this, &ScrollBar::onValueChanged);
        connect(this, &ScrollBar::valueChanged,
                m_partnerScrollBar, &QScrollBar::setValue);

        setRange(m_partnerScrollBar->minimum(), m_partnerScrollBar->maximum());
    }

    parent->installEventFilter(this);

    setVisible(maximum() > 0 && !m_isForceHidden);
    adjustPosition(parent->size());
}

void ScrollBar::onPageUp() {
    setValue(value() - pageStep());
}

void ScrollBar::onPageDown() {
    setValue(value() + pageStep());
}

void ScrollBar::onValueChanged(int value) {
    setValue(value);
}

int ScrollBar::value() const {
    return m_value;
}

int ScrollBar::minimum() const {
    return m_minimum;
}

int ScrollBar::maximum() const {
    return m_maximum;
}

Qt::Orientation ScrollBar::orientation() const {
    return m_orientation;
}

int ScrollBar::pageStep() const {
    return m_pageStep;
}

int ScrollBar::singleStep() const {
    return m_singleStep;
}

bool ScrollBar::isSliderDown() const {
    return m_isPressed;
}

void ScrollBar::setValue(int value) {
    value = qBound(m_minimum, value, m_maximum);
    if (value == m_value) return;

    m_value = value;
    adjustHandlePosition();
    emit valueChanged(m_value);
}

void ScrollBar::setMinimum(int min) {
    if (min == m_minimum) return;
    m_minimum = min;
    setValue(m_value);
}

void ScrollBar::setMaximum(int max) {
    if (max == m_maximum) return;
    m_maximum = max;
    setValue(m_value);
}

void ScrollBar::setRange(int min, int max) {
    if (min == m_minimum && max == m_maximum) return;

    m_minimum = min;
    m_maximum = max;
    setValue(m_value);

    setVisible(m_maximum > 0 && !m_isForceHidden);
    adjustHandleSize();
    adjustHandlePosition();
}

void ScrollBar::setPageStep(int step) {
    m_pageStep = qMax(1, step);
}

void ScrollBar::setSingleStep(int step) {
    m_singleStep = qMax(1, step);
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
    if (m_groove && m_groove->upButton()) {
        m_groove->upButton()->setLightColor(light);
        m_groove->upButton()->setDarkColor(dark);
    }
    if (m_groove && m_groove->downButton()) {
        m_groove->downButton()->setLightColor(light);
        m_groove->downButton()->setDarkColor(dark);
    }
}

void ScrollBar::setGrooveColor(const QColor& light, const QColor& dark) {
    if (m_groove) {
        m_groove->setLightBackgroundColor(light);
        m_groove->setDarkBackgroundColor(dark);
    }
}

void ScrollBar::setHandleDisplayMode(Fluent::ScrollBarHandleDisplayMode mode) {
    if (mode == m_handleDisplayMode) return;

    m_handleDisplayMode = mode;

    if (!m_handle) return;

    if (mode == Fluent::ScrollBarHandleDisplayMode::ON_HOVER && !m_isEnter) {
        m_handle->fadeOut();
    } else if (mode == Fluent::ScrollBarHandleDisplayMode::ALWAYS) {
        m_handle->fadeIn();
    }
}

void ScrollBar::expand() {
    if (m_isExpanded || !m_isEnter || !m_groove) return;

    m_isExpanded = true;
    m_groove->fadeIn();
    if (m_handle) {
        m_handle->fadeIn();
    }
}

void ScrollBar::collapse() {
    if (!m_isExpanded || m_isEnter || !m_groove) return;

    m_isExpanded = false;
    m_groove->fadeOut();
    if (m_handle && m_handleDisplayMode == Fluent::ScrollBarHandleDisplayMode::ON_HOVER) {
        m_handle->fadeOut();
    }
}

void ScrollBar::setForceHidden(bool isHidden) {
    m_isForceHidden = isHidden;
    setVisible(maximum() > 0 && !isHidden);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ScrollBar::enterEvent(QEnterEvent *event) {
#else
void ScrollBar::enterEvent(QEvent *event) {
#endif
    Q_UNUSED(event);
    m_isEnter = true;
    QTimer::singleShot(200, this, &ScrollBar::expand);
}

void ScrollBar::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    m_isEnter = false;
    QTimer::singleShot(200, this, &ScrollBar::collapse);
}

bool ScrollBar::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::Resize) {
        QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(obj);
        if (scrollArea) {
            adjustPosition(scrollArea->size());
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
    adjustHandlePosition();
}

void ScrollBar::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);

    m_isPressed = true;
    m_pressedPos = event->pos();

    if (!m_handle) return;

    // 如果点击的是手柄或不在滑动区域，直接返回
    if (childAt(event->pos()) == m_handle || !isSlideRegion(event->pos())) {
        return;
    }

    // 计算新的值位置
    int valuePos = 0;
    if (orientation() == Qt::Vertical) {
        if (event->pos().y() > m_handle->geometry().bottom()) {
            valuePos = event->pos().y() - m_handle->height() - m_padding;
        } else {
            valuePos = event->pos().y() - m_padding;
        }
    } else {
        if (event->pos().x() > m_handle->geometry().right()) {
            valuePos = event->pos().x() - m_handle->width() - m_padding;
        } else {
            valuePos = event->pos().x() - m_padding;
        }
    }

    const int slideLengthValue = qMax(slideLength(), 1);
    setValue(static_cast<int>(static_cast<qreal>(valuePos) / slideLengthValue * maximum()));
    emit sliderPressed();
}

void ScrollBar::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    m_isPressed = false;
    emit sliderReleased();
}

void ScrollBar::mouseMoveEvent(QMouseEvent* event) {
    if (!m_isPressed) return;

    const int deltaPosition = (orientation() == Qt::Vertical)
                              ? event->pos().y() - m_pressedPos.y()
                              : event->pos().x() - m_pressedPos.x();

    const int slideLengthValue = qMax(slideLength(), 1);
    const int deltaValue = static_cast<int>(
        static_cast<qreal>(deltaPosition) / slideLengthValue * (maximum() - minimum())
    );

    setValue(value() + deltaValue);
    m_pressedPos = event->pos();
    emit sliderMoved();
}

void ScrollBar::wheelEvent(QWheelEvent* event) {
    QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(parent());
    if (scrollArea && scrollArea->viewport()) {
        QApplication::sendEvent(scrollArea->viewport(), event);
    }
}

void ScrollBar::adjustPosition(const QSize& size) {
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
    int handleSize = 0;

    if (orientation() == Qt::Vertical) {
        total = maximum() - minimum() + scrollArea->height();
        handleSize = static_cast<int>(
            grooveLength() * scrollArea->height() / qMax(total, 1)
        );
        m_handle->setFixedHeight(qMax(30, handleSize));
    } else {
        total = maximum() - minimum() + scrollArea->width();
        handleSize = static_cast<int>(
            grooveLength() * scrollArea->width() / qMax(total, 1)
        );
        m_handle->setFixedWidth(qMax(30, handleSize));
    }
}

void ScrollBar::adjustHandlePosition() {
    if (!m_handle) return;

    const int totalRange = qMax(maximum() - minimum(), 1);
    const int delta = static_cast<int>(
        static_cast<qreal>(value()) / totalRange * slideLength()
    );

    if (orientation() == Qt::Vertical) {
        const int xPos = width() - m_handle->width() - 3;
        m_handle->move(xPos, m_padding + delta);
    } else {
        const int yPos = height() - m_handle->height() - 3;
        m_handle->move(m_padding + delta, yPos);
    }
}

int ScrollBar::grooveLength() const {
    return (orientation() == Qt::Vertical ? height() : width()) - 2 * m_padding;
}

int ScrollBar::slideLength() const {
    if (!m_handle) return 0;
    return grooveLength() - (orientation() == Qt::Vertical
                             ? m_handle->height()
                             : m_handle->width());
}

bool ScrollBar::isSlideRegion(const QPoint& pos) const {
    if (orientation() == Qt::Vertical) {
        return m_padding <= pos.y() && pos.y() <= height() - m_padding;
    } else {
        return m_padding <= pos.x() && pos.x() <= width() - m_padding;
    }
}

void ScrollBar::onOpacityAnimationValueChanged(const QVariant& value) {
    Q_UNUSED(value);

    if (!m_groove || !m_handle) return;

    const qreal opacity = m_groove->opacity();
    if (orientation() == Qt::Vertical) {
        m_handle->setFixedWidth(static_cast<int>(3 + opacity * 3));
    } else {
        m_handle->setFixedHeight(static_cast<int>(3 + opacity * 3));
    }
    adjustHandlePosition();
}

// ============================================================================
// SmoothScrollBar 实现
// ============================================================================

SmoothScrollBar::SmoothScrollBar(Qt::Orientation orientation, QAbstractScrollArea* parent)
    : ScrollBar(orientation, parent)
{
    m_animation = new QPropertyAnimation(this, "value", this);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    m_animation->setDuration(m_animationDuration);
    m_internalValue = value();
}

void SmoothScrollBar::setValue(int value, bool useAnimation) {
    if (value == ScrollBar::value()) return;

    if (!m_animation) {
        ScrollBar::setValue(value);
        return;
    }

    m_animation->stop();

    if (!useAnimation) {
        ScrollBar::setValue(value);
        return;
    }

    const int deltaValue = qAbs(value - ScrollBar::value());
    if (deltaValue < 50) {
        m_animation->setDuration(
            static_cast<int>(m_animationDuration * deltaValue / 70.0)
        );
    } else {
        m_animation->setDuration(m_animationDuration);
    }

    m_animation->setStartValue(ScrollBar::value());
    m_animation->setEndValue(value);
    m_animation->start();
}

void SmoothScrollBar::scrollValue(int value, bool useAnimation) {
    m_internalValue += value;
    m_internalValue = qBound(minimum(), m_internalValue, maximum());
    setValue(m_internalValue, useAnimation);
}

void SmoothScrollBar::scrollTo(int value, bool useAnimation) {
    m_internalValue = qBound(minimum(), value, maximum());
    setValue(m_internalValue, useAnimation);
}

void SmoothScrollBar::resetValue(int value) {
    m_internalValue = value;
}

void SmoothScrollBar::mousePressEvent(QMouseEvent* event) {
    if (m_animation) {
        m_animation->stop();
    }
    ScrollBar::mousePressEvent(event);
    m_internalValue = ScrollBar::value();
}

void SmoothScrollBar::mouseMoveEvent(QMouseEvent* event) {
    if (m_animation) {
        m_animation->stop();
    }
    ScrollBar::mouseMoveEvent(event);
    m_internalValue = ScrollBar::value();
}

void SmoothScrollBar::setScrollAnimation(int duration, QEasingCurve::Type easing) {
    m_animationDuration = qMax(0, duration);
    if (m_animation) {
        m_animation->setDuration(m_animationDuration);
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
        listView->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{height: 0px}");
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
