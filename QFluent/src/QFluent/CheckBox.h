#pragma once

#include <QCheckBox>

#include "Property.h"

class CheckBoxPrivate;
class QFLUENT_EXPORT CheckBox : public QCheckBox {
    Q_OBJECT
    Q_Q_CREATE(CheckBox)

public:
    explicit CheckBox(QWidget *parent = nullptr);
    CheckBox(const QString &text, QWidget *parent = nullptr);
    ~CheckBox();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};
