#include "Slider.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QStyleOptionSlider>
#include <QRectF>
#include <QtMath>

#include "Theme.h"
// ============================================================================
// SliderHandle Implementation
// ============================================================================

SliderHandle::SliderHandle(QSlider* parent)
    : QWidget(parent)
    , m_radius(5.0)
{
    setFixedSize(22, 22);

    m_radiusAni = new QPropertyAnimation(this, "radius", this);
    m_radiusAni->setDuration(100);
}

void SliderHandle::setRadius(qreal r)
{
    if (m_radius != r) {
        m_radius = r;
        update();
    }
}

void SliderHandle::setHandleColor(const QColor& light, const QColor& dark)
{
    m_lightHandleColor = light;
    m_darkHandleColor = dark;
    update();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void SliderHandle::enterEvent(QEnterEvent *e) {
    QWidget::enterEvent(e);
    startAni(6.5);
}
#else
void SliderHandle::enterEvent(QEvent *e) {
    QWidget::enterEvent(e);
    startAni(6.5);
}
#endif

void SliderHandle::leaveEvent(QEvent* e)
{
    QWidget::leaveEvent(e);
    startAni(5.0);
}

void SliderHandle::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent(e);
    startAni(4.0);
    emit pressed();
}

void SliderHandle::mouseReleaseEvent(QMouseEvent* e)
{
    QWidget::mouseReleaseEvent(e);
    startAni(6.5);
    emit released();
}

void SliderHandle::startAni(qreal radius)
{
    m_radiusAni->stop();
    m_radiusAni->setStartValue(m_radius);
    m_radiusAni->setEndValue(radius);
    m_radiusAni->start();
}

void SliderHandle::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    bool isDark = Theme::instance()->isDarkTheme();

    // 绘制外圈
    painter.setPen(QColor(0, 0, 0, isDark ? 90 : 25));
    painter.setBrush(isDark ? QColor(69, 69, 69) : Qt::white);
    painter.drawEllipse(rect().adjusted(1, 1, -1, -1));

    // 绘制内圈
    QColor color = isDark ? m_darkHandleColor : m_lightHandleColor;
    color = color.isValid() ? color : Theme::instance()->themeColor();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(QPointF(11, 11), m_radius, m_radius);

}


// ============================================================================
// Slider Implementation
// ============================================================================

Slider::Slider(QWidget* parent)
    : QSlider(parent)
{
    postInit();
}

Slider::Slider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
{
    postInit();
}

void Slider::postInit()
{
    m_handle = new SliderHandle(this);
    setOrientation(orientation());

    connect(m_handle, &SliderHandle::pressed, this, &QSlider::sliderPressed);
    connect(m_handle, &SliderHandle::released, this, &QSlider::sliderReleased);
    connect(this, &QSlider::valueChanged, this, &Slider::adjustHandlePos);
}

void Slider::setThemeColor(const QColor& light, const QColor& dark)
{
    m_lightGrooveColor = light;
    m_darkGrooveColor = dark;
    m_handle->setHandleColor(light, dark);
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
    int l = (orientation() == Qt::Horizontal) ? width() : height();
    return l - m_handle->width();
}

void Slider::mousePressEvent(QMouseEvent* e)
{
    m_pressedPos = e->pos();
    setValue(posToValue(e->pos()));
    emit clicked(value());
}

void Slider::mouseMoveEvent(QMouseEvent* e)
{
    setValue(posToValue(e->pos()));
    m_pressedPos = e->pos();
    emit sliderMoved(value());
}

void Slider::adjustHandlePos()
{
    int total = qMax(maximum() - minimum(), 1);
    int delta = static_cast<int>((value() - minimum()) * grooveLength() / static_cast<qreal>(total));

    if (orientation() == Qt::Vertical) {
        m_handle->move(0, delta);
    } else {
        m_handle->move(delta, 0);
    }
}

int Slider::posToValue(const QPoint& pos) const
{
    qreal pd = m_handle->width() / 2.0;
    qreal gs = qMax(grooveLength(), 1);
    qreal v = (orientation() == Qt::Horizontal) ? pos.x() : pos.y();
    return static_cast<int>((v - pd) / gs * (maximum() - minimum()) + minimum());
}

void Slider::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Theme::instance()->isDarkTheme() ? QColor(255, 255, 255, 115) : QColor(0, 0, 0, 100));

    if (orientation() == Qt::Horizontal) {
        drawHorizonGroove(&painter);
        drawHorizonTick(&painter);
    } else {
        drawVerticalGroove(&painter);
        drawVerticalTick(&painter);
    }
}

void Slider::drawHorizonTick(QPainter* painter)
{
    Q_UNUSED(painter);
    // 留空，子类可以重写
}

void Slider::drawVerticalTick(QPainter* painter)
{
    Q_UNUSED(painter);
    // 留空，子类可以重写
}

void Slider::drawHorizonGroove(QPainter* painter)
{
    qreal w = width();
    qreal r = m_handle->width() / 2.0;
    painter->drawRoundedRect(QRectF(r, r - 2, w - r * 2, 4), 2, 2);

    if (maximum() - minimum() == 0) {
        return;
    }

    bool isDark = Theme::instance()->isDarkTheme();
    QColor color = isDark ? m_darkGrooveColor : m_lightGrooveColor;
    color = color.isValid() ? color : Theme::instance()->themeColor();

    painter->setBrush(color);
    qreal aw = (value() - minimum()) / static_cast<qreal>(maximum() - minimum()) * (w - r * 2);
    painter->drawRoundedRect(QRectF(r, r - 2, aw, 4), 2, 2);
}

void Slider::drawVerticalGroove(QPainter* painter)
{
    qreal h = height();
    qreal r = m_handle->width() / 2.0;
    painter->drawRoundedRect(QRectF(r - 2, r, 4, h - 2 * r), 2, 2);

    if (maximum() - minimum() == 0) {
        return;
    }

    bool isDark = Theme::instance()->isDarkTheme();
    QColor color = isDark ? m_darkGrooveColor : m_lightGrooveColor;
    color = color.isValid() ? color : Theme::instance()->themeColor();

    painter->setBrush(color);
    qreal ah = (value() - minimum()) / static_cast<qreal>(maximum() - minimum()) * (h - r * 2);
    painter->drawRoundedRect(QRectF(r - 2, r, 4, ah), 2, 2);
}

void Slider::resizeEvent(QResizeEvent* e)
{
    QSlider::resizeEvent(e);
    adjustHandlePos();
}


// ============================================================================
// ClickableSlider Implementation
// ============================================================================

ClickableSlider::ClickableSlider(QWidget* parent)
    : QSlider(parent)
{
}

ClickableSlider::ClickableSlider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
{
}

void ClickableSlider::mousePressEvent(QMouseEvent* e)
{
    QSlider::mousePressEvent(e);

    int value;
    if (orientation() == Qt::Horizontal) {
        value = static_cast<int>(e->pos().x() / static_cast<qreal>(width()) * maximum());
    } else {
        value = static_cast<int>((height() - e->pos().y()) / static_cast<qreal>(height()) * maximum());
    }

    setValue(value);
    emit clicked(this->value());
}


// ============================================================================
// HollowHandleStyle Implementation
// ============================================================================

HollowHandleStyle::HollowHandleStyle(const QMap<QString, QVariant>& config)
    : QProxyStyle()
{
    // 默认配置
    m_config["groove.height"] = 3;
    m_config["sub-page.color"] = QColor(255, 255, 255);
    m_config["add-page.color"] = QColor(255, 255, 255, 64);
    m_config["handle.color"] = QColor(255, 255, 255);
    m_config["handle.ring-width"] = 4;
    m_config["handle.hollow-radius"] = 6;
    m_config["handle.margin"] = 4;

    // 应用用户配置
    for (auto it = config.constBegin(); it != config.constEnd(); ++it) {
        m_config[it.key()] = it.value();
    }

    // 计算手柄大小
    int w = m_config["handle.margin"].toInt() +
            m_config["handle.ring-width"].toInt() +
            m_config["handle.hollow-radius"].toInt();
    m_config["handle.size"] = QSize(2 * w, 2 * w);
}

QRect HollowHandleStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex* opt,
                                        SubControl sc, const QWidget* widget) const
{
    const QStyleOptionSlider* sliderOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt);

    if (cc != CC_Slider || !sliderOpt || sliderOpt->orientation != Qt::Horizontal || sc == SC_SliderTickmarks) {
        return QProxyStyle::subControlRect(cc, opt, sc, widget);
    }

    QRect rect = opt->rect;

    if (sc == SC_SliderGroove) {
        int h = m_config["groove.height"].toInt();
        QRectF grooveRect(0, (rect.height() - h) / 2.0, rect.width(), h);
        return grooveRect.toRect();
    }
    else if (sc == SC_SliderHandle) {
        QSize size = m_config["handle.size"].toSize();
        int x = QStyle::sliderPositionFromValue(sliderOpt->minimum, sliderOpt->maximum,
                                                 sliderOpt->sliderPosition, rect.width());

        // 解决手柄跑出滑块的问题
        x = x * (rect.width() - size.width()) / rect.width();
        QRectF sliderRect(x, 0, size.width(), size.height());
        return sliderRect.toRect();
    }

    return QRect();
}

void HollowHandleStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt,
                                          QPainter* painter, const QWidget* widget) const
{
    const QStyleOptionSlider* sliderOpt = qstyleoption_cast<const QStyleOptionSlider*>(opt);

    if (cc != CC_Slider || !sliderOpt || sliderOpt->orientation != Qt::Horizontal) {
        return QProxyStyle::drawComplexControl(cc, opt, painter, widget);
    }

    QRect grooveRect = subControlRect(cc, opt, SC_SliderGroove, widget);
    QRect handleRect = subControlRect(cc, opt, SC_SliderHandle, widget);
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
    handleColor.setAlpha(sliderOpt->activeSubControls != SC_SliderHandle ? 255 : 153);
    painter->setBrush(handleColor);
    painter->drawPath(path);

    // 按下手柄
    if (widget && qobject_cast<const QSlider*>(widget)->isSliderDown()) {
        handleColor.setAlpha(255);
        painter->setBrush(handleColor);
        painter->drawEllipse(handleRect);
    }
}
