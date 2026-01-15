#include "AcrylicToolTip.h"
#include "AcrylicLabel.h"

#include <QPainter>
#include <QPainterPath>
#include <QShowEvent>
#include <QRect>
#include <QRectF>
#include <QPoint>
#include <QWidget>

// ============================================================================
// AcrylicToolTipContainer 实现
// ============================================================================

AcrylicToolTipContainer::AcrylicToolTipContainer(QWidget *parent)
    : QFrame(parent)
    , AcrylicWidget(20)  // 默认模糊半径
{
    setProperty("transparent", true);

    initializeAcrylicBrush(
        this,                           // 绘制设备
        QColor(242, 242, 242, 150),     // 色调颜色
        QColor(255, 255, 255, 10),      // 亮度颜色
        0.02                            // 噪声不透明度
    );
}

QPainterPath AcrylicToolTipContainer::acrylicClipPath() const
{
    // 创建圆角矩形裁剪路径
    // 边距调整 1 像素，圆角半径 3 像素
    QPainterPath path;
    QRectF adjustedRect = QRectF(rect()).adjusted(1, 1, -1, -1);
    path.addRoundedRect(adjustedRect, 3.0, 3.0);
    return path;
}

void AcrylicToolTipContainer::paintEvent(QPaintEvent *event)
{
    // 绘制亚克力效果
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    drawAcrylic(&painter);

    // 调用基类的绘制事件
    QFrame::paintEvent(event);
}

// ============================================================================
// AcrylicToolTip 实现
// ============================================================================

AcrylicToolTip::AcrylicToolTip(const QString &text, QWidget *parent)
    : ToolTip(text, parent, new AcrylicToolTipContainer(parent))
    , m_acrylicContainer(static_cast<AcrylicToolTipContainer*>(container()))
{
    // 容器已经在父类构造函数中设置好了
    // m_acrylicContainer 指向同一个容器对象
}

AcrylicToolTipContainer* AcrylicToolTip::acrylicContainer() const
{
    return m_acrylicContainer;
}

void AcrylicToolTip::showEvent(QShowEvent *event)
{
    // 在显示工具提示时，抓取背景图像并应用到亚克力画刷
    if (m_acrylicContainer && m_acrylicContainer->acrylicBrush()) {
        // 计算容器在屏幕上的全局位置
        QPoint containerGlobalPos = pos() + m_acrylicContainer->pos();

        // 创建抓取区域矩形
        QRect grabRect(containerGlobalPos, m_acrylicContainer->size());

        // 抓取屏幕图像并应用到亚克力画刷
        m_acrylicContainer->acrylicBrush()->grabImage(grabRect);
    }

    // 调用基类的显示事件处理
    ToolTip::showEvent(event);
}

// ============================================================================
// AcrylicToolTipFilter 实现
// ============================================================================

AcrylicToolTipFilter::AcrylicToolTipFilter(QWidget *parent,
                                           int showDelay,
                                           ToolTipPosition position)
    : ToolTipFilter(parent, showDelay, position)
{
    // 基类构造函数已经完成所有初始化工作
}

ToolTip* AcrylicToolTipFilter::createToolTip()
{
    // 创建亚克力工具提示实例
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) {
        return nullptr;
    }

    // 获取父窗口的工具提示文本
    QString tooltipText = parentWidget->toolTip();

    // 创建并返回亚克力工具提示
    return new AcrylicToolTip(tooltipText, parentWidget->window());
}

