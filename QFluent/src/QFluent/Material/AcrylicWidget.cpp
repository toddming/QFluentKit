#include "AcrylicWidget.h"
#include "AcrylicLabel.h"
#include "Theme.h"

#include <QPainter>
#include <QPainterPath>
#include <QColor>
#include <QWidget>

// ==================== AcrylicWidget 实现 ====================

AcrylicWidget::AcrylicWidget(int blurRadius)
    : m_acrylicBrush(nullptr)
    , m_blurRadius(blurRadius)
{
    // 注意：由于 AcrylicBrush 需要一个 QWidget* 作为设备参数
    // 这里暂时传入 nullptr，子类需要在构造函数中调用 initializeAcrylicBrush
}

AcrylicWidget::~AcrylicWidget()
{
    if (m_acrylicBrush) {
        delete m_acrylicBrush;
        m_acrylicBrush = nullptr;
    }
}

AcrylicBrush* AcrylicWidget::acrylicBrush() const
{
    return m_acrylicBrush;
}

void AcrylicWidget::setBlurRadius(int radius)
{
    if (m_blurRadius == radius) {
        return;
    }

    m_blurRadius = radius;

    if (m_acrylicBrush) {
        m_acrylicBrush->setBlurRadius(radius);
    }
}

int AcrylicWidget::blurRadius() const
{
    return m_blurRadius;
}

void AcrylicWidget::initializeAcrylicBrush(QWidget *device,
                                          const QColor &tintColor,
                                          const QColor &luminosityColor,
                                          double noiseOpacity)
{
    if (!device) {
        return;
    }

    // 如果已存在画刷实例，先清理
    if (m_acrylicBrush) {
        delete m_acrylicBrush;
        m_acrylicBrush = nullptr;
    }

    // 创建新的亚克力画刷实例
    m_acrylicBrush = new AcrylicBrush(
        device,
        m_blurRadius,
        tintColor,
        luminosityColor,
        noiseOpacity
    );
}

void AcrylicWidget::updateAcrylicColor()
{
    if (!m_acrylicBrush) {
        return;
    }

    QColor tintColor;
    QColor luminosityColor;

    if (Theme::instance()->isDarkTheme()) {
        tintColor = QColor(32, 32, 32, 200);
        luminosityColor = QColor(0, 0, 0, 0);
    } else {
        tintColor = QColor(255, 255, 255, 180);
        luminosityColor = QColor(255, 255, 255, 0);
    }

    m_acrylicBrush->setTintColor(tintColor);
    m_acrylicBrush->setLuminosityColor(luminosityColor);
}

QPainterPath AcrylicWidget::acrylicClipPath() const
{
    return QPainterPath();
}

void AcrylicWidget::drawAcrylic(QPainter *painter)
{
    if (!painter || !m_acrylicBrush) {
        return;
    }

    // 获取裁剪路径
    QPainterPath clipPath = acrylicClipPath();

    // 如果裁剪路径非空，则设置到画刷
    if (!clipPath.isEmpty()) {
        m_acrylicBrush->setClipPath(clipPath);
    }

    // 更新亚克力颜色以适应当前主题
    updateAcrylicColor();

    // 执行亚克力绘制
    m_acrylicBrush->paint();
}
