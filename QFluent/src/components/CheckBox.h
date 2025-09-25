#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QCheckBox>
#include <QPainter>
#include <QColor>
#include <QStyleOptionButton>
#include <QWidget>

#include "../common/Theme.h"
#include "../Property.h"
#include "FluentIcon.h"


class QFLUENT_EXPORT CheckBox : public QCheckBox {
    Q_OBJECT

public:
    explicit CheckBox(QWidget *parent = nullptr);
    CheckBox(const QString &text, QWidget *parent = nullptr);

    enum class CheckBoxState {
        NORMAL,
        HOVER,
        PRESSED,
        CHECKED,
        CHECKED_HOVER,
        CHECKED_PRESSED,
        DISABLED,
        CHECKED_DISABLED
    };


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QColor borderColor() const;
    QColor backgroundColor() const;
    CheckBoxState state() const;

    bool isPressed = false;
    bool isHover = false;
};

#endif // CHECKBOX_H
