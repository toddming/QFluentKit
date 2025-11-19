#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include <QRadioButton>

#include "FluentGlobal.h"

class QFLUENT_EXPORT RadioButton : public QRadioButton
{
    Q_OBJECT

public:
    explicit RadioButton(QWidget *parent = nullptr);
    explicit RadioButton(const QString &text, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
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
