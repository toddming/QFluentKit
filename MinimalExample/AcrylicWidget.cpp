#include "AcrylicWidget.h"

#include <QShowEvent>

#ifdef Q_OS_WIN
#  include <QOperatingSystemVersion>
#  include <QColor>
#  include <windows.h>
#  include <dwmapi.h>
#  pragma comment(lib, "dwmapi.lib")
#endif

// ────────────────────────────────────────────────────────────────────────────
//  DWM 属性常量（SDK 版本较旧时可能未定义）
// ────────────────────────────────────────────────────────────────────────────
#ifdef Q_OS_WIN
#  ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#    define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#  endif
#  ifndef DWMWA_CAPTION_COLOR
#    define DWMWA_CAPTION_COLOR 35
#  endif
#  ifndef DWMWA_SYSTEMBACKDROP_TYPE
#    define DWMWA_SYSTEMBACKDROP_TYPE 38
#  endif
#  ifndef DWMSBT_NONE
#    define DWMSBT_NONE            1
#    define DWMSBT_MAINWINDOW      2   // Mica
#    define DWMSBT_TRANSIENTWINDOW 3   // Acrylic
#    define DWMSBT_TABBEDWINDOW    4   // Mica Alt
#  endif
#endif

// ────────────────────────────────────────────────────────────────────────────
//  构造 / 析构
// ────────────────────────────────────────────────────────────────────────────

AcrylicWidget::AcrylicWidget(QWidget *parent)
    : QWidget(parent)
{
    applyStyleSheet();
}

// ────────────────────────────────────────────────────────────────────────────
//  公开接口
// ────────────────────────────────────────────────────────────────────────────

void AcrylicWidget::setWindowEffect(WindowEffect effect)
{
    if (m_effect == effect)
        return;

    m_effect = effect;
    applyEffect(m_effect);
    emit windowEffectChanged(m_effect);
}

AcrylicWidget::WindowEffect AcrylicWidget::windowEffect() const
{
    return m_effect;
}

void AcrylicWidget::setDarkTheme(bool dark)
{
    if (m_isDark == dark)
        return;

    m_isDark = dark;
    applyEffect(m_effect);
    emit themeChanged(m_isDark);
}

bool AcrylicWidget::isDarkTheme() const
{
    return m_isDark;
}

void AcrylicWidget::setStyleSheet(const QString &styleSheet)
{
    // 保存用户样式表，重新合并应用
    m_userStyleSheet = styleSheet;
    applyStyleSheet();
}

// ────────────────────────────────────────────────────────────────────────────
//  受保护事件
// ────────────────────────────────────────────────────────────────────────────

void AcrylicWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    applyEffect(m_effect);
}

// ────────────────────────────────────────────────────────────────────────────
//  私有实现
// ────────────────────────────────────────────────────────────────────────────

QString AcrylicWidget::bgStyleRule() const
{
    // 特效模式透明，Normal 模式显示主题色
    const QString bgColor = (m_effect != WindowEffect::Normal)
                                ? QStringLiteral("transparent")
                                : (m_isDark ? QStringLiteral("#202020")
                                            : QStringLiteral("#F0F4F9"));

    // 使用 !important 确保内部背景色规则优先级最高，不被用户规则中的
    // background / background-color 覆盖
    return QStringLiteral("AcrylicWidget { background-color: %1 !important; }").arg(bgColor);
}

void AcrylicWidget::applyStyleSheet()
{
    // 用户样式在前（保证用户其他规则正常生效），
    // 内部背景色规则在后并附加 !important，确保背景色始终由类自身控制
    QWidget::setStyleSheet(m_userStyleSheet + bgStyleRule());
}

void AcrylicWidget::applyEffect(WindowEffect effect)
{
#ifdef Q_OS_WIN
    // ── 1. 版本检测：低于 Windows 11 (Build 22000) 降级为 Normal ──────────
    const auto current = QOperatingSystemVersion::current();
    const bool isWin11 = (current >= QOperatingSystemVersion(
                              QOperatingSystemVersion::Windows, 10, 0, 22000));
    if (!isWin11)
        effect = WindowEffect::Normal;
#endif

    // ── 2. 刷新背景色样式表 ────────────────────────────────────────────────
    applyStyleSheet();

#ifdef Q_OS_WIN
    // ── 3. 获取原生窗口句柄 ────────────────────────────────────────────────
    const HWND hwnd = reinterpret_cast<HWND>(this->winId());
    if (!hwnd) {
        qWarning("AcrylicWidget: invalid window handle");
        return;
    }

    // ── 4. 深色模式标题栏 ──────────────────────────────────────────────────
    const BOOL darkMode = m_isDark ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttribute(hwnd,
                                       DWMWA_USE_IMMERSIVE_DARK_MODE,
                                       &darkMode, sizeof(darkMode));
    if (FAILED(hr))
        qDebug("AcrylicWidget: failed to set dark mode: 0x%08lX",
               static_cast<unsigned long>(hr));

    // ── 5. 根据效果类型配置 DWM ────────────────────────────────────────────
    bool needExtendFrame       = true;
    bool needResetCaptionColor = true;

    switch (effect) {

    case WindowEffect::Acrylic:
    case WindowEffect::DWMBlur: {
        int backdropType = DWMSBT_TRANSIENTWINDOW;
        hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                   &backdropType, sizeof(backdropType));
        if (FAILED(hr))
            qWarning("AcrylicWidget: failed to set Acrylic/DWMBlur: 0x%08lX",
                     static_cast<unsigned long>(hr));
        break;
    }

    case WindowEffect::Mica: {
        int backdropType = DWMSBT_MAINWINDOW;
        hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                   &backdropType, sizeof(backdropType));
        if (FAILED(hr))
            qWarning("AcrylicWidget: failed to set Mica: 0x%08lX",
                     static_cast<unsigned long>(hr));
        break;
    }

    case WindowEffect::MicaAlt: {
        int backdropType = DWMSBT_TABBEDWINDOW;
        hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                   &backdropType, sizeof(backdropType));
        if (FAILED(hr))
            qWarning("AcrylicWidget: failed to set MicaAlt: 0x%08lX",
                     static_cast<unsigned long>(hr));
        break;
    }

    case WindowEffect::Normal: {
        int backdropType = DWMSBT_NONE;
        hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE,
                                   &backdropType, sizeof(backdropType));
        if (FAILED(hr))
            qWarning("AcrylicWidget: failed to reset backdrop: 0x%08lX",
                     static_cast<unsigned long>(hr));

        MARGINS margins = {0, 0, 0, 0};
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        // 标题栏颜色跟随主题色
        const QColor color(m_isDark ? QStringLiteral("#202020")
                                    : QStringLiteral("#F0F4F9"));
        const COLORREF captionColor = RGB(color.red(), color.green(), color.blue());
        hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR,
                                   &captionColor, sizeof(captionColor));
        if (FAILED(hr))
            qDebug("AcrylicWidget: failed to set caption color: 0x%08lX",
                   static_cast<unsigned long>(hr));

        needExtendFrame       = false;
        needResetCaptionColor = false;
        break;
    }

    default:
        return;
    }

    // ── 6. 特效模式：还原标题栏颜色为系统默认 ────────────────────────────────
    if (needResetCaptionColor) {
        constexpr COLORREF defaultColor = 0xFFFFFFFF;
        hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR,
                                   &defaultColor, sizeof(defaultColor));
        if (FAILED(hr))
            qDebug("AcrylicWidget: failed to reset caption color: 0x%08lX",
                   static_cast<unsigned long>(hr));
    }

    // ── 7. 特效模式：将 DWM 边框扩展到整个客户区 ─────────────────────────────
    if (needExtendFrame) {
        MARGINS margins = {-1, -1, -1, -1};
        hr = DwmExtendFrameIntoClientArea(hwnd, &margins);
        if (FAILED(hr))
            qWarning("AcrylicWidget: failed to extend frame: 0x%08lX",
                     static_cast<unsigned long>(hr));
    }

#else
    if (effect != WindowEffect::Normal)
        qWarning("AcrylicWidget: DWM effects are only supported on Windows 11+.");
#endif
}