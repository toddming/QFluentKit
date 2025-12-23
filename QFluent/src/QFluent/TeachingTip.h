#pragma once

#include "Flyout.h"
#include "FluentGlobal.h"

#include <QWidget>

// 前向声明 - Qt 类
class QHBoxLayout;
class QVBoxLayout;
class QPaintEvent;
class QShowEvent;
class QCloseEvent;
class QPropertyAnimation;
class QGraphicsDropShadowEffect;
class QIcon;
class QPixmap;
class QPainter;

// 前向声明 - 自定义类
class FlyoutViewBase;
class TeachingTipManager;

// ============================================================================
// 枚举定义
// ============================================================================

// 尾巴位置枚举
enum class TeachingTipTailPosition {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    LEFT_TOP,
    LEFT_BOTTOM,
    RIGHT_TOP,
    RIGHT_BOTTOM,
    NONE
};

// 图片位置枚举
enum class ImagePosition {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};

// ============================================================================
// TeachingTip 视图
// ============================================================================
class QFLUENT_EXPORT TeachingTipView : public FlyoutView {
    Q_OBJECT
public:
    explicit TeachingTipView(const QString& title,
                            const QString& content,
                            const QIcon& icon = QIcon(),
                            const QPixmap& image = QPixmap(),
                            bool isClosable = true,
                            TeachingTipTailPosition tailPosition = TeachingTipTailPosition::BOTTOM,
                            QWidget* parent = nullptr);

    ~TeachingTipView() override = default;

    TeachingTipManager* manager() const { return m_manager; }
    QHBoxLayout* hBoxLayout() const { return m_hBoxLayout; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void adjustImage() override;
    void addImageToLayout() override;

private:
    TeachingTipManager* m_manager;
    QHBoxLayout* m_hBoxLayout;
};

// ============================================================================
// TeachingTip 气泡
// ============================================================================
class TeachTipBubble : public QWidget {
    Q_OBJECT
public:
    explicit TeachTipBubble(FlyoutViewBase* view,
                           TeachingTipTailPosition tailPosition = TeachingTipTailPosition::BOTTOM,
                           QWidget* parent = nullptr);

    ~TeachTipBubble() override = default;

    void setView(QWidget* view);
    FlyoutViewBase* view() const { return m_view; }
    TeachingTipManager* manager() const { return m_manager; }
    QHBoxLayout* hBoxLayout() const { return m_hBoxLayout; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    TeachingTipManager* m_manager;
    QHBoxLayout* m_hBoxLayout;
    FlyoutViewBase* m_view;
};

// ============================================================================
// TeachingTip 主类
// ============================================================================
class QFLUENT_EXPORT TeachingTip : public QWidget {
    Q_OBJECT
public:
    explicit TeachingTip(FlyoutViewBase* view,
                        QWidget* target,
                        int duration = 1000,
                        TeachingTipTailPosition tailPosition = TeachingTipTailPosition::BOTTOM,
                        QWidget* parent = nullptr,
                        bool isDeleteOnClose = true);

    ~TeachingTip() override = default;

    void setShadowEffect(int blurRadius = 35, const QPoint& offset = QPoint(0, 8));

    FlyoutViewBase* view() const { return m_bubble ? m_bubble->view() : nullptr; }
    void setView(FlyoutViewBase* view);
    void addWidget(QWidget* widget, int stretch = 0, Qt::Alignment align = Qt::AlignLeft);

    static TeachingTip* make(FlyoutViewBase* view,
                            QWidget* target,
                            int duration = 1000,
                            TeachingTipTailPosition tailPosition = TeachingTipTailPosition::BOTTOM,
                            QWidget* parent = nullptr,
                            bool isDeleteOnClose = true);

    static TeachingTip* create(QWidget* target,
                              const QString& title,
                              const QString& content,
                              const QIcon& icon = QIcon(),
                              const QPixmap& image = QPixmap(),
                              bool isClosable = true,
                              int duration = 1000,
                              TeachingTipTailPosition tailPosition = TeachingTipTailPosition::BOTTOM,
                              QWidget* parent = nullptr,
                              bool isDeleteOnClose = true);

    // 公共成员（保持与原代码兼容）
    QWidget* target;
    int duration;
    TeachingTipManager* m_manager;

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void fadeOut();

private:
    QHBoxLayout* m_hBoxLayout;
    QPropertyAnimation* m_opacityAni;
    TeachTipBubble* m_bubble;
    QGraphicsDropShadowEffect* m_shadowEffect;
    bool m_isDeleteOnClose;
};

// ============================================================================
// PopupTeachingTip
// ============================================================================
class PopupTeachingTip : public TeachingTip {
    Q_OBJECT
public:
    explicit PopupTeachingTip(FlyoutViewBase* view,
                             QWidget* target,
                             int duration = 1000,
                             TeachingTipTailPosition tailPosition = TeachingTipTailPosition::BOTTOM,
                             QWidget* parent = nullptr,
                             bool isDeleteOnClose = true);

    ~PopupTeachingTip() override = default;
};

// ============================================================================
// TeachingTip 管理器基类
// ============================================================================
class TeachingTipManager : public QObject {
    Q_OBJECT
public:
    explicit TeachingTipManager(QObject* parent = nullptr);
    ~TeachingTipManager() override = default;

    virtual void doLayout(TeachTipBubble* tip);
    virtual ImagePosition imagePosition() const;
    virtual QPoint position(TeachingTip* tip);
    virtual void draw(TeachTipBubble* tip, QPainter& painter);

    static TeachingTipManager* make(TeachingTipTailPosition position);

protected:
    virtual QPoint pos(TeachingTip* tip);
};

// ============================================================================
// 各种位置的管理器实现
// ============================================================================
class TopTailTeachingTipManager : public TeachingTipManager {
    Q_OBJECT
public:
    explicit TopTailTeachingTipManager(QObject* parent = nullptr);
    void doLayout(TeachTipBubble* tip) override;
    ImagePosition imagePosition() const override;
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class BottomTailTeachingTipManager : public TeachingTipManager {
    Q_OBJECT
public:
    explicit BottomTailTeachingTipManager(QObject* parent = nullptr);
    void doLayout(TeachTipBubble* tip) override;
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class LeftTailTeachingTipManager : public TeachingTipManager {
    Q_OBJECT
public:
    explicit LeftTailTeachingTipManager(QObject* parent = nullptr);
    void doLayout(TeachTipBubble* tip) override;
    ImagePosition imagePosition() const override;
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class RightTailTeachingTipManager : public TeachingTipManager {
    Q_OBJECT
public:
    explicit RightTailTeachingTipManager(QObject* parent = nullptr);
    void doLayout(TeachTipBubble* tip) override;
    ImagePosition imagePosition() const override;
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class TopLeftTailTeachingTipManager : public TopTailTeachingTipManager {
    Q_OBJECT
public:
    explicit TopLeftTailTeachingTipManager(QObject* parent = nullptr);
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class TopRightTailTeachingTipManager : public TopTailTeachingTipManager {
    Q_OBJECT
public:
    explicit TopRightTailTeachingTipManager(QObject* parent = nullptr);
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class BottomLeftTailTeachingTipManager : public BottomTailTeachingTipManager {
    Q_OBJECT
public:
    explicit BottomLeftTailTeachingTipManager(QObject* parent = nullptr);
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class BottomRightTailTeachingTipManager : public BottomTailTeachingTipManager {
    Q_OBJECT
public:
    explicit BottomRightTailTeachingTipManager(QObject* parent = nullptr);
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class LeftTopTailTeachingTipManager : public LeftTailTeachingTipManager {
    Q_OBJECT
public:
    explicit LeftTopTailTeachingTipManager(QObject* parent = nullptr);
    ImagePosition imagePosition() const override;
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class LeftBottomTailTeachingTipManager : public LeftTailTeachingTipManager {
    Q_OBJECT
public:
    explicit LeftBottomTailTeachingTipManager(QObject* parent = nullptr);
    ImagePosition imagePosition() const override;
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class RightTopTailTeachingTipManager : public RightTailTeachingTipManager {
    Q_OBJECT
public:
    explicit RightTopTailTeachingTipManager(QObject* parent = nullptr);
    ImagePosition imagePosition() const override;
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};

class RightBottomTailTeachingTipManager : public RightTailTeachingTipManager {
    Q_OBJECT
public:
    explicit RightBottomTailTeachingTipManager(QObject* parent = nullptr);
    ImagePosition imagePosition() const override;
    void draw(TeachTipBubble* tip, QPainter& painter) override;

protected:
    QPoint pos(TeachingTip* tip) override;
};
