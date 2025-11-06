#pragma once

#include <QProgressBar>
#include <QPropertyAnimation>
#include "Property.h"

class QFLUENT_EXPORT ProgressBar : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(bool useAni READ isUseAni WRITE setUseAni NOTIFY useAniChanged)
    Q_PROPERTY(float val READ getVal WRITE setVal NOTIFY valChanged)

public:
    explicit ProgressBar(QWidget *parent = nullptr, bool useAni = true);
    ~ProgressBar() override = default;

    bool isUseAni() const;
    void setUseAni(bool isUse);
    float getVal() const;
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
    void _onValueChanged(int value);

private:
    float _val;
    bool _useAni;
    QColor lightBackgroundColor;
    QColor darkBackgroundColor;
    QColor _lightBarColor;
    QColor _darkBarColor;
    QPropertyAnimation ani;
    bool _isPaused;
    bool _isError;
};
