#pragma once

#include <QSplitter>
#include <QSplitterHandle>

#include "FluentGlobal.h"

// 前置声明
class QPaintEvent;
class QMouseEvent;
class QEnterEvent;
class QEvent;

/**
 * @class SplitterHandle
 * @brief 自定义分割器手柄
 * @details 实现鼠标悬停效果：默认显示小方块，悬停时显示完整分割线
 */
class SplitterHandle : public QSplitterHandle
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param orientation 分割器方向（水平或垂直）
     * @param parent 父分割器对象
     */
    explicit SplitterHandle(Qt::Orientation orientation, QSplitter *parent);

    /**
     * @brief 析构函数
     */
    ~SplitterHandle() override;

protected:
    /**
     * @brief 绘制事件处理
     * @param event 绘制事件对象
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief 鼠标进入事件
     * @param event 事件对象
     */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    /**
     * @brief 鼠标离开事件
     * @param event 事件对象
     */
    void leaveEvent(QEvent *event) override;

    /**
     * @brief 鼠标移动事件
     * @param event 鼠标事件对象
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief 鼠标按下事件
     * @param event 鼠标事件对象
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief 鼠标释放事件
     * @param event 鼠标事件对象
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    /**
     * @brief 绘制小方块（默认状态）
     * @param painter 绘制器对象
     */
    void drawHandle(QPainter *painter);

    /**
     * @brief 绘制完整分割线（悬停状态）
     * @param painter 绘制器对象
     */
    void drawFullLine(QPainter *painter);

    bool m_isHovered; ///< 鼠标悬停状态标志
    bool m_isPressed; ///< 鼠标按下状态标志
};

/**
 * @class Splitter
 * @brief 自定义分割器
 * @details 使用自定义手柄实现现代 UI 风格
 */
class QFLUENT_EXPORT Splitter : public QSplitter
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit Splitter(QWidget *parent = nullptr);

    /**
     * @brief 构造函数（带方向参数）
     * @param orientation 分割器方向（水平或垂直）
     * @param parent 父对象
     */
    explicit Splitter(Qt::Orientation orientation, QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~Splitter() override;

protected:
    /**
     * @brief 创建自定义手柄
     * @return 返回新创建的 SplitterHandle 对象指针
     */
    QSplitterHandle *createHandle() override;
};
