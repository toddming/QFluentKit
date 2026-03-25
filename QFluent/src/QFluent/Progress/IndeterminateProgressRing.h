#pragma once

#include <QProgressBar>

#include "FluentGlobal.h"

class QPropertyAnimation;
class QParallelAnimationGroup;
class QSequentialAnimationGroup;
class QFLUENT_EXPORT IndeterminateProgressRing : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(int startAngle READ startAngle WRITE setStartAngle)
    Q_PROPERTY(int spanAngle READ spanAngle WRITE setSpanAngle)
    Q_PROPERTY(int strokeWidth READ strokeWidth WRITE setStrokeWidth)

public:
    explicit IndeterminateProgressRing(QWidget *parent = nullptr, bool start = true);
    ~IndeterminateProgressRing();

    int startAngle() const;
    void setStartAngle(int angle);

    int spanAngle() const;
    void setSpanAngle(int angle);

    int strokeWidth() const;
    void setStrokeWidth(int w);

    void start();
    void stop();

    QColor lightBarColor() const;
    QColor darkBarColor() const;

    void setCustomBarColor(const QColor &light, const QColor &dark);
    void setCustomBackgroundColor(const QColor &light, const QColor &dark);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor lightBackgroundColor;
    QColor darkBackgroundColor;
    QColor m_lightBarColor;
    QColor m_darkBarColor;
    int m_strokeWidth;
    int m_startAngle;
    int m_spanAngle;

    QPropertyAnimation *startAngleAni1;
    QPropertyAnimation *startAngleAni2;
    QPropertyAnimation *spanAngleAni1;
    QPropertyAnimation *spanAngleAni2;

    QSequentialAnimationGroup *startAngleAniGroup;
    QSequentialAnimationGroup *spanAngleAniGroup;
    QParallelAnimationGroup *aniGroup;

};
