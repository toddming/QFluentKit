#pragma once

#include <QObject>

// 前置声明
class QAbstractScrollArea;
class QPropertyAnimation;
class QScrollBar;
class QTimer;
class ScrollBar;

class ScrollBarPrivate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int targetMaximum READ targetMaximum WRITE setTargetMaximum NOTIFY targetMaximumChanged)

public:
    explicit ScrollBarPrivate(ScrollBar* parent);
    ~ScrollBarPrivate();

    // 属性访问器
    void setTargetMaximum(int maximum);
    int targetMaximum() const;

public Q_SLOTS:
    void onRangeChanged(int min, int max);

Q_SIGNALS:
    void targetMaximumChanged();

private:
    friend class ScrollBar;
    ScrollBar* q_ptr = nullptr;
    Q_DECLARE_PUBLIC(ScrollBar)

    // 属性成员
    bool m_isAnimationEnabled{false};
    qreal m_speedLimit{20.0};
    int m_targetMaximum{0};
    int m_scrollValue{-1};
    bool m_isExpand{false};

    // 指针成员（由Qt父子关系管理）
    QScrollBar* m_originScrollBar{nullptr};
    QAbstractScrollArea* m_originScrollArea{nullptr};
    QTimer* m_expandTimer{nullptr};
    QPropertyAnimation* m_slideSmoothAnimation{nullptr};

    // 私有方法
    void scroll(Qt::KeyboardModifiers modifiers, int delta);
    int pixelPosToRangeValue(int pos) const;
    void initAllConfig();
    void handleScrollBarValueChanged(QScrollBar* scrollBar, int value);
    void handleScrollBarRangeChanged(int min, int max);
    void handleScrollBarGeometry();
};
