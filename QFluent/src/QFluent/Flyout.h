#pragma once

#include "FluentGlobal.h"

#include <QWidget>
#include <QIcon>
#include <QPixmap>
#include <QColor>
#include <QPoint>

// 前向声明 - Qt 类
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QPaintEvent;
class QShowEvent;
class QCloseEvent;
class QPropertyAnimation;
class QParallelAnimationGroup;
class QGraphicsDropShadowEffect;

// 前向声明 - 自定义类
class ImageLabel;
class TransparentToolButton;
class FlyoutAnimationManager;

// ============================================================================
// 动画类型枚举
// ============================================================================
enum class FlyoutAnimationType {
    PULL_UP,
    DROP_DOWN,
    SLIDE_LEFT,
    SLIDE_RIGHT,
    FADE_IN,
    NONE
};

// ============================================================================
// 图标组件
// ============================================================================
class FlyoutIconWidget : public QWidget {
    Q_OBJECT
public:
    explicit FlyoutIconWidget(const QIcon& icon, QWidget* parent = nullptr);
    void setIcon(const QIcon& icon);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QIcon m_icon;
};

// ============================================================================
// Flyout 视图基类
// ============================================================================
class QFLUENT_EXPORT FlyoutViewBase : public QWidget {
    Q_OBJECT
public:
    explicit FlyoutViewBase(QWidget* parent = nullptr);
    virtual ~FlyoutViewBase() = default;

    virtual void addWidget(QWidget* widget, int stretch = 0, Qt::Alignment align = Qt::AlignLeft);

    QColor backgroundColor() const;
    QColor borderColor() const;

protected:
    void paintEvent(QPaintEvent* event) override;
};

// ============================================================================
// Flyout 视图
// ============================================================================
class QFLUENT_EXPORT FlyoutView : public FlyoutViewBase {
    Q_OBJECT
public:
    explicit FlyoutView(const QString& title,
                       const QString& content,
                       const QIcon& icon = QIcon(),
                       const QPixmap& image = QPixmap(),
                       bool isClosable = false,
                       QWidget* parent = nullptr);

    ~FlyoutView() override = default;

    void addWidget(QWidget* widget, int stretch = 0, Qt::Alignment align = Qt::AlignLeft) override;

    QVBoxLayout* widgetLayout() const { return m_widgetLayout; }
    QVBoxLayout* vBoxLayout() const { return m_vBoxLayout; }
    QHBoxLayout* viewLayout() const { return m_viewLayout; }
    ImageLabel* imageLabel() const { return m_imageLabel; }

signals:
    void closed();

protected:
    void showEvent(QShowEvent* event) override;
    virtual void adjustImage();
    virtual void addImageToLayout();

private:
    void initWidgets();
    void initLayout();
    void adjustText();

private:
    QString m_title;
    QString m_content;
    QIcon m_icon;
    QPixmap m_image;
    bool m_isClosable;

    QVBoxLayout* m_vBoxLayout;
    QHBoxLayout* m_viewLayout;
    QVBoxLayout* m_widgetLayout;

    QLabel* m_titleLabel;
    QLabel* m_contentLabel;
    FlyoutIconWidget* m_iconWidget;
    ImageLabel* m_imageLabel;
    TransparentToolButton* m_closeButton;
};

// ============================================================================
// Flyout 主类
// ============================================================================
class QFLUENT_EXPORT Flyout : public QWidget {
    Q_OBJECT
public:
    explicit Flyout(FlyoutViewBase* view,
                   QWidget* parent = nullptr,
                   bool isDeleteOnClose = true,
                   bool isMacInputMethodEnabled = false);

    ~Flyout() override;

    void setShadowEffect(int blurRadius = 35, const QPoint& offset = QPoint(0, 8));
    void exec(const QPoint& pos, FlyoutAnimationType aniType = FlyoutAnimationType::PULL_UP);
    void fadeOut();

    static Flyout* make(FlyoutViewBase* view,
                       QWidget* target = nullptr,
                       QWidget* parent = nullptr,
                       FlyoutAnimationType aniType = FlyoutAnimationType::PULL_UP,
                       bool isDeleteOnClose = true,
                       bool isMacInputMethodEnabled = false);

    static Flyout* create(const QString& title,
                         const QString& content,
                         const QIcon& icon = QIcon(),
                         const QPixmap& image = QPixmap(),
                         bool isClosable = false,
                         QWidget* target = nullptr,
                         QWidget* parent = nullptr,
                         FlyoutAnimationType aniType = FlyoutAnimationType::PULL_UP,
                         bool isDeleteOnClose = true,
                         bool isMacInputMethodEnabled = false);

    FlyoutViewBase* view() const { return m_view; }

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    FlyoutViewBase* m_view;
    QHBoxLayout* m_hBoxLayout;
    FlyoutAnimationManager* m_aniManager;
    QGraphicsDropShadowEffect* m_shadowEffect;
    QPropertyAnimation* m_fadeOutAni;
    bool m_isDeleteOnClose;
    bool m_isMacInputMethodEnabled;
};

// ============================================================================
// 动画管理器基类
// ============================================================================
class FlyoutAnimationManager : public QObject {
    Q_OBJECT
public:
    explicit FlyoutAnimationManager(Flyout* flyout);
    ~FlyoutAnimationManager() override = default;

    virtual void exec(const QPoint& pos);
    virtual QPoint position(QWidget* target);

    static FlyoutAnimationManager* make(FlyoutAnimationType aniType, Flyout* flyout);

protected:
    QPoint adjustPosition(const QPoint& pos);

protected:
    Flyout* m_flyout;
    QParallelAnimationGroup* m_aniGroup;
    QPropertyAnimation* m_slideAni;
    QPropertyAnimation* m_opacityAni;
};

// ============================================================================
// 各种动画管理器实现
// ============================================================================
class PullUpFlyoutAnimationManager : public FlyoutAnimationManager {
    Q_OBJECT
public:
    explicit PullUpFlyoutAnimationManager(Flyout* flyout);
    QPoint position(QWidget* target) override;
    void exec(const QPoint& pos) override;
};

class DropDownFlyoutAnimationManager : public FlyoutAnimationManager {
    Q_OBJECT
public:
    explicit DropDownFlyoutAnimationManager(Flyout* flyout);
    QPoint position(QWidget* target) override;
    void exec(const QPoint& pos) override;
};

class SlideLeftFlyoutAnimationManager : public FlyoutAnimationManager {
    Q_OBJECT
public:
    explicit SlideLeftFlyoutAnimationManager(Flyout* flyout);
    QPoint position(QWidget* target) override;
    void exec(const QPoint& pos) override;
};

class SlideRightFlyoutAnimationManager : public FlyoutAnimationManager {
    Q_OBJECT
public:
    explicit SlideRightFlyoutAnimationManager(Flyout* flyout);
    QPoint position(QWidget* target) override;
    void exec(const QPoint& pos) override;
};

class FadeInFlyoutAnimationManager : public FlyoutAnimationManager {
    Q_OBJECT
public:
    explicit FadeInFlyoutAnimationManager(Flyout* flyout);
    QPoint position(QWidget* target) override;
    void exec(const QPoint& pos) override;
};

class DummyFlyoutAnimationManager : public FlyoutAnimationManager {
    Q_OBJECT
public:
    explicit DummyFlyoutAnimationManager(Flyout* flyout);
    QPoint position(QWidget* target) override;
    void exec(const QPoint& pos) override;
};
