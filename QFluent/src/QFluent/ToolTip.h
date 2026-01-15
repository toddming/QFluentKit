#pragma once

#include <QFrame>
#include <QPoint>
#include <QRect>
#include <QObject>
#include <QModelIndex>

#include "FluentGlobal.h"

class QLabel;
class QTimer;
class QHBoxLayout;
class QPropertyAnimation;
class QGraphicsDropShadowEffect;
class QEvent;
class QWidget;
class QAbstractItemView;
class QTableView;
class QStyleOptionViewItem;
class QHelpEvent;

/**
 * @brief 工具提示位置枚举
 */
enum class ToolTipPosition {
    Top = 0,
    Bottom = 1,
    Left = 2,
    Right = 3,
    TopLeft = 4,
    TopRight = 5,
    BottomLeft = 6,
    BottomRight = 7
};

/**
 * @brief 项视图工具提示类型枚举
 */
enum class ItemViewToolTipType {
    List = 0,
    Table = 1
};

// 前置声明
class ToolTip;
class ToolTipPositionManager;

/**
 * @brief 工具提示类
 */
class QFLUENT_EXPORT ToolTip : public QFrame {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param text 工具提示文本
     * @param parent 父窗口部件指针
     * @param container 自定义容器，如果为 nullptr 则创建默认 QFrame
     */
    explicit ToolTip(const QString &text = QString(), QWidget *parent = nullptr, QFrame *container = nullptr);
    ~ToolTip() override;

    QString text() const;
    void setText(const QString &text);

    int duration() const;
    void setDuration(int duration);

    void adjustPos(QWidget *widget, ToolTipPosition position);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    /**
     * @brief 获取容器
     * @return 容器指针
     */
    QFrame* container() const;

private:
    void setupUi();
    void setStyleSheet();

    QString m_text;
    int m_duration;
    QFrame *m_container;
    QTimer *m_timer;
    QHBoxLayout *m_containerLayout;
    QLabel *m_label;
    QPropertyAnimation *m_opacityAnimation;
    QGraphicsDropShadowEffect *m_shadowEffect;
};

/**
 * @brief 工具提示位置管理器基类
 */
class ToolTipPositionManager {
public:
    virtual ~ToolTipPositionManager() = default;

    QPoint position(ToolTip *tooltip, QWidget *parent);

    static ToolTipPositionManager* make(ToolTipPosition position);

protected:
    virtual QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) = 0;
};

/**
 * @brief 顶部工具提示位置管理器
 */
class TopToolTipManager : public ToolTipPositionManager {
protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 底部工具提示位置管理器
 */
class BottomToolTipManager : public ToolTipPositionManager {
protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 左侧工具提示位置管理器
 */
class LeftToolTipManager : public ToolTipPositionManager {
protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 右侧工具提示位置管理器
 */
class RightToolTipManager : public ToolTipPositionManager {
protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 右上工具提示位置管理器
 */
class TopRightToolTipManager : public ToolTipPositionManager {
protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 左上工具提示位置管理器
 */
class TopLeftToolTipManager : public ToolTipPositionManager {
protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 右下工具提示位置管理器
 */
class BottomRightToolTipManager : public ToolTipPositionManager {
protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 左下工具提示位置管理器
 */
class BottomLeftToolTipManager : public ToolTipPositionManager {
protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 项视图工具提示位置管理器
 */
class ItemViewToolTipManager : public ToolTipPositionManager {
public:
    explicit ItemViewToolTipManager(const QRect &itemRect = QRect());
    ~ItemViewToolTipManager() override = default;

    static ItemViewToolTipManager* make(ItemViewToolTipType tipType, const QRect &itemRect);

protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;

    QRect m_itemRect;
};

/**
 * @brief 表格项视图工具提示位置管理器
 */
class TableItemToolTipManager : public ItemViewToolTipManager {
public:
    explicit TableItemToolTipManager(const QRect &itemRect = QRect());
    ~TableItemToolTipManager() override = default;

protected:
    QPoint calculatePosition(ToolTip *tooltip, QWidget *parent) override;
};

/**
 * @brief 工具提示过滤器
 */
class QFLUENT_EXPORT ToolTipFilter : public QObject {
    Q_OBJECT

public:
    explicit ToolTipFilter(QWidget *parent, int showDelay = 300,
                          ToolTipPosition position = ToolTipPosition::Top);
    ~ToolTipFilter() override;

    void setToolTipDelay(int delay);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    virtual ToolTip* createToolTip();
    virtual bool canShowToolTip() const;

protected slots:
    void hideToolTip();
    void showToolTip();

protected:
    bool m_isEnter;
    ToolTip *m_tooltip;
    int m_tooltipDelay;
    ToolTipPosition m_position;
    QTimer *m_timer;
};

/**
 * @brief 项视图工具提示
 */
class QFLUENT_EXPORT ItemViewToolTip : public ToolTip {
    Q_OBJECT

public:
    explicit ItemViewToolTip(const QString &text = QString(), QWidget *parent = nullptr);
    ~ItemViewToolTip() override = default;

    void adjustPos(QAbstractItemView *view, const QRect &itemRect,
                   ItemViewToolTipType tooltipType);
};

/**
 * @brief 项视图工具提示代理
 */
class QFLUENT_EXPORT ItemViewToolTipDelegate : public ToolTipFilter {
    Q_OBJECT

public:
    explicit ItemViewToolTipDelegate(QAbstractItemView *parent, int showDelay = 300,
                                    ItemViewToolTipType tooltipType = ItemViewToolTipType::Table);
    ~ItemViewToolTipDelegate() override = default;

    void setText(const QString &text);
    void setToolTipDuration(int duration);

    bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
                   QStyleOptionViewItem *option, const QModelIndex &index);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    ToolTip* createToolTip() override;
    bool canShowToolTip() const override;

protected slots:
    void showToolTip();

private:
    QString m_text;
    QModelIndex m_currentIndex;
    int m_tooltipDuration;
    ItemViewToolTipType m_tooltipType;
    QWidget *m_viewport;
};
