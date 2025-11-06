#pragma once

#include <QWidget>
#include <QToolButton>


#include "Property.h"

class QLabel;
class QHBoxLayout;
class QPropertyAnimation;
class Indicator : public QToolButton {
    Q_OBJECT
    Q_PROPERTY(qreal sliderX READ sliderX WRITE setSliderX)

public:
    explicit Indicator(QWidget *parent = nullptr);

signals:
    void checkedChanged(bool checked);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void toggleSlider();

private:
    void drawBackground(QPainter &painter);
    void drawCircle(QPainter &painter);
    QColor backgroundColor() const;
    QColor borderColor() const;
    QColor sliderColor() const;

    qreal sliderX() const { return m_sliderX; }
    void setSliderX(qreal x);

    qreal m_sliderX;
    QPropertyAnimation *m_slideAni;
};

class QFLUENT_EXPORT SwitchButton : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString onText READ onText WRITE setOnText)
    Q_PROPERTY(QString offText READ offText WRITE setOffText)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)

public:
    enum class IndicatorPosition { Left, Right };

    explicit SwitchButton(QWidget *parent = nullptr, IndicatorPosition indicatorPos = IndicatorPosition::Left);
    explicit SwitchButton(const QString &text, QWidget *parent = nullptr, IndicatorPosition indicatorPos = IndicatorPosition::Left);

    bool isChecked() const;
    void setChecked(bool checked);
    QString text() const;
    void setText(const QString &text);
    QString onText() const;
    void setOnText(const QString &text);
    QString offText() const;
    void setOffText(const QString &text);
    int spacing() const;
    void setSpacing(int spacing);
    void setCheckedIndicatorColor(const QColor &light, const QColor &dark);
    void toggleChecked();

signals:
    void checkedChanged(bool checked);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void initWidget();
    void updateText();

    Indicator *m_indicator;
    QLabel *m_label;
    QHBoxLayout *m_hBox;
    IndicatorPosition m_indicatorPos;
    QString m_text;
    QString m_onText;
    QString m_offText;
    int m_spacing;
};
