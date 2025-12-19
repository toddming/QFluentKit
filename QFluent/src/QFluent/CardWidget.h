#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include "Animation.h"
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QColor>

#include "FluentGlobal.h"

class QFLUENT_EXPORT CardWidget : public BackgroundAnimationWidget
{
    Q_OBJECT
    Q_PROPERTY(int borderRadius READ getBorderRadius WRITE setBorderRadius)

public:
    explicit CardWidget(QWidget *parent = nullptr);

    void setClickEnabled(bool isEnabled);
    bool isClickEnabled() const;

    int getBorderRadius() const;
    void setBorderRadius(int radius);

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    QColor _normalBackgroundColor() const override;
    QColor _hoverBackgroundColor() const override;
    QColor _pressedBackgroundColor() const override;

    bool _isClickEnabled;
    int _borderRadius;
};

#endif // CARDWIDGET_H
