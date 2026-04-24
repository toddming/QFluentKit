#pragma once

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QToolButton>
#include <QPointer>
#include <QScopedPointer>

#include "FluentGlobal.h"
#include "FluentIcon.h"

class QHBoxLayout;
class QPainter;
class QRect;

class SpinButton : public QToolButton
{
    Q_OBJECT

public:
    explicit SpinButton(const QIcon &icon, QWidget *parent = nullptr);
    QSize sizeHint() const override;

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    QIcon m_icon;
    bool m_isPressed = false;
};

class QFLUENT_EXPORT SpinBoxBase
{
    Q_DISABLE_COPY(SpinBoxBase)

public:
    explicit SpinBoxBase(QWidget *host);
    ~SpinBoxBase();

    void setup();
    void addUpDownButtons();
    void setError(bool isError);
    bool isError() const;
    void setSymbolVisible(bool visible);
    void drawFocusBorder(QPainter *painter, const QRect &rect);
    QWidget *lineEdit() const;

private:
    QPointer<QWidget> m_parentWidget;
    QHBoxLayout *m_layout = nullptr;
    SpinButton *m_upButton = nullptr;
    SpinButton *m_downButton = nullptr;
    bool m_isError = false;
    bool m_symbolVisible = true;

    void setupStyle();
    void connectButtons();
};

template <typename T>
class QFLUENT_EXPORT InlineSpinBoxBase : public T
{
    static_assert(std::is_base_of_v<QAbstractSpinBox, T>,
                  "T must derive from QAbstractSpinBox");

public:
    explicit InlineSpinBoxBase(QWidget *parent = nullptr);

    void setError(bool isError);
    bool isError() const;
    void setSymbolVisible(bool visible);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QScopedPointer<SpinBoxBase> m_helper;
};

class QFLUENT_EXPORT SpinBox : public InlineSpinBoxBase<QSpinBox>
{
    Q_OBJECT

public:
    using Base = InlineSpinBoxBase<QSpinBox>;
    using Base::Base;
};

class QFLUENT_EXPORT DoubleSpinBox : public InlineSpinBoxBase<QDoubleSpinBox>
{
    Q_OBJECT

public:
    using Base = InlineSpinBoxBase<QDoubleSpinBox>;
    using Base::Base;
};

class QFLUENT_EXPORT TimeEdit : public InlineSpinBoxBase<QTimeEdit>
{
    Q_OBJECT

public:
    using Base = InlineSpinBoxBase<QTimeEdit>;
    using Base::Base;
};

class QFLUENT_EXPORT DateTimeEdit : public InlineSpinBoxBase<QDateTimeEdit>
{
    Q_OBJECT

public:
    using Base = InlineSpinBoxBase<QDateTimeEdit>;
    using Base::Base;
};

class QFLUENT_EXPORT DateEdit : public InlineSpinBoxBase<QDateEdit>
{
    Q_OBJECT

public:
    using Base = InlineSpinBoxBase<QDateEdit>;
    using Base::Base;
};
