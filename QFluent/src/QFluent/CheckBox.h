#pragma once

#include <QCheckBox>

#include "FluentGlobal.h"

class QEnterEvent;
class CheckBoxPrivate;
class QFLUENT_EXPORT CheckBox : public QCheckBox {
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, CheckBox)

public:
    explicit CheckBox(QWidget *parent = nullptr);
    CheckBox(const QString &text, QWidget *parent = nullptr);
    ~CheckBox();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QScopedPointer<CheckBoxPrivate> d_ptr;
};
