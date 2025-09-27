#include "MenuItemDelegate.h"

#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>

#include "Theme.h"

MenuItemDelegate::MenuItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void MenuItemDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    if (!isSeparator(index)) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // ===== 分隔符绘制逻辑 =====
    painter->save();

    // 根据主题设置分隔符颜色（亮色主题：黑色25%透明度；暗色主题：白色25%透明度）
    int c = Theme::instance()->isDarkMode() ? 255 : 0;
    QPen pen(QColor(c, c, c, 25), 1);
    pen.setCosmetic(true);  // 保持1像素不受缩放影响
    painter->setPen(pen);

    // 计算分隔符位置（垂直居中+4px偏移）
    QRect rect = option.rect;
    int yPos = rect.y() + 4;  // 垂直居中偏移调整

    // 绘制横贯菜单宽度的分隔线（+12px扩展确保覆盖边距）
    painter->drawLine(0, yPos, rect.width() + 12, yPos);

    painter->restore();
}

bool MenuItemDelegate::isSeparator(const QModelIndex &index) const
{
    QVariant data = index.data(Qt::DecorationRole);
    return data.isValid() && data.toString() == "separator";
}



ShortcutMenuItemDelegate::ShortcutMenuItemDelegate(QObject *parent)
    : MenuItemDelegate(parent)
{
}

void ShortcutMenuItemDelegate::paint(QPainter *painter,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    MenuItemDelegate::paint(painter, option, index);

    if (isSeparator(index))
        return;

    QVariant actionData = index.data(Qt::UserRole);
    if (!actionData.isValid())
        return;

    QAction *action = qvariant_cast<QAction*>(actionData);
    if (!action || action->shortcut().isEmpty())
        return;

    painter->save();

    if (!(option.state & QStyle::State_Enabled)) {
        painter->setOpacity(Theme::instance()->isDarkMode() ? 0.5 : 0.6);  // 深色/浅色主题不同透明度
    }

    QFont font = painter->font();
    painter->setFont(font);
    painter->setPen(Theme::instance()->isDarkMode() ?
                        QColor(255, 255, 255, 200) :  // 深色主题：白色80%透明度
                        QColor(0, 0, 0, 153));        // 浅色主题：黑色60%透明度

    QFontMetrics fm(font);
    QString shortcut = action->shortcut().toString(QKeySequence::NativeText);
    int shortcutWidth = fm.horizontalAdvance(shortcut);

    painter->translate(option.rect.width() - shortcutWidth - 20, 0);

    QRectF textRect(0, option.rect.y(),
                    shortcutWidth, option.rect.height());
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, shortcut);

    painter->restore();
}


IndicatorMenuItemDelegate::IndicatorMenuItemDelegate(QObject *parent)
    : MenuItemDelegate(parent)
{

}

void IndicatorMenuItemDelegate::paint(QPainter *painter,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
{

    MenuItemDelegate::paint(painter, option, index);
    if (!(option.state & QStyle::State_Selected))
        return;

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    painter->setPen(Qt::NoPen);
    painter->setBrush(Theme::instance()->themeColor());
    qreal y_offset = (option.rect.height() - 15) / 2.0 + 2;
    painter->drawRoundedRect(6.0, option.rect.y() + y_offset, 3.0, 15.0, 1.5, 1.5);

    painter->restore();
}
