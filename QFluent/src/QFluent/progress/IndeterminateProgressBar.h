#ifndef INDETERMINATEPROGRESSBAR_H
#define INDETERMINATEPROGRESSBAR_H

#include <QProgressBar>
#include "FluentGlobal.h"

class QPropertyAnimation;
class QParallelAnimationGroup;
class QSequentialAnimationGroup;
class QFLUENT_EXPORT IndeterminateProgressBar : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(qreal shortPos READ shortPos WRITE setShortPos)
    Q_PROPERTY(qreal longPos READ longPos WRITE setLongPos)

public:
    explicit IndeterminateProgressBar(QWidget *parent = nullptr, bool start = true);
    ~IndeterminateProgressBar();

    QColor lightBarColor() const;
    QColor darkBarColor() const;

    void setCustomBarColor(const QColor &light, const QColor &dark);

    qreal shortPos() const;
    void setShortPos(qreal pos);

    qreal longPos() const;
    void setLongPos(qreal pos);

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

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    QColor barColor() const;

    qreal m_shortPos = 0;
    qreal m_longPos = 0;

    QPropertyAnimation *m_shortBarAni = nullptr;
    QPropertyAnimation *m_longBarAni = nullptr;

    QColor m_lightBarColor;
    QColor m_darkBarColor;

    bool m_isError = false;

    QParallelAnimationGroup *m_aniGroup = nullptr;
    QSequentialAnimationGroup *m_longBarAniGroup = nullptr;
};

#endif // INDETERMINATEPROGRESSBAR_H
