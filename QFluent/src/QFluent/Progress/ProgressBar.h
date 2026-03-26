#pragma once

#include <QProgressBar>
#include <QPropertyAnimation>
#include "FluentGlobal.h"

class QFLUENT_EXPORT ProgressBar : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(bool useAni READ isUseAni WRITE setUseAni NOTIFY useAniChanged)
    Q_PROPERTY(float val READ value WRITE setVal NOTIFY valChanged)

public:
    explicit ProgressBar(QWidget *parent = nullptr, bool useAni = true);
    ~ProgressBar() override = default;

    bool isUseAni() const;
    void setUseAni(bool isUse);
    float value() const;
    void setVal(float v);

    void resume();
    void pause();
    void setPaused(bool isPaused);
    bool isPaused() const;
    void error();
    void setError(bool isError);
    bool isError() const;

    void setCustomBarColor(const QColor &light, const QColor &dark);
    void setCustomBackgroundColor(const QColor &light, const QColor &dark);

    QColor barColor() const;

signals:
    void useAniChanged(bool isUse);
    void valChanged(float val);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onValueChanged(int value);

private:
    float m_val;
    bool m_useAni;
    QColor m_lightBackgroundColor;
    QColor m_darkBackgroundColor;
    QColor m_lightBarColor;
    QColor m_darkBarColor;
    QPropertyAnimation m_ani;
    bool m_isPaused;
    bool m_isError;
};
