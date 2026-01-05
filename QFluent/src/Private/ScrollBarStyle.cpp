#include "ScrollBarStyle.h"
#include "QFluent/ScrollBar.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QStyleOptionSlider>
#include <QtMath>

#include "Theme.h"

namespace {
    /**
     * @brief 判断是否为深色主题
     * @return true为深色，false为浅色
     * @note 实际项目中应从系统或配置中获取主题信息
     */
    bool isDarkTheme() {
        return Theme::instance()->isDarkTheme();
    }

    /**
     * @brief 获取背景颜色
     */
    QColor getBackgroundColor() {
        return isDarkTheme() ? QColor(44, 44, 44, 245) : QColor(252, 252, 252, 217);
    }

    /**
     * @brief 获取前景颜色（滑块和指示器颜色）
     */
    QColor getForegroundColor() {
        return isDarkTheme() ? QColor(255, 255, 255, 139) : QColor(0, 0, 0, 114);
    }
}

ScrollBarStyle::ScrollBarStyle(QStyle* style)
    : QProxyStyle(style)
    , m_isExpanded(false)
    , m_opacity(0.0)
    , m_sliderExtent(COLLAPSED_SLIDER_EXTENT)
    , m_scrollBar(nullptr)
{
}

ScrollBarStyle::~ScrollBarStyle()
{
    // 弱引用，不需要删除m_scrollBar
}

void ScrollBarStyle::setOpacity(qreal value)
{
    m_opacity = qBound(0.0, value, 1.0); // 确保值在有效范围内
}

void ScrollBarStyle::setSliderExtent(qreal value)
{
    m_sliderExtent = qMax(0.0, value); // 确保值非负
}

void ScrollBarStyle::setScrollBar(ScrollBar* scrollBar)
{
    m_scrollBar = scrollBar;
}

void ScrollBarStyle::drawComplexControl(
    ComplexControl control,
    const QStyleOptionComplex* option,
    QPainter* painter,
    const QWidget* widget
) const
{
    if (control != QStyle::CC_ScrollBar) {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        return;
    }

    const QStyleOptionSlider* sliderOption = qstyleoption_cast<const QStyleOptionSlider*>(option);
    if (!sliderOption || !painter) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    // 绘制背景和指示器（仅在展开状态）
    if (m_isExpanded) {
        // 绘制背景
        painter->setOpacity(m_opacity);
        painter->setBrush(getBackgroundColor());
        painter->drawRoundedRect(sliderOption->rect, BACKGROUND_RADIUS, BACKGROUND_RADIUS);

        // 绘制指示器
        painter->setBrush(getForegroundColor());
        if (sliderOption->orientation == Qt::Horizontal) {
            drawHorizontalIndicators(painter, sliderOption, control, widget);
        } else {
            drawVerticalIndicators(painter, sliderOption, control, widget);
        }
    }

    // 绘制滑块
    painter->setOpacity(1.0);
    drawSlider(painter, sliderOption, control, widget);

    painter->restore();
}

void ScrollBarStyle::drawHorizontalIndicators(
    QPainter* painter,
    const QStyleOptionSlider* option,
    ComplexControl control,
    const QWidget* widget
) const
{
    if (!painter || !option) {
        return;
    }

    const QRect leftIndicatorRect = subControlRect(control, option, QStyle::SC_ScrollBarSubLine, widget);
    const QRect rightIndicatorRect = subControlRect(control, option, QStyle::SC_ScrollBarAddLine, widget);

    const qreal centerY = leftIndicatorRect.height() / 2.0;
    const qreal halfWidth = qCos(qDegreesToRadians(INDICATOR_ANGLE)) * INDICATOR_SIZE / 2.0;
    const qreal halfHeight = INDICATOR_SIZE / 2.0;

    // 左三角（指向左）
    const qreal leftCenterX = leftIndicatorRect.x() + leftIndicatorRect.width() / 2.0;
    QPainterPath leftPath;
    leftPath.moveTo(leftCenterX - halfWidth, centerY);
    leftPath.lineTo(leftCenterX + halfWidth, centerY + halfHeight);
    leftPath.lineTo(leftCenterX + halfWidth, centerY - halfHeight);
    leftPath.closeSubpath();
    painter->drawPath(leftPath);

    // 右三角（指向右）
    const qreal rightCenterX = rightIndicatorRect.x() + rightIndicatorRect.width() / 2.0;
    QPainterPath rightPath;
    rightPath.moveTo(rightCenterX + halfWidth, centerY);
    rightPath.lineTo(rightCenterX - halfWidth, centerY + halfHeight);
    rightPath.lineTo(rightCenterX - halfWidth, centerY - halfHeight);
    rightPath.closeSubpath();
    painter->drawPath(rightPath);
}

void ScrollBarStyle::drawVerticalIndicators(
    QPainter* painter,
    const QStyleOptionSlider* option,
    ComplexControl control,
    const QWidget* widget
) const
{
    if (!painter || !option) {
        return;
    }

    const QRect upIndicatorRect = subControlRect(control, option, QStyle::SC_ScrollBarSubLine, widget);
    const QRect downIndicatorRect = subControlRect(control, option, QStyle::SC_ScrollBarAddLine, widget);

    const qreal centerX = upIndicatorRect.width() / 2.0;
    const qreal halfWidth = INDICATOR_SIZE / 2.0;
    const qreal heightOffset = (INDICATOR_SIZE / 2.0) / qCos(qDegreesToRadians(INDICATOR_ANGLE));
    const qreal bottomOffset = (INDICATOR_SIZE / 2.0) * qTan(qDegreesToRadians(INDICATOR_ANGLE));

    // 上三角（指向上）
    const qreal upCenterY = upIndicatorRect.center().y() + 2.0; // 微调位置
    QPainterPath upPath;
    upPath.moveTo(centerX, upCenterY - heightOffset);
    upPath.lineTo(centerX + halfWidth, upCenterY + bottomOffset);
    upPath.lineTo(centerX - halfWidth, upCenterY + bottomOffset);
    upPath.closeSubpath();
    painter->drawPath(upPath);

    // 下三角（指向下）
    const qreal downCenterY = downIndicatorRect.center().y() + 2.0; // 微调位置
    QPainterPath downPath;
    downPath.moveTo(centerX, downCenterY + bottomOffset);
    downPath.lineTo(centerX + halfWidth, downCenterY - heightOffset);
    downPath.lineTo(centerX - halfWidth, downCenterY - heightOffset);
    downPath.closeSubpath();
    painter->drawPath(downPath);
}

void ScrollBarStyle::drawSlider(
    QPainter* painter,
    const QStyleOptionSlider* option,
    ComplexControl control,
    const QWidget* widget
) const
{
    if (!painter || !option) {
        return;
    }

    QRectF sliderRect = subControlRect(control, option, QStyle::SC_ScrollBarSlider, widget);
    painter->setBrush(getForegroundColor());

    // 根据方向调整滑块位置和大小
    if (option->orientation == Qt::Horizontal) {
        sliderRect.setRect(
            sliderRect.x(),
            sliderRect.bottom() - SLIDER_MARGIN - m_sliderExtent,
            sliderRect.width(),
            m_sliderExtent
        );
    } else {
        sliderRect.setRect(
            sliderRect.right() - SLIDER_MARGIN - m_sliderExtent,
            sliderRect.y(),
            m_sliderExtent,
            sliderRect.height()
        );
    }

    const qreal radius = m_sliderExtent / 2.0;
    painter->drawRoundedRect(sliderRect, radius, radius);
}

int ScrollBarStyle::pixelMetric(
    PixelMetric metric,
    const QStyleOption* option,
    const QWidget* widget
) const
{
    if (metric == QStyle::PM_ScrollBarExtent) {
        return SCROLLBAR_EXTENT;
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

int ScrollBarStyle::styleHint(
    StyleHint hint,
    const QStyleOption* option,
    const QWidget* widget,
    QStyleHintReturn* returnData
) const
{
    // 启用点击滚动条槽时直接跳转到该位置
    if (hint == QStyle::SH_ScrollBar_LeftClickAbsolutePosition) {
        return 1;
    }
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

QPropertyAnimation* ScrollBarStyle::createAnimation(
    const QByteArray& propertyName,
    qreal startValue,
    qreal endValue,
    int duration
)
{
    QPropertyAnimation* animation = new QPropertyAnimation(this, propertyName);
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);

    // 动画完成后自动删除
    connect(animation, &QPropertyAnimation::finished, animation, &QObject::deleteLater);

    return animation;
}

void ScrollBarStyle::startExpandAnimation(bool isExpand)
{
    if (isExpand) {
        // 展开动画
        m_isExpanded = true;

        // 透明度动画
        QPropertyAnimation* opacityAnimation = createAnimation(
            "opacity",
            m_opacity,
            1.0,
            ANIMATION_DURATION
        );

        // 每帧更新滚动条
        if (m_scrollBar) {
            connect(opacityAnimation, &QPropertyAnimation::valueChanged,
                    m_scrollBar, QOverload<>::of(&QWidget::update));
        }

        opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        // 滑块宽度动画
        QPropertyAnimation* extentAnimation = createAnimation(
            "sliderExtent",
            m_sliderExtent,
            SCROLLBAR_EXTENT - 2 * SLIDER_MARGIN,
            ANIMATION_DURATION
        );
        extentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        // 收起动画

        // 透明度动画
        QPropertyAnimation* opacityAnimation = createAnimation(
            "opacity",
            m_opacity,
            0.0,
            ANIMATION_DURATION
        );

        // 动画完成后标记为未展开状态
        connect(opacityAnimation, &QPropertyAnimation::finished, this, [this]() {
            m_isExpanded = false;
        });

        // 每帧更新滚动条
        if (m_scrollBar) {
            connect(opacityAnimation, &QPropertyAnimation::valueChanged,
                    m_scrollBar, QOverload<>::of(&QWidget::update));
        }

        opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        // 滑块宽度动画
        QPropertyAnimation* extentAnimation = createAnimation(
            "sliderExtent",
            m_sliderExtent,
            COLLAPSED_SLIDER_EXTENT,
            ANIMATION_DURATION
        );
        extentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}
