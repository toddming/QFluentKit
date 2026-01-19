#pragma once
#include <QWidget>

#include "FluentTitleBar.h"

// ========== 窗口相关 ==========
enum class WindowEffect {
    Normal = 0x0000,
    DWMBlur = 0x0001,
    Acrylic,
    Mica,
    MicaAlt
};

class FluentTitleBar;

class FluentWidget : public QWidget
{
    Q_OBJECT
    Q_ENUM(WindowEffect)

public:
    explicit FluentWidget(QWidget *parent = nullptr);

    void setWindowButtonHint(WindowButtonHint hint, bool isEnable = true);
    void setWindowButtonHints(WindowButtonHints hints);
    WindowButtonHints windowButtonHints() const;

    void setWindowEffect(WindowEffect effect);
    WindowEffect windowEffect() const;

    FluentTitleBar *titleBar() const;
    void setHitTestVisible(QWidget *w, bool visible);

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    void setDarkTheme(bool dark);

    QObject *_windowAgent{nullptr};
    FluentTitleBar *_windowBar;
    WindowEffect _windowDisplayMode;
};
