#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <QAbstractScrollArea>
#include <QScrollBar>

#include "Property.h"

class ScrollBarPrivate;
class QFLUENT_EXPORT ScrollBar : public QScrollBar
{
    Q_OBJECT
    Q_Q_CREATE(ScrollBar)
    Q_PROPERTY_CREATE_Q_H(bool, IsAnimation)
    Q_PROPERTY_CREATE_Q_H(qreal, SpeedLimit)
public:
    explicit ScrollBar(QWidget* parent = nullptr);
    explicit ScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);
    explicit ScrollBar(QScrollBar* originScrollBar, QAbstractScrollArea* parent = nullptr);
    ~ScrollBar();

Q_SIGNALS:
    Q_SIGNAL void rangeAnimationFinished();

protected:
    virtual bool event(QEvent* event) override;
    virtual bool eventFilter(QObject* watched, QEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
};

#endif // SCROLLBAR_H
