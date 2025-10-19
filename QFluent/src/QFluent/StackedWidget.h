#ifndef STACKEDWIDGET_H
#define STACKEDWIDGET_H

#include <QStackedWidget>
#include <QEasingCurve>

#include "Property.h"

QT_BEGIN_NAMESPACE
class QPropertyAnimation;
QT_END_NAMESPACE

class QHBoxLayout;
class QGraphicsOpacityEffect;
class OpacityAniStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit OpacityAniStackedWidget(QWidget *parent = nullptr);

    void addWidget(QWidget *w);
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *w);

private slots:
    void __onAniFinished();

private:
    int __nextIndex;
    QList<QGraphicsOpacityEffect*> __effects;
    QList<QPropertyAnimation*> __anis;
};


// ============ PopUpAniInfo ============
struct PopUpAniInfo
{
    QWidget *widget;
    int deltaX;
    int deltaY;
    QPropertyAnimation *ani;

    PopUpAniInfo(QWidget *w, int dx, int dy, QPropertyAnimation *a)
        : widget(w), deltaX(dx), deltaY(dy), ani(a) {}
};


// ============ PopUpAniStackedWidget ============
class PopUpAniStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit PopUpAniStackedWidget(QWidget *parent = nullptr);

    void addWidget(QWidget *widget, int deltaX = 0, int deltaY = 76);
    void removeWidget(QWidget *widget);

    void setAnimationEnabled(bool isEnabled);
    bool isAnimationEnabled() const { return m_isAnimationEnabled; }

    void setCurrentIndex(int index, bool needPopOut = false, bool showNextWidgetDirectly = true,
                         int duration = 250, QEasingCurve easingCurve = QEasingCurve::OutQuad);
    void setCurrentWidget(QWidget *widget, bool needPopOut = false, bool showNextWidgetDirectly = true,
                          int duration = 250, QEasingCurve easingCurve = QEasingCurve::OutQuad);

signals:
    void aniFinished();
    void aniStart();

private:
    void __setAnimation(QPropertyAnimation *ani, const QPoint &startValue, const QPoint &endValue,
                        int duration, QEasingCurve easingCurve = QEasingCurve::Linear);
    void __onAniFinished();

    QList<PopUpAniInfo> m_aniInfos;
    bool m_isAnimationEnabled;
    int m_nextIndex;
    QPropertyAnimation *m_ani;
};


// ============ 最终暴露的 StackedWidget ============
class QFLUENT_EXPORT StackedWidget : public QFrame
{
    Q_OBJECT

public:
    explicit StackedWidget(QWidget *parent = nullptr);

    bool isAnimationEnabled() const;
    void setAnimationEnabled(bool isEnabled);

    void addWidget(QWidget *widget);
    void removeWidget(QWidget *widget);
    QWidget* widget(int index) const;
    void setCurrentWidget(QWidget *widget, bool popOut = true);
    void setCurrentIndex(int index, bool popOut = true);
    int currentIndex() const;
    QWidget* currentWidget() const;
    int indexOf(QWidget *widget) const;
    int count() const;

signals:
    void currentChanged(int index);

private:
    QHBoxLayout *hBoxLayout;
    PopUpAniStackedWidget *view;
};

#endif // STACKEDWIDGET_H
