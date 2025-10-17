#pragma once

#include <QProgressBar>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QPainter>
#include <QColor>

class IndeterminateProgressBar : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(bool useAni READ isUseAni WRITE setUseAni NOTIFY useAniChanged)
    Q_PROPERTY(float shortPos READ shortPos WRITE setShortPos NOTIFY shortPosChanged)
    Q_PROPERTY(float longPos READ longPos WRITE setLongPos NOTIFY longPosChanged)

public:
    explicit IndeterminateProgressBar(QWidget *parent = nullptr, bool start = true);
    ~IndeterminateProgressBar() override = default;

    bool isUseAni() const;
    void setUseAni(bool isUse);
    float shortPos() const;
    void setShortPos(float p);
    float longPos() const;
    void setLongPos(float p);

    void start();
    void stop();
    bool isStarted() const;
    void pause();
    void resume();
    void setPaused(bool isPaused);
    bool isPaused() const;
    void error();
    void setError(bool isError);
    bool isError() const;

    void setCustomBarColor(const QColor &light, const QColor &dark);
    QColor barColor() const;

signals:
    void useAniChanged(bool isUse);
    void shortPosChanged(float p);
    void longPosChanged(float p);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float _shortPos;
    float _longPos;
    QPropertyAnimation shortBarAni;
    QPropertyAnimation longBarAni;
    QParallelAnimationGroup aniGroup;
    QSequentialAnimationGroup longBarAniGroup;
    QColor _lightBarColor;
    QColor _darkBarColor;
    bool _isError;
};
