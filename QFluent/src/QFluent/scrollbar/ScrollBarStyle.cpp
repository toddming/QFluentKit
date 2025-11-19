#include "ScrollBarStyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QStyleOptionSlider>
#include <QtMath>
#include <QSvgRenderer>

#include "ScrollBar.h"
#include "Theme.h"
#include "FluentIcon.h"

ScrollBarStyle::ScrollBarStyle(QStyle* style)
{
    m_isExpand = false;
    m_Opacity = 0;
    m_ScrollBar = nullptr;
    m_SliderExtent = 2.4;
}

ScrollBarStyle::~ScrollBarStyle()
{
}

void ScrollBarStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const
{
    switch (control)
    {
    case QStyle::CC_ScrollBar:
    {
        if (const QStyleOptionSlider* sopt = qstyleoption_cast<const QStyleOptionSlider*>(option))
        {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(Qt::NoPen);
            QRect scrollBarRect = sopt->rect;
            if (m_isExpand)
            {
                // 背景绘制
                painter->setOpacity(m_Opacity);
                painter->setPen(Qt::NoPen);
                painter->setBrush(Theme::instance()->isDarkTheme() ? QColor(44, 44, 44, 245) : QColor(252, 252, 252, 217)); // 滑槽
                painter->drawRoundedRect(scrollBarRect, 6, 6);
                //指示器绘制 center()在此处不适用 存在外围边距宽度 需手动计算
                int sideLength = 8;
                painter->setBrush(Theme::instance()->isDarkTheme() ? QColor(255, 255, 255, 139) : QColor(0, 0, 0, 114));

                QMap<QString, QString> attrs;
                attrs["fill"] = Theme::instance()->isDarkTheme() ? "#FFFFFF" : "#000000";
                attrs["fill-opacity"] = Theme::instance()->isDarkTheme() ?  "0.54" : "0.45";

                if (sopt->orientation == Qt::Horizontal)
                {
                    QRect leftIndicatorRect = subControlRect(control, sopt, QStyle::SC_ScrollBarSubLine, widget);
                    QRect rightIndicatorRect = subControlRect(control, sopt, QStyle::SC_ScrollBarAddLine, widget);
                    // 左三角
                    FluentIcon(Fluent::IconType::CARE_LEFT_SOLID).render(painter, leftIndicatorRect, Fluent::ThemeMode::AUTO, QList<int>(), attrs);
                    // 右三角
                    FluentIcon(Fluent::IconType::CARE_RIGHT_SOLID).render(painter, rightIndicatorRect, Fluent::ThemeMode::AUTO, QList<int>(), attrs);

                } else {
                    QRect upIndicatorRect = subControlRect(control, sopt, QStyle::SC_ScrollBarSubLine, widget);
                    QRect downIndicatorRect = subControlRect(control, sopt, QStyle::SC_ScrollBarAddLine, widget);
                    // 上三角
                    FluentIcon(Fluent::IconType::CARE_UP_SOLID).render(painter, upIndicatorRect, Fluent::ThemeMode::AUTO, QList<int>(), attrs);
                    // 下三角
                    FluentIcon(Fluent::IconType::CARE_DOWN_SOLID).render(painter, downIndicatorRect, Fluent::ThemeMode::AUTO, QList<int>(), attrs);
                }
            }
            painter->setOpacity(1);
            //滑块绘制
            QRectF sliderRect = subControlRect(control, sopt, QStyle::SC_ScrollBarSlider, widget);
            painter->setBrush(Theme::instance()->isDarkTheme() ? QColor(255, 255, 255, 139) : QColor(0, 0, 0, 114));
            if (sopt->orientation == Qt::Horizontal) {
                sliderRect.setRect(sliderRect.x(), sliderRect.bottom() - _sliderMargin - m_SliderExtent, sliderRect.width(), m_SliderExtent);
            } else {
                sliderRect.setRect(sliderRect.right() - _sliderMargin - m_SliderExtent, sliderRect.y(), m_SliderExtent, sliderRect.height());
            }
            painter->drawRoundedRect(sliderRect, m_SliderExtent / 2.0, m_SliderExtent / 2.0);
            painter->restore();
        }
        return;
    }
    default:
    {
        break;
    }
    }
    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

int ScrollBarStyle::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    switch (metric)
    {
    case QStyle::PM_ScrollBarExtent:
    {
        return _scrollBarExtent;
    }
    default:
    {
        break;
    }
    }
    return QProxyStyle::pixelMetric(metric, option, widget);
}

int ScrollBarStyle::styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData) const
{
    if (hint == QStyle::SH_ScrollBar_LeftClickAbsolutePosition) {
        return true;
    }
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

void ScrollBarStyle::startExpandAnimation(bool isExpand)
{
    QPointer<ScrollBarStyle> self = this;
    if (isExpand) {
        m_isExpand = true;
        QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "Opacity");
        connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
            if (!self.isNull() && ! m_ScrollBar.isNull())
                m_ScrollBar->update();
        });
        opacityAnimation->setDuration(250);
        opacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
        opacityAnimation->setStartValue(m_Opacity);
        opacityAnimation->setEndValue(1);
        opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation* extentAnimation = new QPropertyAnimation(this, "SliderExtent");
        extentAnimation->setDuration(250);
        extentAnimation->setEasingCurve(QEasingCurve::InOutSine);
        extentAnimation->setStartValue(m_SliderExtent);
        extentAnimation->setEndValue(_scrollBarExtent - 2 * _sliderMargin);
        extentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    } else {
        QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "Opacity");
        connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
            if (!self.isNull())
                m_isExpand = false;
        });
        connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=]() {
            if (!self.isNull() && !m_ScrollBar.isNull())
                m_ScrollBar->update();
        });
        opacityAnimation->setDuration(250);
        opacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
        opacityAnimation->setStartValue(m_Opacity);
        opacityAnimation->setEndValue(0);
        opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation* extentAnimation = new QPropertyAnimation(this, "SliderExtent");
        extentAnimation->setDuration(250);
        extentAnimation->setEasingCurve(QEasingCurve::InOutSine);
        extentAnimation->setStartValue(m_SliderExtent);
        extentAnimation->setEndValue(2.4);
        extentAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}


void ScrollBarStyle::setScrollBar(ScrollBar* bar)
{
    if (bar != m_ScrollBar) {
        m_ScrollBar = bar;
    }
}

void ScrollBarStyle::setOpacity(qreal value)
{
    if (value != m_Opacity) {
        m_Opacity = value;
        emit onOpacityChanged();
    }
}

qreal ScrollBarStyle::getOpacity()
{
    return m_Opacity;
}


void ScrollBarStyle::setSliderExtent(qreal value)
{
    if (value != m_SliderExtent) {
        m_SliderExtent = value;
        emit onSliderExtentChanged();
    }
}

qreal ScrollBarStyle::getSliderExtent()
{
    return m_SliderExtent;
}
