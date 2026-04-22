#include "SpinBox.h"

#include <QMenu>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QApplication>

#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"

template class InlineSpinBoxBase<QSpinBox>;
template class InlineSpinBoxBase<QDoubleSpinBox>;
template class InlineSpinBoxBase<QTimeEdit>;
template class InlineSpinBoxBase<QDateTimeEdit>;
template class InlineSpinBoxBase<QDateEdit>;

// --- SpinButton ---

SpinButton::SpinButton(const FluentIconBase &icon, QWidget *parent)
    : QToolButton(parent)
    , m_fluentIcon(icon.clone())
{
    setFixedSize(sizeHint());
    setCursor(Qt::ArrowCursor);
    setIconSize(QSize(10, 10));
}

QSize SpinButton::sizeHint() const
{
    return QSize(31, 23);
}

void SpinButton::mousePressEvent(QMouseEvent *e)
{
    m_isPressed = true;
    QToolButton::mousePressEvent(e);
}

void SpinButton::mouseReleaseEvent(QMouseEvent *e)
{
    m_isPressed = false;
    QToolButton::mouseReleaseEvent(e);
}

void SpinButton::paintEvent(QPaintEvent *e)
{
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

// --- SpinBoxBase ---

SpinBoxBase::SpinBoxBase(QWidget *host)
    : m_parentWidget(host)
{
}

SpinBoxBase::~SpinBoxBase() = default;

void SpinBoxBase::setup()
{
    setupStyle();
    connectButtons();
}

void SpinBoxBase::setupStyle()
{
    if (!m_parentWidget)
        return;

    m_parentWidget->setProperty("transparent", true);
    StyleSheet::registerWidget(m_parentWidget, Fluent::ThemeStyle::SPIN_BOX);

    if (auto *spin = qobject_cast<QAbstractSpinBox *>(m_parentWidget))
        spin->setButtonSymbols(QAbstractSpinBox::NoButtons);

    m_parentWidget->setFixedHeight(33);
    m_parentWidget->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_parentWidget->setContextMenuPolicy(Qt::NoContextMenu);
}

void SpinBoxBase::addUpDownButtons()
{
    if (m_layout || !m_parentWidget)
        return;

    auto *le = lineEdit();
    if (!le)
        return;

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

void SpinBoxBase::connectButtons()
{
    if (auto *spin = qobject_cast<QAbstractSpinBox *>(m_parentWidget)) {
        QObject::connect(m_upButton, &QToolButton::clicked, spin, &QAbstractSpinBox::stepUp);
        QObject::connect(m_downButton, &QToolButton::clicked, spin, &QAbstractSpinBox::stepDown);
    }
}

void SpinBoxBase::setError(bool isError)
{
    if (!m_parentWidget || m_isError == isError)
        return;

    m_isError = isError;
    m_parentWidget->setProperty("isError", isError);
    m_parentWidget->style()->unpolish(m_parentWidget);
    m_parentWidget->style()->polish(m_parentWidget);
    m_parentWidget->update();
}

bool SpinBoxBase::isError() const
{
    return m_isError;
}

void SpinBoxBase::setSymbolVisible(bool visible)
{
    m_symbolVisible = visible;
    if (!m_parentWidget)
        return;

    if (m_upButton)
        m_upButton->setVisible(visible);
    if (m_downButton)
        m_downButton->setVisible(visible);
}

QWidget *SpinBoxBase::lineEdit() const
{
    if (auto *spin = qobject_cast<QAbstractSpinBox *>(m_parentWidget))
        return spin->findChild<QLineEdit *>();
    return nullptr;
}

void SpinBoxBase::drawFocusBorder(QPainter *painter, const QRect &rect)
{
    if (!painter)
        return;

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    QPainterPath path;
    int w = rect.width();
    int h = rect.height();
    path.addRoundedRect(QRectF(0, h - 10, w, 10), 5, 5);

    QPainterPath rectPath;
    rectPath.addRect(0, h - 10, w, 8);
    path = path.subtracted(rectPath);

    painter->fillPath(path, Theme::themeColor(Fluent::ThemeColor::PRIMARY));
}

// --- InlineSpinBoxBase ---

template <typename T>
InlineSpinBoxBase<T>::InlineSpinBoxBase(QWidget *parent)
    : T(parent)
    , m_helper(new SpinBoxBase(this))
{
    m_helper->addUpDownButtons();
    m_helper->setup();
}

template <typename T>
void InlineSpinBoxBase<T>::setError(bool isError)
{
    m_helper->setError(isError);
}

template <typename T>
bool InlineSpinBoxBase<T>::isError() const
{
    return m_helper->isError();
}

template <typename T>
void InlineSpinBoxBase<T>::setSymbolVisible(bool visible)
{
    m_helper->setSymbolVisible(visible);
}

template <typename T>
void InlineSpinBoxBase<T>::paintEvent(QPaintEvent *event)
{
    T::paintEvent(event);
    if (this->hasFocus() && !isError()) {
        QPainter painter(this);
        m_helper->drawFocusBorder(&painter, this->rect());
    }
}
