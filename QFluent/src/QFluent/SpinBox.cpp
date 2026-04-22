#include "SpinBox.h"
#include <QMenu>
#include <QLineEdit>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QColor>
#include <QStyleOption>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"


template class InlineSpinBoxBase<QSpinBox>;
template class InlineSpinBoxBase<QDoubleSpinBox>;
template class InlineSpinBoxBase<QTimeEdit>;
template class InlineSpinBoxBase<QDateTimeEdit>;
template class InlineSpinBoxBase<QDateEdit>;


// --- SpinButton ---

SpinButton::SpinButton(const FluentIconBase &icon, QWidget* parent)
    : QToolButton(parent), m_fluentIcon(icon.clone()) {
    setFixedSize(sizeHint());
    setCursor(Qt::ArrowCursor);

    setFixedSize(31, 23);
    setIconSize(QSize(10, 10));
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::SPIN_BOX);
}

void SpinButton::mousePressEvent(QMouseEvent* e) {
    m_isPressed = true;
    QToolButton::mousePressEvent(e);
}

void SpinButton::mouseReleaseEvent(QMouseEvent* e) {
    m_isPressed = false;
    QToolButton::mouseReleaseEvent(e);
}

void SpinButton::paintEvent(QPaintEvent* e) {
    QToolButton::paintEvent(e);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (!isEnabled()) {
        painter.setOpacity(0.36);
    } else if (m_isPressed) {
        painter.setOpacity(0.7);
    }

    FluentIconUtils::drawIcon(*m_fluentIcon, &painter, QRectF(10, 6.5, 11, 11));
}

SpinBoxBase::SpinBoxBase(QWidget* host)
    : m_parentWidget(host)
{

}

SpinBoxBase::~SpinBoxBase() = default;

void SpinBoxBase::setup() {
    setupStyle();
    connectButtons();
}

void SpinBoxBase::setupStyle() {
    m_parentWidget->setProperty("transparent", true);
    StyleSheetManager::instance()->registerWidget(m_parentWidget, Fluent::ThemeStyle::SPIN_BOX);

    if (auto *spin = qobject_cast<QAbstractSpinBox*>(m_parentWidget)) {
        spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    }

    m_parentWidget->setFixedHeight(33);
    m_parentWidget->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_parentWidget->setContextMenuPolicy(Qt::NoContextMenu);
}

void SpinBoxBase::addUpDownButtons() {
    if (m_layout || !m_parentWidget) return;

    auto* le = lineEdit();
    if (!le) return;

    m_layout = new QHBoxLayout(m_parentWidget);
    m_layout->setContentsMargins(10, 4, 4, 4);
    m_layout->setSpacing(5);

    m_upButton = new SpinButton(FluentIcon(Fluent::IconType::UP), m_parentWidget);
    m_downButton = new SpinButton(FluentIcon(Fluent::IconType::DOWN), m_parentWidget);

    m_layout->addWidget(le);
    m_layout->addWidget(m_upButton);
    m_layout->addWidget(m_downButton);

    m_parentWidget->setLayout(m_layout);
    m_parentWidget->setMinimumWidth(60);
}

void SpinBoxBase::connectButtons() {
    if (auto* spin = qobject_cast<QSpinBox*>(m_parentWidget)) {
        QObject::connect(m_upButton, &QToolButton::clicked, spin, &QSpinBox::stepUp);
        QObject::connect(m_downButton, &QToolButton::clicked, spin, &QSpinBox::stepDown);
    } else if (auto* spin = qobject_cast<QDoubleSpinBox*>(m_parentWidget)) {
        QObject::connect(m_upButton, &QToolButton::clicked, spin, &QDoubleSpinBox::stepUp);
        QObject::connect(m_downButton, &QToolButton::clicked, spin, &QDoubleSpinBox::stepDown);
    } else if (auto* te = qobject_cast<QTimeEdit*>(m_parentWidget)) {
        QObject::connect(m_upButton, &QToolButton::clicked, te, &QTimeEdit::stepUp);
        QObject::connect(m_downButton, &QToolButton::clicked, te, &QTimeEdit::stepDown);
    } else if (auto* dte = qobject_cast<QDateTimeEdit*>(m_parentWidget)) {
        QObject::connect(m_upButton, &QToolButton::clicked, dte, &QDateTimeEdit::stepUp);
        QObject::connect(m_downButton, &QToolButton::clicked, dte, &QDateTimeEdit::stepDown);
    } else if (auto* de = qobject_cast<QDateEdit*>(m_parentWidget)) {
        QObject::connect(m_upButton, &QToolButton::clicked, de, &QDateEdit::stepUp);
        QObject::connect(m_downButton, &QToolButton::clicked, de, &QDateEdit::stepDown);
    }
}

void SpinBoxBase::setError(bool isError) {
    if (m_isError == isError) return;
    m_isError = isError;
    m_parentWidget->setProperty("isError", isError);
    m_parentWidget->style()->unpolish(m_parentWidget);
    m_parentWidget->style()->polish(m_parentWidget);
    m_parentWidget->update();
}

void SpinBoxBase::setSymbolVisible(bool visible) {
    m_symbolVisible = visible;
    if (m_upButton) m_upButton->setVisible(visible);
    if (m_downButton) m_downButton->setVisible(visible);
}

QWidget* SpinBoxBase::lineEdit() const {
    if (auto* spin = qobject_cast<QAbstractSpinBox*>(m_parentWidget)) {
        return spin->findChild<QLineEdit*>();
    }
    return nullptr;
}

void SpinBoxBase::drawFocusBorder(QPainter* painter, const QRect& rect) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    QPainterPath path;
    int w = rect.width();
    int h = rect.height();
    path.addRoundedRect(QRectF(0, h-10, w, 10), 5, 5);

    QPainterPath rectPath;
    rectPath.addRect(0, h-10, w, 8);
    path = path.subtracted(rectPath);
    painter->fillPath(path, Theme::themeColor(Fluent::ThemeColor::PRIMARY));
}



