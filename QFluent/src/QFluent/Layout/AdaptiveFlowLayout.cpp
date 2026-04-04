#include "AdaptiveFlowLayout.h"
#include <QWidget>
#include <QWidgetItem>
#include <QtMath>

QT_BEGIN_NAMESPACE

AdaptiveFlowLayout::AdaptiveFlowLayout(QWidget *parent, bool enableAnimation, bool tightMode)
    : FlowLayout(parent, enableAnimation, tightMode)
    , m_widgetMinimumWidth(200)
    , m_widgetMaximumWidth(-1)
{
}

void AdaptiveFlowLayout::setWidgetMinimumWidth(int width)
{
    m_widgetMinimumWidth = qMax(1, width);
}

int AdaptiveFlowLayout::widgetMinimumWidth() const
{
    return m_widgetMinimumWidth;
}

void AdaptiveFlowLayout::setWidgetMaximumWidth(int width)
{
    m_widgetMaximumWidth = width;
}

int AdaptiveFlowLayout::widgetMaximumWidth() const
{
    return m_widgetMaximumWidth;
}

int AdaptiveFlowLayout::calculateHeight(const QRect &rect) const
{
    const QMargins margins = contentsMargins();
    int spaceX = horizontalSpacing();
    int spaceY = verticalSpacing();

    // 计算可用宽度
    int availableWidth = rect.width() - margins.left() - margins.right();

    // 计算每行可容纳的控件数量
    int cardsPerRow = 1;
    if (m_widgetMinimumWidth + spaceX > 0) {
        cardsPerRow = qMax(1, (availableWidth + spaceX) / (m_widgetMinimumWidth + spaceX));
    }

    // 计算控件宽度以填满整行
    int cardWidth = availableWidth;
    if (cardsPerRow > 1) {
        cardWidth = (availableWidth - (cardsPerRow - 1) * spaceX) / cardsPerRow;
    }

    // 应用最大宽度限制
    if (m_widgetMaximumWidth > 0 && cardWidth > m_widgetMaximumWidth) {
        cardWidth = m_widgetMaximumWidth;
    }

    // 计算高度
    int y = rect.y() + margins.top();
    int rowHeight = 0;
    int colIndex = 0;

    for (int i = 0; i < m_items.size(); ++i) {
        QLayoutItem *item = m_items.at(i);
        if (!item) {
            continue;
        }

        // 紧凑模式下跳过不可见的控件
        QWidget *widget = item->widget();
        if (tightMode() && widget && !widget->isVisible()) {
            continue;
        }

        // 检查是否需要换行
        if (colIndex >= cardsPerRow && cardsPerRow > 0) {
            y += rowHeight + spaceY;
            rowHeight = 0;
            colIndex = 0;
        }

        // 更新行高
        rowHeight = qMax(rowHeight, item->sizeHint().height());
        colIndex++;
    }

    return y + rowHeight + margins.bottom() - rect.y();
}

int AdaptiveFlowLayout::doLayout(const QRect &rect, bool applyGeometry)
{
    const QMargins margins = contentsMargins();
    int spaceX = horizontalSpacing();
    int spaceY = verticalSpacing();

    // 计算可用宽度
    int availableWidth = rect.width() - margins.left() - margins.right();

    // 计算每行可容纳的控件数量
    int cardsPerRow = 1;
    if (m_widgetMinimumWidth + spaceX > 0) {
        cardsPerRow = qMax(1, (availableWidth + spaceX) / (m_widgetMinimumWidth + spaceX));
    }

    // 计算控件宽度以填满整行
    int cardWidth = availableWidth;
    if (cardsPerRow > 1) {
        cardWidth = (availableWidth - (cardsPerRow - 1) * spaceX) / cardsPerRow;
    }

    // 应用最大宽度限制
    if (m_widgetMaximumWidth > 0 && cardWidth > m_widgetMaximumWidth) {
        cardWidth = m_widgetMaximumWidth;
    }

    // 执行布局
    int x = rect.x() + margins.left();
    int y = rect.y() + margins.top();
    int rowHeight = 0;
    int colIndex = 0;

    for (int i = 0; i < m_items.size(); ++i) {
        QLayoutItem *item = m_items.at(i);
        if (!item) {
            continue;
        }

        // 紧凑模式下跳过不可见的控件
        QWidget *widget = item->widget();
        if (tightMode() && widget && !widget->isVisible()) {
            continue;
        }

        // 检查是否需要换行
        if (colIndex >= cardsPerRow && cardsPerRow > 0) {
            x = rect.x() + margins.left();
            y += rowHeight + spaceY;
            rowHeight = 0;
            colIndex = 0;
        }

        // 应用几何位置
        if (applyGeometry) {
            // 获取控件原始高度，使用计算后的宽度
            QSize itemSizeHint = item->sizeHint();
            int itemHeight = itemSizeHint.height();

            // 如果控件有固定高度，使用固定高度
            if (widget && widget->hasHeightForWidth()) {
                itemHeight = widget->heightForWidth(cardWidth);
            }

            QRect targetGeometry(x, y, cardWidth, itemHeight);
            item->setGeometry(targetGeometry);
        }

        // 更新位置
        x += cardWidth + spaceX;
        rowHeight = qMax(rowHeight, item->sizeHint().height());
        colIndex++;
    }

    return y + rowHeight + margins.bottom() - rect.y();
}

QT_END_NAMESPACE
