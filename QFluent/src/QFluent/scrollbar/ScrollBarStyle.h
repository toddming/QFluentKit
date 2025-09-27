#ifndef SCROLLBARSTYLE_H
#define SCROLLBARSTYLE_H
#include <QProxyStyle>
#include <QPointer>

class ScrollBar;
class ScrollBarStyle : public QProxyStyle
{
    Q_OBJECT
    Q_PROPERTY(qreal Opacity READ getOpacity WRITE setOpacity NOTIFY onOpacityChanged)
    Q_PROPERTY(qreal SliderExtent READ getSliderExtent WRITE setSliderExtent NOTIFY onSliderExtentChanged)

public:
    explicit ScrollBarStyle(QStyle* style = nullptr);
    ~ScrollBarStyle();
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const override;
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr, const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override;
    void startExpandAnimation(bool isExpand);

    void setScrollBar(ScrollBar* bar);

    qreal getOpacity();
    void setOpacity(qreal value);

    void setSliderExtent(qreal value);
    qreal getSliderExtent();

signals:
    void onOpacityChanged();
    void onSliderExtentChanged();

private:
    qreal _sliderMargin{2.5};
    int _scrollBarExtent{10};

    bool m_isExpand;
    qreal m_Opacity;
    qreal m_SliderExtent;
    QPointer<ScrollBar> m_ScrollBar;
};

#endif // SCROLLBARSTYLE_H
