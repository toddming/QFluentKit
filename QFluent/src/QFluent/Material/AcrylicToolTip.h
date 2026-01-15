#pragma once

#include <QFrame>
#include "FluentGlobal.h"
#include "AcrylicWidget.h"
#include "QFluent/ToolTip.h"

class QPainterPath;
class QShowEvent;
class QPaintEvent;
class QString;
class QWidget;

/**
 * @brief 亚克力工具提示容器
 *
 * 提供带有亚克力毛玻璃效果的容器，特性包括：
 * - 继承 AcrylicWidget 以获得亚克力效果
 * - 自定义圆角裁剪路径
 * - 透明属性设置
 */
class QFLUENT_EXPORT AcrylicToolTipContainer : public QFrame, public AcrylicWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针，默认为 nullptr
     */
    explicit AcrylicToolTipContainer(QWidget *parent = nullptr);

    ~AcrylicToolTipContainer() override = default;

protected:
    /**
     * @brief 获取亚克力裁剪路径
     *
     * 返回圆角矩形裁剪路径，圆角半径为 3 像素
     * @return 圆角矩形裁剪路径
     */
    QPainterPath acrylicClipPath() const override;

    /**
     * @brief 绘制事件处理
     * @param event 绘制事件指针
     */
    void paintEvent(QPaintEvent *event) override;

private:
    // 禁用拷贝构造和赋值运算符
    AcrylicToolTipContainer(const AcrylicToolTipContainer &) = delete;
    AcrylicToolTipContainer& operator=(const AcrylicToolTipContainer &) = delete;
};

/**
 * @brief 亚克力工具提示
 *
 * 带有亚克力毛玻璃效果的工具提示窗口，特性包括：
 * - 自动抓取背景并应用模糊效果
 * - 在构造时创建亚克力容器并传递给父类
 * - 在显示时动态更新背景
 */
class QFLUENT_EXPORT AcrylicToolTip : public ToolTip
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param text 工具提示文本，默认为空字符串
     * @param parent 父窗口部件指针，默认为 nullptr
     */
    explicit AcrylicToolTip(const QString &text = QString(), QWidget *parent = nullptr);

    ~AcrylicToolTip() override = default;

    /**
     * @brief 获取亚克力容器
     * @return 亚克力容器指针
     */
    AcrylicToolTipContainer* acrylicContainer() const;

protected:
    /**
     * @brief 显示事件处理
     *
     * 在显示工具提示时抓取背景图像并应用到亚克力画刷
     * @param event 显示事件指针
     */
    void showEvent(QShowEvent *event) override;

private:
    AcrylicToolTipContainer *m_acrylicContainer;  ///< 亚克力容器指针

    // 禁用拷贝构造和赋值运算符
    AcrylicToolTip(const AcrylicToolTip &) = delete;
    AcrylicToolTip& operator=(const AcrylicToolTip &) = delete;
};

/**
 * @brief 亚克力工具提示过滤器
 *
 * 为窗口部件安装亚克力风格的工具提示，特性包括：
 * - 自动创建亚克力工具提示
 * - 延迟显示支持
 * - 事件过滤和管理
 */
class QFLUENT_EXPORT AcrylicToolTipFilter : public ToolTipFilter
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针
     * @param showDelay 显示延迟（毫秒），默认为 300ms
     * @param position 工具提示位置，默认为顶部
     */
    explicit AcrylicToolTipFilter(QWidget *parent,
                                  int showDelay = 300,
                                  ToolTipPosition position = ToolTipPosition::Top);

    ~AcrylicToolTipFilter() override = default;

protected:
    /**
     * @brief 创建工具提示
     *
     * 重写基类方法，创建亚克力工具提示实例
     * @return 亚克力工具提示指针
     */
    ToolTip* createToolTip() override;

private:
    // 禁用拷贝构造和赋值运算符
    AcrylicToolTipFilter(const AcrylicToolTipFilter &) = delete;
    AcrylicToolTipFilter& operator=(const AcrylicToolTipFilter &) = delete;
};
