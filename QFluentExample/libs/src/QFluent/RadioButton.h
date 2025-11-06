#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include <QRadioButton>

#include "Property.h"

class QFLUENT_EXPORT RadioButton : public QRadioButton
{
    Q_OBJECT

public:
    explicit RadioButton(QWidget *parent = nullptr);
    explicit RadioButton(const QString &text, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void init();
    void drawIndicator(QPainter &painter);
    void drawText(QPainter &painter);
    void drawCircle(QPainter &painter, const QPoint &center, int radius, int thickness, const QColor &borderColor, const QColor &filledColor);

    QPoint indicatorPos_;
    bool isHover_;
};

#endif // RADIOBUTTON_H
