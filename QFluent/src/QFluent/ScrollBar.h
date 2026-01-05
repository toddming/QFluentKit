#pragma once

#include <QScrollBar>

#include "FluentGlobal.h"

class QEvent;
class QMouseEvent;
class QWheelEvent;
class ScrollBarPrivate;
class QContextMenuEvent;
class QAbstractScrollArea;

class QFLUENT_EXPORT ScrollBar : public QScrollBar
{
    Q_OBJECT

public:
    explicit ScrollBar(QWidget* parent = nullptr);
    explicit ScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);
    explicit ScrollBar(QScrollBar* scrollBar, QAbstractScrollArea* parent = nullptr);
    ~ScrollBar() override;

    void setAnimationEnabled(bool enabled);
    bool isAnimationEnabled() const;

protected:
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    Q_DISABLE_COPY(ScrollBar)
    Q_DECLARE_PRIVATE(ScrollBar)
    QScopedPointer<ScrollBarPrivate> d_ptr;
};
