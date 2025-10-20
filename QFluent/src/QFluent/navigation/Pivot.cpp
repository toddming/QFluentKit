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

PivotItem::PivotItem(const QString &text, QWidget *parent)
    : QPushButton(text, parent), _isSelected(false)
{
    setAttribute(Qt::WA_LayoutUsesWidgetRect);
    setCheckable(true);
    setChecked(false);
    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::PIVOT);
    connect(this, &PivotItem::clicked, [this]() {
        emit itemClicked(true);
    });
}

bool PivotItem::isSelected() const {
    return _isSelected;
}

void PivotItem::setSelected(bool isSelected) {
    if (_isSelected == isSelected)
        return;
    _isSelected = isSelected;
    setChecked(isSelected);
    emit isSelectedChanged(isSelected);
    update();
}

void PivotItem::paintEvent(QPaintEvent *event) {
    QPushButton::paintEvent(event);
    // 可以在这里添加自定义绘制逻辑
}

Pivot::Pivot(QWidget *parent)
    : QWidget(parent), _currentRouteKey(""), lightIndicatorColor(), darkIndicatorColor()
{
    hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setSpacing(0);
    hBoxLayout->setAlignment(Qt::AlignLeft);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setIndicatorColor(Theme::instance()->themeColor(), Theme::instance()->themeColor());

    slideAni = FluentAnimation::create(
        FluentAnimationType::POINT_TO_POINT,
        FluentAnimationProperty::SCALE,
        FluentAnimationSpeed::FAST,
        QVariant(0),
        this
    );

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::PIVOT);
}

Pivot::~Pivot() {
    clear();
}

void Pivot::addItem(const QString &routeKey, const QString &text, const QIcon &icon) {
    insertItem(-1, routeKey, text, icon);
}

void Pivot::addWidget(const QString &routeKey, PivotItem *widget) {
    insertWidget(-1, routeKey, widget);
}

void Pivot::insertItem(int index, const QString &routeKey, const QString &text, const QIcon &icon) {
    if (items.contains(routeKey)) {
        return;
    }

    PivotItem *item = new PivotItem(text, this);
    item->setIcon(icon);
    insertWidget(index, routeKey, item);
}

void Pivot::insertWidget(int index, const QString &routeKey, PivotItem *widget) {
    if (items.contains(routeKey)) {
        return;
    }

    widget->setProperty("routeKey", routeKey);
    connect(widget, &PivotItem::itemClicked, this, &Pivot::_onItemClicked);
    items[routeKey] = widget;
    hBoxLayout->insertWidget(index, widget, 1);
}

void Pivot::removeWidget(const QString &routeKey) {
    if (!items.contains(routeKey)) {
        return;
    }

    PivotItem *item = items.take(routeKey);
    hBoxLayout->removeWidget(item);
    item->deleteLater();
    if (items.isEmpty()) {
        _currentRouteKey = "";
    }
}

void Pivot::clear() {
    for (auto it = items.begin(); it != items.end(); ++it) {
        PivotItem *item = it.value();
        hBoxLayout->removeWidget(item);
        item->deleteLater();
    }
    items.clear();
    _currentRouteKey = "";
}

PivotItem *Pivot::currentItem() const {
    if (_currentRouteKey.isEmpty()) {
        return nullptr;
    }
    return widget(_currentRouteKey);
}

QString Pivot::currentRouteKey() const {
    return _currentRouteKey;
}

void Pivot::setCurrentItem(const QString &routeKey) {
    if (!items.contains(routeKey) || routeKey == _currentRouteKey) {
        return;
    }

    _currentRouteKey = routeKey;
    slideAni->stop();
    slideAni->startAnimation(widget(routeKey)->x());
    for (auto it = items.begin(); it != items.end(); ++it) {
        it.value()->setSelected(it.key() == routeKey);
    }
    emit currentItemChanged(routeKey);
    emit currentRouteKeyChanged(routeKey);
}

void Pivot::setItemFontSize(int size) {
    for (auto it = items.begin(); it != items.end(); ++it) {
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
    lightIndicatorColor = light;
    darkIndicatorColor = dark;
    update();
}

void Pivot::_onItemClicked() {
    PivotItem *item = qobject_cast<PivotItem *>(sender());
    if (item) {
        setCurrentItem(item->property("routeKey").toString());
    }
}

void Pivot::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    _adjustIndicatorPos();
}

void Pivot::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    _adjustIndicatorPos();
}

void Pivot::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if (!currentItem()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(lightIndicatorColor.isValid() ? lightIndicatorColor : QColor(0, 0, 0, 0)));
    int x = currentItem()->width() / 2 - 8 + slideAni->value().toFloat();
    painter.drawRoundedRect(x, height() - 3, 16, 3, 1.5, 1.5);
}

void Pivot::_adjustIndicatorPos() {
    PivotItem *item = currentItem();
    if (item) {
        slideAni->stop();
        slideAni->setValue(item->x());
    }
}

PivotItem *Pivot::widget(const QString &routeKey) const {
    if (!items.contains(routeKey)) {
        qWarning() << "Route key" << routeKey << "is not valid";
        return nullptr;
    }
    return items[routeKey];
}
