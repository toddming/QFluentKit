#pragma once

#include "Animation.h"
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
    QColor normalBackgroundColor() const override;
    QColor hoverBackgroundColor() const override;
    QColor pressedBackgroundColor() const override;

    bool _isClickEnabled;
    int _borderRadius;
};

