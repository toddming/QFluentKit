#include "Slider.h"
#include <QPropertyAnimation>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QStyleOptionSlider>
#include <QStyle>
#include <QEnterEvent>
#include <QRectF>

#include "Theme.h"

// ==================== SliderHandle 实现 ====================

SliderHandle::SliderHandle(QSlider *parent)
    : QWidget(parent)
    , m_radius(5.0)
    , m_lightHandleColor()
    , m_darkHandleColor()
    , m_radiusAnimation(nullptr)
{
    setFixedSize(22, 22);

    m_radiusAnimation = new QPropertyAnimation(this, "radius", this);
    m_radiusAnimation->setDuration(100);
}

SliderHandle::~SliderHandle()
{
    // QPropertyAnimation会被Qt的父子关系自动删除
}

qreal SliderHandle::radius() const
{
    return m_radius;
}

void SliderHandle::setRadius(qreal r)
{
    if (qFuzzyCompare(m_radius, r)) {
        return;
    }

    m_radius = r;
    update();
}

void SliderHandle::setHandleColor(const QColor &light, const QColor &dark)
{
    m_lightHandleColor = light;
    m_darkHandleColor = dark;
    update();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void SliderHandle::enterEvent(QEnterEvent *event) {
    Q_UNUSED(event);
    startAnimation(6.5);
}
#else
void SliderHandle::enterEvent(QEvent *event) {
    Q_UNUSED(event);
    startAnimation(6.5);
}
#endif

void SliderHandle::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    startAnimation(5.0);
}

void SliderHandle::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    startAnimation(4.0);
    emit pressed();
}

void SliderHandle::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    startAnimation(6.5);
    emit released();
}

void SliderHandle::startAnimation(qreal targetRadius)
{
    if (!m_radiusAnimation) {
        return;
    }

    m_radiusAnimation->stop();
    m_radiusAnimation->setStartValue(m_radius);
    m_radiusAnimation->setEndValue(targetRadius);
    m_radiusAnimation->start();
}

void SliderHandle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    bool isDark = Theme::instance()->isDarkTheme();

    // 绘制外圆
    painter.setPen(QColor(0, 0, 0, isDark ? 90 : 25));
    painter.setBrush(isDark ? QColor(69, 69, 69) : Qt::white);
    painter.drawEllipse(rect().adjusted(1, 1, -1, -1));

    // 绘制内圆
    QColor color = Theme::instance()->themeColor();
    m_darkHandleColor = m_darkHandleColor.isValid() ? m_darkHandleColor : color;
    m_lightHandleColor = m_lightHandleColor.isValid() ? m_lightHandleColor : color;
    painter.setBrush(isDark ? m_darkHandleColor : m_lightHandleColor);
    painter.drawEllipse(QPoint(11, 11), static_cast<int>(m_radius), static_cast<int>(m_radius));
}

// ==================== Slider 实现 ====================

Slider::Slider(QWidget *parent)
    : QSlider(parent)
    , m_handle(nullptr)
    , m_pressedPos()
    , m_lightGrooveColor()
    , m_darkGrooveColor()
{
    init();
}

Slider::Slider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent)
    , m_handle(nullptr)
    , m_pressedPos()
    , m_lightGrooveColor()
    , m_darkGrooveColor()
{
    init();
}

Slider::~Slider()
{
    // m_handle会被Qt的父子关系自动删除
}

void Slider::init()
{
    m_handle = new SliderHandle(this);
    setOrientation(orientation());

    connect(m_handle, &SliderHandle::pressed, this, &Slider::sliderPressed);
    connect(m_handle, &SliderHandle::released, this, &Slider::sliderReleased);
    connect(this, &Slider::valueChanged, this, &Slider::adjustHandlePosition);
}

void Slider::setThemeColor(const QColor &light, const QColor &dark)
{
    m_lightGrooveColor = light;
    m_darkGrooveColor = dark;

    if (m_handle) {
        m_handle->setHandleColor(light, dark);
    }

    update();
}

void Slider::setOrientation(Qt::Orientation orientation)
{
    QSlider::setOrientation(orientation);

    if (orientation == Qt::Horizontal) {
        setMinimumHeight(22);
    } else {
        setMinimumWidth(22);
    }
}

int Slider::grooveLength() const
{
    int length = (orientation() == Qt::Horizontal) ? width() : height();
    return length - (m_handle ? m_handle->width() : 0);
}

void Slider::adjustHandlePosition()
{
    if (!m_handle) {
        return;
    }

    int total = qMax(maximum() - minimum(), 1);
    int delta = static_cast<int>((value() - minimum()) * grooveLength() / static_cast<qreal>(total));

    if (orientation() == Qt::Vertical) {
        m_handle->move(0, delta);
    } else {
        m_handle->move(delta, 0);
    }
}

int Slider::posToValue(const QPoint &pos) const
{
    if (!m_handle) {
        return minimum();
    }

    qreal halfWidth = m_handle->width() / 2.0;
    int grooveSize = qMax(grooveLength(), 1);
    int position = (orientation() == Qt::Horizontal) ? pos.x() : pos.y();

    qreal ratio = (position - halfWidth) / static_cast<qreal>(grooveSize);
    int newValue = static_cast<int>(ratio * (maximum() - minimum()) + minimum());

    return qBound(minimum(), newValue, maximum());
}

void Slider::mousePressEvent(QMouseEvent *event)
{
    if (!event) {
        return;
    }

    m_pressedPos = event->pos();
    setValue(posToValue(event->pos()));
    emit clicked(value());
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    if (!event) {
        return;
    }

    setValue(posToValue(event->pos()));
    m_pressedPos = event->pos();
    emit sliderMoved(value());
}

void Slider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    bool isDark = Theme::instance()->isDarkTheme();
    painter.setBrush(isDark ? QColor(255, 255, 255, 115) : QColor(0, 0, 0, 100));

    if (orientation() == Qt::Horizontal) {
        drawHorizontalGroove(&painter);
        drawHorizontalTick(&painter);
    } else {
        drawVerticalGroove(&painter);
        drawVerticalTick(&painter);
    }
}

void Slider::drawHorizontalTick(QPainter *painter)
{
    Q_UNUSED(painter);
    // 预留用于绘制刻度
}

void Slider::drawVerticalTick(QPainter *painter)
{
    Q_UNUSED(painter);
    // 预留用于绘制刻度
}

void Slider::drawHorizontalGroove(QPainter *painter)
{
    if (!painter || !m_handle) {
        return;
    }

    qreal w = width();
    qreal r = m_handle->width() / 2.0;

    painter->drawRoundedRect(QRectF(r, r - 2, w - r * 2, 4), 2, 2);

    if (maximum() - minimum() == 0) {
        return;
    }

    bool isDark = Theme::instance()->isDarkTheme();

    QColor color = Theme::instance()->themeColor();
    m_darkGrooveColor = m_darkGrooveColor.isValid() ? m_darkGrooveColor : color;
    m_lightGrooveColor = m_lightGrooveColor.isValid() ? m_lightGrooveColor : color;

    painter->setBrush(isDark ? m_darkGrooveColor : m_lightGrooveColor);
    qreal activeWidth = (value() - minimum()) * (w - r * 2) / static_cast<qreal>(maximum() - minimum());
    painter->drawRoundedRect(QRectF(r, r - 2, activeWidth, 4), 2, 2);
}

void Slider::drawVerticalGroove(QPainter *painter)
{
    if (!painter || !m_handle) {
        return;
    }

    qreal h = height();
    qreal r = m_handle->width() / 2.0;

    painter->drawRoundedRect(QRectF(r - 2, r, 4, h - 2 * r), 2, 2);

    if (maximum() - minimum() == 0) {
        return;
    }

    bool isDark = Theme::instance()->isDarkTheme();
    QColor color = Theme::instance()->themeColor();
    m_darkGrooveColor = m_darkGrooveColor.isValid() ? m_darkGrooveColor : color;
    m_lightGrooveColor = m_lightGrooveColor.isValid() ? m_lightGrooveColor : color;

    painter->setBrush(isDark ? m_darkGrooveColor : m_lightGrooveColor);
    qreal activeHeight = (value() - minimum()) * (h - r * 2) / static_cast<qreal>(maximum() - minimum());
    painter->drawRoundedRect(QRectF(r - 2, r, 4, activeHeight), 2, 2);
}

void Slider::resizeEvent(QResizeEvent *event)
{
    QSlider::resizeEvent(event);
    adjustHandlePosition();
}

// ==================== ClickableSlider 实现 ====================

ClickableSlider::ClickableSlider(QWidget *parent)
    : QSlider(parent)
{
}

ClickableSlider::ClickableSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent)
{
}

void ClickableSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);

    if (!event) {
        return;
    }

    int newValue = 0;

    if (orientation() == Qt::Horizontal) {
        newValue = static_cast<int>(event->pos().x() * maximum() / static_cast<qreal>(width()));
    } else {
        newValue = static_cast<int>((height() - event->pos().y()) * maximum() / static_cast<qreal>(height()));
    }

    setValue(newValue);
    emit clicked(value());
}

// ==================== HollowHandleStyle 实现 ====================

HollowHandleStyle::HollowHandleStyle(const QMap<QString, QVariant> &config)
    : QProxyStyle()
{
    initConfig(config);
}

void HollowHandleStyle::initConfig(const QMap<QString, QVariant> &customConfig)
{
    // 设置默认配置
    m_config["groove.height"] = 3;
    m_config["sub-page.color"] = QColor(255, 255, 255);
    m_config["add-page.color"] = QColor(255, 255, 255, 64);
    m_config["handle.color"] = QColor(255, 255, 255);
    m_config["handle.ring-width"] = 4;
    m_config["handle.hollow-radius"] = 6;
    m_config["handle.margin"] = 4;

    // 应用自定义配置
    for (auto it = customConfig.constBegin(); it != customConfig.constEnd(); ++it) {
        m_config[it.key()] = it.value();
    }

    // 计算手柄大小
    int width = m_config["handle.margin"].toInt() +
            m_config["handle.ring-width"].toInt() +
            m_config["handle.hollow-radius"].toInt();
    m_config["handle.size"] = QSize(2 * width, 2 * width);
}

QRect HollowHandleStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                        SubControl subControl, const QWidget *widget) const
{
    if (control != CC_Slider || !option) {
        return QProxyStyle::subControlRect(control, option, subControl, widget);
    }

    const QStyleOptionSlider *sliderOption = qstyleoption_cast<const QStyleOptionSlider *>(option);
    if (!sliderOption || sliderOption->orientation != Qt::Horizontal || subControl == SC_SliderTickmarks) {
        return QProxyStyle::subControlRect(control, option, subControl, widget);
    }

    QRect rect = option->rect;

    if (subControl == SC_SliderGroove) {
        int h = m_config["groove.height"].toInt();
        QRectF grooveRect(0, (rect.height() - h) / 2.0, rect.width(), h);
        return grooveRect.toRect();
    } else if (subControl == SC_SliderHandle) {
        QSize size = m_config["handle.size"].toSize();
        int x = QStyle::sliderPositionFromValue(sliderOption->minimum, sliderOption->maximum,
                                                sliderOption->sliderPosition, rect.width());

        // 防止手柄超出滑块范围
        x = x * (rect.width() - size.width()) / rect.width();
        QRectF sliderRect(x, 0, size.width(), size.height());
        return sliderRect.toRect();
    }

    return QRect();
}

void HollowHandleStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                           QPainter *painter, const QWidget *widget) const
{
    if (control != CC_Slider || !option || !painter) {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        return;
    }

    const QStyleOptionSlider *sliderOption = qstyleoption_cast<const QStyleOptionSlider *>(option);
    if (!sliderOption || sliderOption->orientation != Qt::Horizontal) {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        return;
    }

    QRect grooveRect = subControlRect(control, option, SC_SliderGroove, widget);
    QRect handleRect = subControlRect(control, option, SC_SliderHandle, widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    // 绘制滑槽
    painter->save();
    painter->translate(grooveRect.topLeft());

    // 绘制已划过的部分
    int w = handleRect.x() - grooveRect.x();
    int h = m_config["groove.height"].toInt();
    painter->setBrush(m_config["sub-page.color"].value<QColor>());
    painter->drawRect(0, 0, w, h);

    // 绘制未划过的部分
    int x = w + m_config["handle.size"].toSize().width();
    painter->setBrush(m_config["add-page.color"].value<QColor>());
    painter->drawRect(x, 0, grooveRect.width() - w, h);
    painter->restore();

    // 绘制手柄
    int ringWidth = m_config["handle.ring-width"].toInt();
    int hollowRadius = m_config["handle.hollow-radius"].toInt();
    int radius = ringWidth + hollowRadius;

    QPainterPath path;
    path.moveTo(0, 0);
    QPoint center = handleRect.center() + QPoint(1, 1);
    path.addEllipse(center, radius, radius);
    path.addEllipse(center, hollowRadius, hollowRadius);

    QColor handleColor = m_config["handle.color"].value<QColor>();
    handleColor.setAlpha((sliderOption->activeSubControls != SC_SliderHandle) ? 255 : 153);
    painter->setBrush(handleColor);
    painter->drawPath(path);

    // 按下手柄时的效果
    if (widget && qobject_cast<const QSlider *>(widget)->isSliderDown()) {
        handleColor.setAlpha(255);
        painter->setBrush(handleColor);
        painter->drawEllipse(handleRect);
    }
}
