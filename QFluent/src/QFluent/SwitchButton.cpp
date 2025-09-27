#include "SwitchButton.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

#include "Theme.h"

Indicator::Indicator(QWidget *parent) : QToolButton(parent), m_sliderX(11) {
    setCheckable(true);
    setFixedSize(42, 22);

    m_slideAni = new QPropertyAnimation(this, "sliderX", this);
    m_slideAni->setDuration(120);

    connect(this, &QToolButton::toggled, this, &Indicator::toggleSlider);
}

void Indicator::mouseReleaseEvent(QMouseEvent *event) {
    QToolButton::mouseReleaseEvent(event);
    emit checkedChanged(isChecked());
}

void Indicator::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    drawBackground(painter);
    drawCircle(painter);
}

void Indicator::toggleSlider() {
    m_slideAni->setEndValue(isChecked() ? 31 : 11);
    m_slideAni->start();
}

void Indicator::drawBackground(QPainter &painter) {
    qreal r = height() / 2.0;
    painter.setPen(borderColor());
    painter.setBrush(backgroundColor());
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), r, r);
}

void Indicator::drawCircle(QPainter &painter) {
    painter.setPen(Qt::NoPen);
    painter.setBrush(sliderColor());
    qreal centerY = height() / 2.0;
    painter.drawEllipse(QPointF(m_sliderX, centerY), 6, 6);
}

QColor Indicator::backgroundColor() const {
    bool isDark = Theme::instance()->isDarkMode();

    if (isChecked()) {
        QColor color = Theme::instance()->themeColor();
        if (!isEnabled()) {
            return isDark ? QColor(255, 255, 255, 41) : QColor(0, 0, 0, 56);
        }
        if (property("isPressed").toBool()) {
            return Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_2);
        }
        if (property("isHover").toBool()) {
            return Theme::instance()->themeColor(ThemeType::ThemeColor::LIGHT_1);
        }
        return color;
    } else {
        if (!isEnabled()) {
            return QColor(0, 0, 0, 0);
        }
        if (property("isPressed").toBool()) {
            return isDark ? QColor(255, 255, 255, 18) : QColor(0, 0, 0, 23);
        }
        if (property("isHover").toBool()) {
            return isDark ? QColor(255, 255, 255, 10) : QColor(0, 0, 0, 15);
        }
        return QColor(0, 0, 0, 0);
    }
}

QColor Indicator::borderColor() const {
    bool isDark = Theme::instance()->isDarkMode();

    if (isChecked()) {
        return isEnabled() ? backgroundColor() : QColor(0, 0, 0, 0);
    } else {
        if (isEnabled()) {
            return isDark ? QColor(255, 255, 255, 153) : QColor(0, 0, 0, 133);
        }
        return isDark ? QColor(255, 255, 255, 41) : QColor(0, 0, 0, 56);
    }
}

QColor Indicator::sliderColor() const {
    bool isDark = Theme::instance()->isDarkMode();

    if (isChecked()) {
        if (isEnabled()) {
            return isDark ? Qt::black : Qt::white;
        }
        return isDark ? QColor(255, 255, 255, 77) : QColor(255, 255, 255);
    } else {
        if (isEnabled()) {
            return isDark ? QColor(255, 255, 255, 201) : QColor(0, 0, 0, 156);
        }
        return isDark ? QColor(255, 255, 255, 96) : QColor(0, 0, 0, 91);
    }
}

void Indicator::setSliderX(qreal x) {
    m_sliderX = qMax(x, 11.0);
    update();
}

SwitchButton::SwitchButton(QWidget *parent, IndicatorPosition indicatorPos)
    : QWidget(parent), m_indicatorPos(indicatorPos), m_text(tr("Off")), m_onText(tr("On")), m_offText(tr("Off")), m_spacing(12) {
    initWidget();
}

SwitchButton::SwitchButton(const QString &text, QWidget *parent, IndicatorPosition indicatorPos)
    : QWidget(parent), m_indicatorPos(indicatorPos), m_text(text), m_onText(tr("On")), m_offText(text), m_spacing(12) {
    initWidget();
}

void SwitchButton::initWidget() {
    setAttribute(Qt::WA_StyledBackground);
    installEventFilter(this);
    setFixedHeight(22);

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::SWITCH_BUTTON);

    m_hBox = new QHBoxLayout(this);
    m_indicator = new Indicator(this);
    m_label = new QLabel(m_text, this);

    m_hBox->setSpacing(m_spacing);
    m_hBox->setContentsMargins(2, 0, 0, 0);

    if (m_indicatorPos == IndicatorPosition::Left) {
        m_hBox->addWidget(m_indicator);
        m_hBox->addWidget(m_label);
        m_hBox->setAlignment(Qt::AlignLeft);
    } else {
        m_hBox->addWidget(m_label, 0, Qt::AlignRight);
        m_hBox->addWidget(m_indicator, 0, Qt::AlignRight);
        m_hBox->setAlignment(Qt::AlignRight);
    }

    connect(m_indicator, &Indicator::toggled, this, &SwitchButton::updateText);
    connect(m_indicator, &Indicator::toggled, this, &SwitchButton::checkedChanged);
}

bool SwitchButton::eventFilter(QObject *obj, QEvent *event) {
    if (obj == this && isEnabled()) {
        if (event->type() == QEvent::MouseButtonPress) {
            m_indicator->setProperty("isPressed", true);
            m_indicator->update();
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_indicator->setProperty("isPressed", false);
            m_indicator->toggle();
        } else if (event->type() == QEvent::Enter) {
            m_indicator->setProperty("isHover", true);
            m_indicator->update();
        } else if (event->type() == QEvent::Leave) {
            m_indicator->setProperty("isHover", false);
            m_indicator->update();
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool SwitchButton::isChecked() const {
    return m_indicator->isChecked();
}

void SwitchButton::setChecked(bool checked) {
    updateText();
    m_indicator->setChecked(checked);
}

QString SwitchButton::text() const {
    return m_text;
}

void SwitchButton::setText(const QString &text) {
    m_text = text;
    m_label->setText(text);
    adjustSize();
}

QString SwitchButton::onText() const {
    return m_onText;
}

void SwitchButton::setOnText(const QString &text) {
    m_onText = text;
    updateText();
}

QString SwitchButton::offText() const {
    return m_offText;
}

void SwitchButton::setOffText(const QString &text) {
    m_offText = text;
    updateText();
}

int SwitchButton::spacing() const {
    return m_spacing;
}

void SwitchButton::setSpacing(int spacing) {
    m_spacing = spacing;
    m_hBox->setSpacing(spacing);
    update();
}

void SwitchButton::toggleChecked() {
    m_indicator->setChecked(!m_indicator->isChecked());
}

void SwitchButton::updateText() {
    setText(isChecked() ? m_onText : m_offText);
}
