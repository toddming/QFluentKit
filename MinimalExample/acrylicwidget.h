#pragma once

#include <QWidget>
#include <QString>

/**
 * @brief AcrylicWidget
 *
 * 支持 Windows 11 DWM 背景特效的 QWidget 派生类。
 * 提供 Acrylic、DWMBlur、Mica、MicaAlt 以及 Normal 五种窗口背景效果，
 * 并暴露深色 / 浅色主题切换接口。
 *
 * 背景色由类内部通过 setStyleSheet 自动管理：
 *   Normal 模式  —— 设置主题背景色（深色 #202020 / 浅色 #F0F4F9）
 *   特效模式     —— 设置 transparent
 *
 * 重写了 setStyleSheet()：用户样式表会被保存并与内部样式合并，
 * 内部背景色规则始终附加在用户样式之后，保证不被覆盖，
 * 且用户的其他规则正常生效。
 *
 * 依赖（仅 Windows）：
 *   qmake:  LIBS += -ldwmapi
 *   CMake:  target_link_libraries(... dwmapi)
 *
 * 用法示例：
 * @code
 *   AcrylicWidget *w = new AcrylicWidget();
 *   w->setWindowEffect(AcrylicWidget::WindowEffect::Mica);
 *   w->setDarkTheme(true);
 *   // 用户可正常设置样式表，内部背景色规则不受影响
 *   w->setStyleSheet("color: white; font-size: 14px;");
 *   w->show();
 * @endcode
 */
class AcrylicWidget : public QWidget
{
    Q_OBJECT

public:

    /** @brief 窗口背景特效类型 */
    enum class WindowEffect {
        Normal,     ///< 无特效，显示主题背景色（跨平台）
        Acrylic,    ///< 亚克力（毛玻璃）效果，DWMSBT_TRANSIENTWINDOW（仅 Win11+）
        DWMBlur,    ///< DWM 模糊，Win11 下等同于 Acrylic（仅 Win11+）
        Mica,       ///< Mica 效果，DWMSBT_MAINWINDOW（仅 Win11+）
        MicaAlt,    ///< Mica Alt 效果，DWMSBT_TABBEDWINDOW（仅 Win11+）
    };

    explicit AcrylicWidget(QWidget *parent = nullptr);
    ~AcrylicWidget() override = default;

    // ------------------------------------------------------------------ //
    //  特效 & 主题接口
    // ------------------------------------------------------------------ //

    /**
     * @brief 设置窗口背景特效
     * @param effect  目标特效类型
     * 若当前系统低于 Windows 11（Build 22000），effect 会被自动降级为 Normal。
     */
    void setWindowEffect(WindowEffect effect);

    /** @brief 返回当前生效的特效类型 */
    [[nodiscard]] WindowEffect windowEffect() const;

    /**
     * @brief 切换深色 / 浅色主题
     * @param dark  true = 深色，false = 浅色
     * 会立即刷新样式表背景色，Windows 下同步更新 DWM 标题栏颜色。
     */
    void setDarkTheme(bool dark);

    /** @brief 返回当前是否处于深色主题 */
    [[nodiscard]] bool isDarkTheme() const;

    /**
     * @brief 重写 setStyleSheet
     * 保存用户样式表，与内部背景色规则合并后一起应用。
     * 用户规则在前，内部背景色规则在后（优先级更高），保证背景色不被覆盖。
     */
    void setStyleSheet(const QString &styleSheet);

signals:
    void themeChanged(bool dark);
    void windowEffectChanged(WindowEffect effect);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void applyEffect(WindowEffect effect);

    /** 将 m_userStyleSheet 与当前背景色规则合并后应用到 QWidget */
    void applyStyleSheet();

    /** 根据当前模式和主题返回背景色 CSS 片段 */
    [[nodiscard]] QString bgStyleRule() const;

    WindowEffect m_effect        = WindowEffect::Normal;
    bool         m_isDark        = false;
    QString      m_userStyleSheet;          ///< 用户通过 setStyleSheet 设置的样式
};