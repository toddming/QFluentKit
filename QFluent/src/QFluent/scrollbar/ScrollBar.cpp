#include "scrollbar.h"
#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QPropertyAnimation>
#include <QTimer>
#include <QEvent>
#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include <QRectF>
#include <QPoint>
#include <QObject>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QAbstractItemView>
#include <QListView>

#include "Theme.h"

ArrowButton::ArrowButton(const FluentIconBase &icon, QWidget* parent)
    : QToolButton(parent)
    , m_fluentIcon(icon.clone())
{
    setFixedSize(10, 10);
}

void ArrowButton::setOpacity(qreal opacity) {
    m_opacity = opacity;
    update();
}

void ArrowButton::setLightColor(const QColor& color) {
    lightColor = color;
    update();
}

void ArrowButton::setDarkColor(const QColor& color) {
    darkColor = color;
    update();
}

void ArrowButton::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QColor color = Theme::instance()->isDarkTheme() ? darkColor : lightColor;
    painter.setOpacity(m_opacity * color.alphaF());

    int s = isDown() ? 7 : 8;
    qreal x = (width() - s) / 2.0;

    QMap<QString, QString> attrs;
    attrs["fill"] = color.name();
    FluentIconUtils::drawIcon(*m_fluentIcon, &painter, QRectF(x, x, s, s), Fluent::ThemeMode::AUTO,  QIcon::Off, attrs);

}

ScrollBarGroove::ScrollBarGroove(Qt::Orientation orient, QWidget* parent) : QWidget(parent) {
    opacityAni = new QPropertyAnimation(this, "opacity", this);

    if (orient == Qt::Vertical) {
        setFixedWidth(12);
        upButton = new ArrowButton(FluentIcon(Fluent::IconType::CARE_UP_SOLID), this);
        downButton = new ArrowButton(FluentIcon(Fluent::IconType::CARE_DOWN_SOLID), this);
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(upButton, 0, Qt::AlignHCenter);
        layout->addStretch(1);
        layout->addWidget(downButton, 0, Qt::AlignHCenter);
        layout->setContentsMargins(0, 3, 0, 3);
    } else {
        setFixedHeight(12);
        upButton = new ArrowButton(FluentIcon(Fluent::IconType::CARE_LEFT_SOLID), this);
        downButton = new ArrowButton(FluentIcon(Fluent::IconType::CARE_RIGHT_SOLID), this);
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->addWidget(upButton, 0, Qt::AlignVCenter);
        layout->addStretch(1);
        layout->addWidget(downButton, 0, Qt::AlignVCenter);
        layout->setContentsMargins(3, 0, 3, 0);
    }

    setOpacity(0.0);
    // 如果需要连接opacityAni->valueChanged到onOpacityAniValueChanged
}

void ScrollBarGroove::setLightBackgroundColor(const QColor& color) {
    lightBackgroundColor = color;
    update();
}

void ScrollBarGroove::setDarkBackgroundColor(const QColor& color) {
    darkBackgroundColor = color;
    update();
}

void ScrollBarGroove::fadeIn() {
    opacityAni->stop();
    opacityAni->setStartValue(m_opacity);
    opacityAni->setEndValue(1.0);
    opacityAni->setDuration(150);
    opacityAni->start();
}

void ScrollBarGroove::fadeOut() {
    opacityAni->stop();
    opacityAni->setStartValue(m_opacity);
    opacityAni->setEndValue(0.0);
    opacityAni->setDuration(150);
    opacityAni->start();
}

void ScrollBarGroove::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);
    painter.setPen(Qt::NoPen);

    QColor brushColor = Theme::instance()->isDarkTheme() ? darkBackgroundColor : lightBackgroundColor;
    painter.setBrush(brushColor);
    painter.drawRoundedRect(rect(), 6, 6);
}

void ScrollBarGroove::setOpacity(qreal opacity) {
    m_opacity = opacity;
    upButton->setOpacity(opacity);
    downButton->setOpacity(opacity);
    update();
}

ArrowButton* ScrollBarGroove::getUpButton()
{
    return upButton;
}

ArrowButton* ScrollBarGroove::getDownButton()
{
    return downButton;
}

QPropertyAnimation* ScrollBarGroove::getOpacityAni()
{
    return opacityAni;
}

qreal ScrollBarGroove::getOpacity() const {
    return m_opacity;
}

void ScrollBarGroove::onOpacityAniValueChanged(const QVariant& value) {
    // 如果需要外部处理，这里实现
}

ScrollBarHandle::ScrollBarHandle(Qt::Orientation orient, QWidget* parent) : QWidget(parent), m_orient(orient) {
    opacityAni = new QPropertyAnimation(this, "opacity", this);
    if (orient == Qt::Vertical) {
        setFixedWidth(3);
    } else {
        setFixedHeight(3);
    }
}

void ScrollBarHandle::setLightColor(const QColor& color) {
    lightColor = color;
    update();
}

void ScrollBarHandle::setDarkColor(const QColor& color) {
    darkColor = color;
    update();
}

void ScrollBarHandle::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    qreal r = (m_orient == Qt::Vertical ? width() : height()) / 2.0;
    painter.setOpacity(m_opacity);
    QColor brushColor = Theme::instance()->isDarkTheme() ? darkColor : lightColor;
    painter.setBrush(brushColor);
    painter.drawRoundedRect(rect(), r, r);
}

void ScrollBarHandle::fadeIn() {
    opacityAni->stop();
    opacityAni->setStartValue(m_opacity);
    opacityAni->setEndValue(1.0);
    opacityAni->setDuration(150);
    opacityAni->start();
}

void ScrollBarHandle::fadeOut() {
    opacityAni->stop();
    opacityAni->setStartValue(m_opacity);
    opacityAni->setEndValue(0.0);
    opacityAni->setDuration(150);
    opacityAni->start();
}

void ScrollBarHandle::setOpacity(qreal opacity) {
    m_opacity = opacity;
    update();
}

qreal ScrollBarHandle::getOpacity() const {
    return m_opacity;
}

ScrollBar::ScrollBar(Qt::Orientation orient, QAbstractScrollArea* parent) : QWidget(parent),
    m_orientation(orient), partnerBar(nullptr) {
    groove = new ScrollBarGroove(orient, this);
    handle = new ScrollBarHandle(orient, this);
    initWidget(parent);
}

void ScrollBar::initWidget(QAbstractScrollArea* parent) {
    connect(groove->getUpButton(), &QToolButton::clicked, this, &ScrollBar::onPageUp);
    connect(groove->getDownButton(), &QToolButton::clicked, this, &ScrollBar::onPageDown);
    connect(groove->getOpacityAni(), &QPropertyAnimation::valueChanged, this, &ScrollBar::onOpacityAniValueChanged);

    if (m_orientation == Qt::Vertical) {
        partnerBar = parent->verticalScrollBar();
        parent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        partnerBar = parent->horizontalScrollBar();
        parent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    connect(partnerBar, &QScrollBar::rangeChanged, [this](int min, int max) { setRange(min, max); });
    connect(partnerBar, &QScrollBar::valueChanged, this, &ScrollBar::onValueChanged);
    connect(this, &ScrollBar::valueChanged, partnerBar, &QScrollBar::setValue);

    parent->installEventFilter(this);

    setRange(partnerBar->minimum(), partnerBar->maximum());
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
    value = qMax(minimum(), qMin(value, maximum()));
    m_value = value;
    emit valueChanged(value);
    adjustHandlePos();
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
    setVal(value);
}

void ScrollBar::setMinimum(int min) {
    if (min == m_minimum) return;
    m_minimum = min;
    emit rangeChanged(qMakePair(min, maximum()));
}

void ScrollBar::setMaximum(int max) {
    if (max == m_maximum) return;
    m_maximum = max;
    emit rangeChanged(qMakePair(minimum(), max));
}

void ScrollBar::setRange(int min, int max) {
    if (min > max || (min == minimum() && max == maximum())) return;
    setMinimum(min);
    setMaximum(max);
    adjustHandleSize();
    adjustHandlePos();
    setVisible(max > 0 && !m_isForceHidden);
    emit rangeChanged(qMakePair(min, max));
}

void ScrollBar::setPageStep(int step) {
    if (step >= 1) m_pageStep = step;
}

void ScrollBar::setSingleStep(int step) {
    if (step >= 1) m_singleStep = step;
}

void ScrollBar::setSliderDown(bool isDown) {
    m_isPressed = isDown;
    if (isDown) emit sliderPressed();
    else emit sliderReleased();
}

void ScrollBar::setHandleColor(const QColor& light, const QColor& dark) {
    handle->setLightColor(light);
    handle->setDarkColor(dark);
}

void ScrollBar::setArrowColor(const QColor& light, const QColor& dark) {
    groove->getUpButton()->setLightColor(light);
    groove->getUpButton()->setDarkColor(dark);
    groove->getDownButton()->setLightColor(light);
    groove->getDownButton()->setDarkColor(dark);
}

void ScrollBar::setGrooveColor(const QColor& light, const QColor& dark) {
    groove->setLightBackgroundColor(light);
    groove->setDarkBackgroundColor(dark);
}

void ScrollBar::setHandleDisplayMode(Fluent::ScrollBarHandleDisplayMode mode) {
    if (mode == handleDisplayMode) return;
    handleDisplayMode = mode;
    if (mode == Fluent::ScrollBarHandleDisplayMode::ON_HOVER && !m_isEnter) {
        handle->fadeOut();
    } else if (mode == Fluent::ScrollBarHandleDisplayMode::ALWAYS) {
        handle->fadeIn();
    }
}

void ScrollBar::expand() {
    if (m_isExpanded || !m_isEnter) return;
    m_isExpanded = true;
    groove->fadeIn();
    handle->fadeIn();
}

void ScrollBar::collapse() {
    if (!m_isExpanded || m_isEnter) return;
    m_isExpanded = false;
    groove->fadeOut();
    if (handleDisplayMode == Fluent::ScrollBarHandleDisplayMode::ON_HOVER) {
        handle->fadeOut();
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ScrollBar::enterEvent(QEnterEvent *e) {
    m_isEnter = true;
    QTimer::singleShot(200, this, &ScrollBar::expand);
}
#else
void ScrollBar::enterEvent(QEvent *e) {
    m_isEnter = true;
    QTimer::singleShot(200, this, &ScrollBar::expand);
}
#endif

void ScrollBar::leaveEvent(QEvent* e) {
    m_isEnter = false;
    QTimer::singleShot(200, this, &ScrollBar::collapse);
}

bool ScrollBar::eventFilter(QObject* obj, QEvent* e) {
    if (obj != parent()) return QWidget::eventFilter(obj, e);
    if (e->type() == QEvent::Resize) {
        QResizeEvent* re = static_cast<QResizeEvent*>(e);
        adjustPos(re->size());
    }
    return QWidget::eventFilter(obj, e);
}

void ScrollBar::resizeEvent(QResizeEvent* e) {
    groove->resize(size());
}

void ScrollBar::mousePressEvent(QMouseEvent* e) {
    QWidget::mousePressEvent(e);
    m_isPressed = true;
    m_pressedPos = e->pos();
    if (childAt(e->pos()) == handle || !isSlideRegion(e->pos())) return;

    int valuePos;
    if (orientation() == Qt::Vertical) {
        if (e->pos().y() > handle->geometry().bottom()) {
            valuePos = e->pos().y() - handle->height() - m_padding;
        } else {
            valuePos = e->pos().y() - m_padding;
        }
    } else {
        if (e->pos().x() > handle->geometry().right()) {
            valuePos = e->pos().x() - handle->width() - m_padding;
        } else {
            valuePos = e->pos().x() - m_padding;
        }
    }

    setValue(double(valuePos) / qMax(slideLength(), 1) * maximum());
    emit sliderPressed();
}

void ScrollBar::mouseReleaseEvent(QMouseEvent* e) {
    QWidget::mouseReleaseEvent(e);
    m_isPressed = false;
    emit sliderReleased();
}

void ScrollBar::mouseMoveEvent(QMouseEvent* e) {
    int dv = (orientation() == Qt::Vertical ? e->pos().y() - m_pressedPos.y() : e->pos().x() - m_pressedPos.x());
    dv = double(dv) / qMax(slideLength(), 1) * (maximum() - minimum());
    setValue(value() + dv);
    m_pressedPos = e->pos();
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
    QAbstractScrollArea* p = qobject_cast<QAbstractScrollArea*>(parent());
    if (!p) return;

    int total, s;
    if (orientation() == Qt::Vertical) {
        total = maximum() - minimum() + p->height();
        s = static_cast<int>(grooveLength() * p->height() / qMax(total, 1));
        handle->setFixedHeight(qMax(30, s));
    } else {
        total = maximum() - minimum() + p->width();
        s = static_cast<int>(grooveLength() * p->width() / qMax(total, 1));
        handle->setFixedWidth(qMax(30, s));
    }
}

void ScrollBar::adjustHandlePos() {
    int total = qMax(maximum() - minimum(), 1);
    int delta = static_cast<int>(value() / static_cast<qreal>(total) * slideLength());
    if (orientation() == Qt::Vertical) {
        int x = width() - handle->width() - 3;
        handle->move(x, m_padding + delta);
    } else {
        int y = height() - handle->height() - 3;
        handle->move(m_padding + delta, y);
    }
}

int ScrollBar::grooveLength() const {
    return (orientation() == Qt::Vertical ? height() : width()) - 2 * m_padding;
}

int ScrollBar::slideLength() const {
    return grooveLength() - (orientation() == Qt::Vertical ? handle->height() : handle->width());
}

bool ScrollBar::isSlideRegion(const QPoint& pos) const {
    return (orientation() == Qt::Vertical ? (m_padding <= pos.y() && pos.y() <= height() - m_padding) :
                                            (m_padding <= pos.x() && pos.x() <= width() - m_padding));
}

void ScrollBar::onOpacityAniValueChanged(const QVariant& value) {
    qreal opacity = groove->getOpacity();
    if (orientation() == Qt::Vertical) {
        handle->setFixedWidth(static_cast<int>(3 + opacity * 3));
    } else {
        handle->setFixedHeight(static_cast<int>(3 + opacity * 3));
    }
    adjustHandlePos();
}

void ScrollBar::setForceHidden(bool isHidden) {
    m_isForceHidden = isHidden;
    setVisible(maximum() > 0 && !isHidden);
}

void ScrollBar::wheelEvent(QWheelEvent* e) {
    QApplication::sendEvent(qobject_cast<QAbstractScrollArea*>(parent())->viewport(), e);
}

SmoothScrollBar::SmoothScrollBar(Qt::Orientation orient, QAbstractScrollArea* parent) : ScrollBar(orient, parent) {
    ani = new QPropertyAnimation(this, "val", this);
    ani->setEasingCurve(QEasingCurve::OutCubic);
    ani->setDuration(duration);
    m_valueInternal = value();
}

void SmoothScrollBar::setValue(int value, bool useAni) {
    if (value == ScrollBar::value()) return;
    ani->stop();
    if (!useAni) {
        ScrollBar::setVal(value);
        return;
    }

    int dv = qAbs(value - ScrollBar::value());
    if (dv < 50) {
        ani->setDuration(static_cast<int>(duration * dv / 70.0));
    } else {
        ani->setDuration(duration);
    }

    ani->setStartValue(ScrollBar::value());
    ani->setEndValue(value);
    ani->start();
}

void SmoothScrollBar::scrollValue(int value, bool useAni) {
    m_valueInternal += value;
    m_valueInternal = qMax(minimum(), qMin(m_valueInternal, maximum()));
    setValue(m_valueInternal, useAni);
}

void SmoothScrollBar::scrollTo(int value, bool useAni) {
    m_valueInternal = value;
    m_valueInternal = qMax(minimum(), qMin(m_valueInternal, maximum()));
    setValue(m_valueInternal, useAni);
}

void SmoothScrollBar::resetValue(int value) {
    m_valueInternal = value;
}

void SmoothScrollBar::mousePressEvent(QMouseEvent* e) {
    ani->stop();
    ScrollBar::mousePressEvent(e);
    m_valueInternal = ScrollBar::value();
}

void SmoothScrollBar::mouseMoveEvent(QMouseEvent* e) {
    ani->stop();
    ScrollBar::mouseMoveEvent(e);
    m_valueInternal = ScrollBar::value();
}

void SmoothScrollBar::setScrollAnimation(int duration, QEasingCurve::Type easing) {
    this->duration = duration;
    ani->setDuration(duration);
    ani->setEasingCurve(easing);
}

SmoothScrollDelegate::SmoothScrollDelegate(QAbstractScrollArea* parent, bool useAni) : QObject(parent), useAni(useAni) {
    vScrollBar = new SmoothScrollBar(Qt::Vertical, parent);
    hScrollBar = new SmoothScrollBar(Qt::Horizontal, parent);
    verticalSmoothScroll = new SmoothScroll(parent, Qt::Vertical);
    horizonSmoothScroll = new SmoothScroll(parent, Qt::Horizontal);

    if (QAbstractItemView* view = qobject_cast<QAbstractItemView*>(parent)) {
        view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    }
    if (QListView* listView = qobject_cast<QListView*>(parent)) {
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        listView->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{height: 0px}");
    }

    parent->viewport()->installEventFilter(this);

    // 代理政策设置
    // 注意：在C++中，无法直接重载parent的方法，所以用户需手动调用这些代理方法
}

bool SmoothScrollDelegate::eventFilter(QObject* obj, QEvent* e) {
    if (e->type() == QEvent::Wheel) {
        QWheelEvent* we = static_cast<QWheelEvent*>(e);

        bool verticalAtEnd = (we->angleDelta().y() < 0 && vScrollBar->value() == vScrollBar->maximum()) ||
                (we->angleDelta().y() > 0 && vScrollBar->value() == vScrollBar->minimum());
        bool horizontalAtEnd = (we->angleDelta().x() < 0 && hScrollBar->value() == hScrollBar->maximum()) ||
                (we->angleDelta().x() > 0 && hScrollBar->value() == hScrollBar->minimum());

        if (verticalAtEnd || horizontalAtEnd) return false;

        if (we->angleDelta().y() != 0) {
            if (!useAni) {
                verticalSmoothScroll->wheelEvent(we);
            } else {
                vScrollBar->scrollValue(-we->angleDelta().y());
            }
        } else {
            if (!useAni) {
                horizonSmoothScroll->wheelEvent(we);
            } else {
                hScrollBar->scrollValue(-we->angleDelta().x());
            }
        }

        e->accept();
        return true;
    }
    return QObject::eventFilter(obj, e);
}

SmoothScroll* SmoothScrollDelegate::getVScroll()
{
    return verticalSmoothScroll;
}

SmoothScroll* SmoothScrollDelegate::getHScroll()
{
    return horizonSmoothScroll;
}

SmoothScrollBar* SmoothScrollDelegate::getVScrollBar()
{
    return vScrollBar;
}

SmoothScrollBar* SmoothScrollDelegate::getHScrollBar()
{
    return hScrollBar;
}

void SmoothScrollDelegate::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    QAbstractScrollArea* p = qobject_cast<QAbstractScrollArea*>(parent());
    p->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vScrollBar->setForceHidden(policy == Qt::ScrollBarAlwaysOff);
}

void SmoothScrollDelegate::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy) {
    QAbstractScrollArea* p = qobject_cast<QAbstractScrollArea*>(parent());
    p->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    hScrollBar->setForceHidden(policy == Qt::ScrollBarAlwaysOff);
}
