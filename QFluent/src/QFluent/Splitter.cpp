#include "Splitter.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QStyleOption>

#include "Theme.h"

// ============================================================================
// SplitterHandle 实现
// ============================================================================

SplitterHandle::SplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent)
    , m_isHovered(false)
    , m_isPressed(false)
{
    // 设置鼠标跟踪以接收鼠标移动事件
    setMouseTracking(true);

    // 设置手柄大小
    if (orientation == Qt::Horizontal) {
        setFixedWidth(8);
    } else {
        setFixedHeight(8);
    }
}

SplitterHandle::~SplitterHandle()
{
    // 析构函数：确保资源正确释放
    // 由于没有动态分配的资源，这里无需特殊处理
}

void SplitterHandle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_isHovered) {
        // 鼠标悬停时绘制完整分割线
        drawFullLine(&painter);
    }
    // 绘制小方块
    drawHandle(&painter);
}

void SplitterHandle::drawHandle(QPainter *painter)
{
    if (!painter) {
        return;
    }

    // 设置画笔颜色（浅灰色）
    painter->setPen(Qt::NoPen);

    QColor color = Theme::isDark() ? QColor(159, 159, 159) : QColor(138, 138, 138);
    painter->setBrush(color);

    // 计算小方块的位置和大小
    const int handleSize = 32; // 小方块的长度
    const int handleThickness = 4; // 小方块的厚度

    QRect handleRect;

    if (orientation() == Qt::Horizontal) {
        // 水平分割器：垂直方向的小方块
        int centerY = height() / 2;
        handleRect = QRect(
            (width() - handleThickness) / 2,
            centerY - handleSize / 2,
            handleThickness,
            handleSize
        );
    } else {
        // 垂直分割器：水平方向的小方块
        int centerX = width() / 2;
        handleRect = QRect(
            centerX - handleSize / 2,
            (height() - handleThickness) / 2,
            handleSize,
            handleThickness
        );
    }

    // 绘制圆角矩形
    painter->drawRoundedRect(handleRect, 2, 2);
}

void SplitterHandle::drawFullLine(QPainter *painter)
{
    if (!painter) {
        return;
    }

    int alpha = m_isPressed ? 23 : 15;
    QColor color = Theme::isDark() ? QColor(255, 255, 255, alpha) : QColor(0, 0, 0, alpha);
    painter->setBrush(color);
    painter->setPen(Qt::NoPen);

    const int lineThickness = 8; // 线条厚度

    QRect lineRect;

    if (orientation() == Qt::Horizontal) {
        // 水平分割器：垂直贯穿的线
        lineRect = QRect(
            (width() - lineThickness) / 2,
            0,
            lineThickness,
            height()
        );
    } else {
        // 垂直分割器：水平贯穿的线
        lineRect = QRect(
            0,
            (height() - lineThickness) / 2,
            width(),
            lineThickness
        );
    }

    // 绘制圆角矩形
    painter->drawRect(lineRect);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void SplitterHandle::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);

    m_isHovered = true;
    update();

    QSplitterHandle::enterEvent(event);
}
#else
void SplitterHandle::enterEvent(QEvent *event)
{
    Q_UNUSED(event);

    m_isHovered = true;
    update();

    QSplitterHandle::enterEvent(event);
}
#endif

void SplitterHandle::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);

    m_isHovered = false;
    update(); // 触发重绘

    // 调用基类实现
    QSplitterHandle::leaveEvent(event);
}

void SplitterHandle::mouseMoveEvent(QMouseEvent *event)
{
    // 确保在拖动时保持悬停状态
    if (!m_isHovered) {
        m_isHovered = true;
        update();
    }

    // 调用基类实现以处理分割器拖动
    QSplitterHandle::mouseMoveEvent(event);
}

void SplitterHandle::mousePressEvent(QMouseEvent *event)
{
    // 设置鼠标按下状态
    m_isPressed = true;
    update(); // 触发重绘以更新透明度

    // 调用基类实现以处理分割器拖动
    QSplitterHandle::mousePressEvent(event);
}

void SplitterHandle::mouseReleaseEvent(QMouseEvent *event)
{
    // 重置鼠标按下状态
    m_isPressed = false;
    update(); // 触发重绘以恢复透明度

    // 调用基类实现
    QSplitterHandle::mouseReleaseEvent(event);
}

// ============================================================================
// Splitter 实现
// ============================================================================

Splitter::Splitter(QWidget *parent)
    : QSplitter(parent)
{
    // 设置手柄宽度
    setHandleWidth(8);

    // 设置样式
    setStyleSheet("QSplitter::handle { background: transparent; }");
}

Splitter::Splitter(Qt::Orientation orientation, QWidget *parent)
    : QSplitter(orientation, parent)
{
    // 设置手柄宽度
    setHandleWidth(8);

    // 设置样式
    setStyleSheet("QSplitter::handle { background: transparent; }");
}

Splitter::~Splitter()
{
    // 析构函数：确保资源正确释放
    // Qt 对象树机制会自动管理子对象的释放
}

QSplitterHandle *Splitter::createHandle()
{
    // 创建自定义手柄
    // 返回的指针由 Qt 对象树管理，无需手动删除
    return new SplitterHandle(orientation(), this);
}
