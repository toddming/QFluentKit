#pragma once

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QToolButton>
#include <QPointer>

#include "FluentGlobal.h"
#include "FluentIcon.h"

class QHBoxLayout;
class SpinButton : public QToolButton {
    Q_OBJECT

public:
    explicit SpinButton(const FluentIconBase &icon, QWidget* parent = nullptr);

    QSize sizeHint() const override { return QSize(31, 23); }

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    std::unique_ptr<FluentIconBase> m_fluentIcon;
    bool m_isPressed = false;
};

// ========================
// SpinBoxBase 辅助类
// ========================
class QFLUENT_EXPORT SpinBoxBase {
    Q_DISABLE_COPY(SpinBoxBase)
public:
    explicit SpinBoxBase(QWidget* host);
    ~SpinBoxBase();

    void setup();
    void addUpDownButtons();
    void setError(bool isError);
    bool isError() const { return m_isError; }
    void setSymbolVisible(bool visible);
    void drawFocusBorder(QPainter* painter, const QRect& rect);
    QWidget* lineEdit() const;

    // 转发函数（不再是纯虚）
    bool hasFocus() const { return m_parentWidget ? m_parentWidget->hasFocus() : false; }
    bool isEnabled() const { return m_parentWidget ? m_parentWidget->isEnabled() : false; }
    void update() { if (m_parentWidget) m_parentWidget->update(); }
    void setProperty(const char* name, bool value) { if (m_parentWidget) m_parentWidget->setProperty(name, value); }

private:
    QPointer<QWidget> m_parentWidget;
    QHBoxLayout* m_layout = nullptr;
    SpinButton* m_upButton = nullptr;
    SpinButton* m_downButton = nullptr;
    bool m_isError = false;
    bool m_symbolVisible = true;

    void setupStyle();
    void connectButtons();
};

// ========================
// 模板 Mixin：InlineSpinBoxBase
// ========================
template <typename T>
class QFLUENT_EXPORT InlineSpinBoxBase : public T {
    static_assert(
        std::is_base_of_v<QAbstractSpinBox, T> ||
            std::is_base_of_v<QDateTimeEdit, T>,
        "T must derive from QAbstractSpinBox or QDateTimeEdit"
        );


public:
    explicit InlineSpinBoxBase(QWidget* parent = nullptr)
        : T(parent), m_helper(new SpinBoxBase(this))
    {
        m_helper->addUpDownButtons();

        m_helper->setup();
    }

    void setError(bool isError) {
        m_helper->setError(isError);
    }

    bool isError() const {
        return m_helper->isError();
    }

    void setSymbolVisible(bool visible) {
        m_helper->setSymbolVisible(visible);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        T::paintEvent(event);
        if (this->hasFocus() && !isError()) {
            QPainter painter(this);
            m_helper->drawFocusBorder(&painter, this->rect());
        }
    }

private:
    QScopedPointer<SpinBoxBase> m_helper;
};

// ========================
// 具体控件类型（使用模板实例化）
// ========================
class QFLUENT_EXPORT SpinBox        : public InlineSpinBoxBase<QSpinBox>         { Q_OBJECT public: using Base = InlineSpinBoxBase<QSpinBox>; using Base::Base; };
class QFLUENT_EXPORT DoubleSpinBox  : public InlineSpinBoxBase<QDoubleSpinBox>   { Q_OBJECT public: using Base = InlineSpinBoxBase<QDoubleSpinBox>; using Base::Base; };
class QFLUENT_EXPORT TimeEdit       : public InlineSpinBoxBase<QTimeEdit>        { Q_OBJECT public: using Base = InlineSpinBoxBase<QTimeEdit>; using Base::Base; };
class QFLUENT_EXPORT DateTimeEdit   : public InlineSpinBoxBase<QDateTimeEdit>    { Q_OBJECT public: using Base = InlineSpinBoxBase<QDateTimeEdit>; using Base::Base; };
class QFLUENT_EXPORT DateEdit       : public InlineSpinBoxBase<QDateEdit>        { Q_OBJECT public: using Base = InlineSpinBoxBase<QDateEdit>; using Base::Base; };
