#pragma once

#include <QScrollArea>

#include "FluentGlobal.h"

class ScrollBar;
class QScrollerProperties;

class QFLUENT_EXPORT ScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit ScrollArea(QWidget *parent = nullptr);

    void enableTransparentBackground();
    void setViewportMargins(int left, int top, int right, int bottom);

    void setGrabGestureEnabled(bool isEnable);

    void setOvershootEnabled(Qt::Orientation orientation, bool isEnable);
    bool isOvershootEnabled(Qt::Orientation orientation) const;

    void setAnimationEnabled(Qt::Orientation orientation, bool isEnable);
    bool isAnimationEnabled(Qt::Orientation orientation) const;

private:
    ScrollBar* getScrollBar(Qt::Orientation orientation) const;
    static void configureScrollerProperties(
        QScrollerProperties& properties,
        qreal gestureRecognitionTime = 0.5
        );

};

// ===================== SingleDirectionScrollArea =====================
class QFLUENT_EXPORT SingleDirectionScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit SingleDirectionScrollArea(QWidget *parent = nullptr, Qt::Orientation orient = Qt::Vertical);

    void enableTransparentBackground();
    void setViewportMargins(int left, int top, int right, int bottom);

    // 重写策略，强制另一方向永远不显示滚动条
    void setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy);

protected:
    void wheelEvent(QWheelEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    Qt::Orientation m_orient;
};



