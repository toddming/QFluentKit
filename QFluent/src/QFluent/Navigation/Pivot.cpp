#include "Pivot.h"
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QHBoxLayout>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QEvent>

#include "Animation.h"
#include "Theme.h"
#include "StyleSheet.h"

PivotItem::PivotItem(const QString &text, QWidget *parent)
    : PushButton(text, parent), m_isSelected(false)
{
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
    setProperty("isSelected", false);
    Theme::setFont(this, 18);
    StyleSheet::registerWidget(this, Fluent::ThemeStyle::PIVOT);
    connect(this, &PivotItem::clicked, [this]() {
        emit itemClicked(true);
    });
}

bool PivotItem::isSelected() const {
    return m_isSelected;
}

void PivotItem::setSelected(bool isSelected) {
    if (m_isSelected == isSelected)
        return;

    m_isSelected = isSelected;
    setProperty("isSelected", isSelected);
    emit isSelectedChanged(isSelected);
    update();
}

void PivotItem::drawIcon(QPainter* painter, const QRectF& rect)
{
    if (isPressed()) {
        painter->setOpacity(0.53);
    } else if (isHover()) {
        painter->setOpacity(0.63);
    }

    fluentIcon().paint(painter, rect.toRect());
}

Pivot::Pivot(QWidget *parent)
    : QWidget(parent), m_currentRouteKey(""), m_lightIndicatorColor(), m_darkIndicatorColor()
{
    m_hBoxLayout = new QHBoxLayout(this);
    m_hBoxLayout->setSpacing(0);
    m_hBoxLayout->setAlignment(Qt::AlignLeft);
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_hBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_slideAni = FluentAnimation::create(
        FluentAnimationType::POINT_TO_POINT,
        FluentAnimationProperty::SCALE,
        FluentAnimationSpeed::FAST,
        QVariant(0),
        this
    );

    StyleSheet::registerWidget(this, Fluent::ThemeStyle::PIVOT);
}

Pivot::~Pivot() {
    clear();
}

void Pivot::addItem(const QString &routeKey, const QString &text, const QIcon &icon) {
    insertItem(-1, routeKey, text, icon);
}

void Pivot::addItem(const QString &routeKey, const QString &text, Fluent::IconType type) {
    insertItem(-1, routeKey, text, type);
}

void Pivot::addWidget(const QString &routeKey, PivotItem *widget) {
    insertWidget(-1, routeKey, widget);
}

void Pivot::insertItem(int index, const QString &routeKey, const QString &text, const QIcon &icon) {
    if (m_items.contains(routeKey)) {
        return;
    }

    PivotItem *item = new PivotItem(text, this);
    item->setIcon(icon);
    item->setProperty("hasIcon", !icon.isNull());
    insertWidget(index, routeKey, item);
}

void Pivot::insertItem(int index, const QString &routeKey, const QString &text, Fluent::IconType type) {
    if (m_items.contains(routeKey)) {
        return;
    }

    PivotItem *item = new PivotItem(text, this);
    item->setIcon(type);
    item->setProperty("hasIcon", true);
    insertWidget(index, routeKey, item);
}

void Pivot::insertWidget(int index, const QString &routeKey, PivotItem *widget) {
    if (m_items.contains(routeKey)) {
        return;
    }

    widget->setProperty("routeKey", routeKey);
    connect(widget, &PivotItem::itemClicked, this, &Pivot::onItemClicked);
    m_items[routeKey] = widget;
    m_hBoxLayout->insertWidget(index, widget, 1);
}

void Pivot::removeWidget(const QString &routeKey) {
    if (!m_items.contains(routeKey)) {
        return;
    }

    PivotItem *item = m_items.take(routeKey);
    m_hBoxLayout->removeWidget(item);
    item->deleteLater();
    if (m_items.isEmpty()) {
        m_currentRouteKey = "";
    }
}

void Pivot::clear() {
    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        PivotItem *item = it.value();
        m_hBoxLayout->removeWidget(item);
        item->deleteLater();
    }
    m_items.clear();
    m_currentRouteKey = "";
}

PivotItem *Pivot::currentItem() const {
    if (m_currentRouteKey.isEmpty()) {
        return nullptr;
    }
    return widget(m_currentRouteKey);
}

QString Pivot::currentRouteKey() const {
    return m_currentRouteKey;
}

void Pivot::setCurrentItem(const QString &routeKey) {
    if (!m_items.contains(routeKey) || routeKey == m_currentRouteKey) {
        return;
    }

    m_currentRouteKey = routeKey;
    m_slideAni->stop();
    m_slideAni->startAnimation(widget(routeKey)->x());
    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        it.value()->setSelected(it.key() == routeKey);
    }
    emit currentItemChanged(routeKey);
    emit currentRouteKeyChanged(routeKey);
}

void Pivot::setItemFontSize(int size) {
    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        QFont font = it.value()->font();
        font.setPixelSize(size);
        it.value()->setFont(font);
        it.value()->adjustSize();
    }
}

void Pivot::setItemText(const QString &routeKey, const QString &text) {
    PivotItem *item = widget(routeKey);
    if (item) {
        item->setText(text);
    }
}

void Pivot::setIndicatorColor(const QColor &light, const QColor &dark) {
    m_lightIndicatorColor = light;
    m_darkIndicatorColor = dark;
    update();
}

void Pivot::onItemClicked() {
    PivotItem *item = qobject_cast<PivotItem *>(sender());
    if (item) {
        setCurrentItem(item->property("routeKey").toString());
    }
}

void Pivot::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    adjustIndicatorPos();
}

void Pivot::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    adjustIndicatorPos();
}

void Pivot::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if (!currentItem()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    QColor color = Theme::isDark() ? m_darkIndicatorColor : m_lightIndicatorColor;
    color = color.isValid() ? color : Theme::themeColor(Fluent::ThemeColor::PRIMARY);
    painter.setBrush(color);
    int x = currentItem()->width() / 2 - 8 + m_slideAni->value().toFloat();
    painter.drawRoundedRect(x, height() - 3, 16, 3, 1.5, 1.5);
}

void Pivot::adjustIndicatorPos() {
    PivotItem *item = currentItem();
    if (item) {
        m_slideAni->stop();
        m_slideAni->setValue(item->x());
    }
}

PivotItem *Pivot::widget(const QString &routeKey) const {
    if (!m_items.contains(routeKey)) {
        qWarning("Pivot: Route key '%s' is not valid", qPrintable(routeKey));
        return nullptr;
    }
    return m_items[routeKey];
}
