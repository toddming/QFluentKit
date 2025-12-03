#include "PickerBase.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <QRegion>
#include <QScrollBar>
#include <QDebug>

#include "Theme.h"
#include "Screen.h"
#include "StyleSheet.h"
#include "CycleListWidget.h"

// PickerColumnFormatter 实现
PickerColumnFormatter::PickerColumnFormatter(QObject* parent)
    : QObject(parent)
{
}

QString PickerColumnFormatter::encode(const QVariant& value)
{
    return value.toString();
}

QVariant PickerColumnFormatter::decode(const QString& value)
{
    return value;
}

// DigitFormatter 实现
QVariant DigitFormatter::decode(const QString& value)
{
    return value.toInt();
}

// PickerColumnButton 实现
PickerColumnButton::PickerColumnButton(const QString& name, const QList<QVariant>& items,
                                     int width, Qt::Alignment align,
                                     PickerColumnFormatter* formatter, QWidget* parent)
    : QPushButton(name, parent), m_name(name), m_align(align)
{
    setItems(items);
    setFormatter(formatter);
    setFixedSize(width, 30);
    setObjectName("pickerButton");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAlignment(align);
}

void PickerColumnButton::setAlignment(Qt::Alignment align)
{
    m_align = align;
    if (align & Qt::AlignLeft) {
        setProperty("align", "left");
    } else if (align & Qt::AlignRight) {
        setProperty("align", "right");
    } else {
        setProperty("align", "center");
    }
    style()->unpolish(this);
    style()->polish(this);
}

QString PickerColumnButton::value() const
{
    if (m_value.isNull()) {
        return QString();
    }
    return m_formatter->encode(m_value);
}

void PickerColumnButton::setValue(const QVariant& v)
{
    m_value = v;
    if (v.isNull()) {
        setText(m_name);
        setProperty("hasValue", false);
    } else {
        setText(value());
        setProperty("hasValue", true);
    }
    style()->unpolish(this);
    style()->polish(this);
}

QStringList PickerColumnButton::items() const
{
    QStringList result;
    for (const QVariant& item : m_items) {
        result << m_formatter->encode(item);
    }
    return result;
}

void PickerColumnButton::setItems(const QList<QVariant>& items)
{
    m_items = items;
}

void PickerColumnButton::setFormatter(PickerColumnFormatter* formatter)
{
    if (formatter) {
        m_formatter = formatter;
    } else {
        m_formatter = new PickerColumnFormatter(this);
    }
}

void PickerColumnButton::setName(const QString& name)
{
    if (text() == m_name) {
        setText(name);
    }
    m_name = name;
}

// DateSeparatorWidget 实现
DateSeparatorWidget::DateSeparatorWidget(Qt::Orientation orient, QWidget* parent)
    : QWidget(parent)
{
    if (orient == Qt::Horizontal) {
        setFixedHeight(1);
    } else {
        setFixedWidth(1);
    }
    setAttribute(Qt::WA_StyledBackground);
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::TIME_PICKER);
}

// ItemMaskWidget 实现
ItemMaskWidget::ItemMaskWidget(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(37);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void ItemMaskWidget::setCustomBackgroundColor(const QColor& light, const QColor& dark)
{
    m_lightBackgroundColor = light;
    m_darkBackgroundColor = dark;
    update();
}

void ItemMaskWidget::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    
    // 绘制背景
    painter.setPen(Qt::NoPen);
    QColor color = Theme::instance()->isDarkTheme() ? m_darkBackgroundColor : m_lightBackgroundColor;
    color = color.isValid() ? color : Theme::instance()->themeColor();
    painter.setBrush(color);
    painter.drawRoundedRect(rect().adjusted(4, 0, -3, 0), 5, 5);
    
    // 绘制文本
    painter.setPen(Theme::instance()->isDarkTheme() ? Qt::black : Qt::white);
    painter.setFont(font());
    
    int w = 0;
    int h = height();

    PickerPanel *panel = qobject_cast<PickerPanel*> (this->parent());
    m_listWidgets = panel->listWidgets();

    for (CycleListWidget* p : m_listWidgets) {
        painter.save();
        
        int x = p->currentItem()->sizeHint().width() / 2 + 4 + this->x();
        QListWidgetItem* item1 = p->itemAt(QPoint(x, y() + 6));

        if (item1) {
            int iw = item1->sizeHint().width();
            int y = p->visualItemRect(item1).y();
            painter.translate(w, y - this->y() + 7);
            drawText(item1, painter, 0);
            
            QListWidgetItem* item2 = p->itemAt(pos() + QPoint(x, h - 6));
            drawText(item2, painter, h);
            
            w += (iw + 8);
        }
        
        painter.restore();
    }
}

void ItemMaskWidget::drawText(QListWidgetItem* item, QPainter& painter, int y)
{
    if (!item) return;

    int align = item->textAlignment();
    int w = item->sizeHint().width();
    int h = item->sizeHint().height();

    QRectF rect;
    if (align & Qt::AlignLeft) {
        rect = QRectF(15, y, w, h);
    } else if (align & Qt::AlignRight) {
        rect = QRectF(4, y, w - 15, h);
    } else {
        rect = QRectF(4, y, w, h);
    }
    
    painter.drawText(rect, Qt::AlignCenter, item->text());
}


void PickerToolButton::drawIcon(QPainter* painter, const QRectF& rect, Fluent::ThemeMode theme)
{
    if (isPressed()) {
        painter->setOpacity(1);
    }
    TransparentToolButton::drawIcon(painter, rect, theme);
}


// PickerBase 实现
PickerBase::PickerBase(QWidget* parent)
    : QPushButton(parent), m_isResetEnabled(false)
{
    m_hBoxLayout = new QHBoxLayout(this);
    m_hBoxLayout->setSpacing(0);
    m_hBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_hBoxLayout->setSizeConstraint(QHBoxLayout::SetFixedSize);
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::TIME_PICKER);
    
    connect(this, &QPushButton::clicked, this, &PickerBase::showPanel);
}

void PickerBase::setSelectedBackgroundColor(const QColor& light, const QColor& dark)
{
    m_lightSelectedBackgroundColor = light;
    m_darkSelectedBackgroundColor = dark;
}

void PickerBase::addColumn(const QString& name, const QList<QVariant>& items, int width,
                          Qt::Alignment align, PickerColumnFormatter* formatter)
{
    PickerColumnButton* button = new PickerColumnButton(name, items, width, align, formatter, this);
    m_columns.append(button);
    m_hBoxLayout->addWidget(button, 0, Qt::AlignLeft);
    
    for (int i = 0; i < m_columns.size() - 1; ++i) {
        m_columns[i]->setProperty("hasBorder", true);
        m_columns[i]->style()->unpolish(m_columns[i]);
        m_columns[i]->style()->polish(m_columns[i]);
    }
}

void PickerBase::setColumnAlignment(int index, Qt::Alignment align)
{
    if (index >= 0 && index < m_columns.size()) {
        m_columns[index]->setAlignment(align);
    }
}

void PickerBase::setColumnWidth(int index, int width)
{
    if (index >= 0 && index < m_columns.size()) {
        m_columns[index]->setFixedWidth(width);
    }
}

void PickerBase::setColumnTight(int index)
{
    if (index >= 0 && index < m_columns.size()) {
        QFontMetrics fm = fontMetrics();
        int maxWidth = 0;
        for (const QString& item : m_columns[index]->items()) {
            maxWidth = qMax(maxWidth, fm.horizontalAdvance(item));
        }
        setColumnWidth(index, maxWidth + 30);
    }
}

void PickerBase::setColumnVisible(int index, bool isVisible)
{
    if (index >= 0 && index < m_columns.size()) {
        m_columns[index]->setVisible(isVisible);
    }
}

QStringList PickerBase::value() const
{
    QStringList result;
    for (PickerColumnButton* c : m_columns) {
        if (c->isVisible()) {
            result << c->value();
        }
    }
    return result;
}

QStringList PickerBase::initialValue() const
{
    return value();
}

void PickerBase::setColumnValue(int index, const QVariant& value)
{
    if (index >= 0 && index < m_columns.size()) {
        m_columns[index]->setValue(value);
    }
}

void PickerBase::setColumnFormatter(int index, PickerColumnFormatter* formatter)
{
    if (index >= 0 && index < m_columns.size()) {
        m_columns[index]->setFormatter(formatter);
    }
}

void PickerBase::setColumnItems(int index, const QList<QVariant>& items)
{
    if (index >= 0 && index < m_columns.size()) {
        m_columns[index]->setItems(items);
    }
}

QString PickerBase::encodeValue(int index, const QVariant& value)
{
    if (index >= 0 && index < m_columns.size()) {
        return m_columns[index]->formatter()->encode(value);
    }
    return QString();
}

QVariant PickerBase::decodeValue(int index, const QString& value)
{
    if (index >= 0 && index < m_columns.size()) {
        return m_columns[index]->formatter()->decode(value);
    }
    return QVariant();
}

void PickerBase::clearColumns()
{
    while (!m_columns.isEmpty()) {
        PickerColumnButton* btn = m_columns.takeLast();
        m_hBoxLayout->removeWidget(btn);
        btn->deleteLater();
    }
}

void PickerBase::reset()
{
    for (int i = 0; i < m_columns.size(); ++i) {
        setColumnValue(i, QVariant());
    }
}

void PickerBase::setResetEnabled(bool isEnabled)
{
    m_isResetEnabled = isEnabled;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void PickerBase::enterEvent(QEnterEvent* e)
{
    setButtonProperty("enter", true);
    QPushButton::enterEvent(e);
}
#else
void PickerBase::enterEvent(QEvent* e)
{
    setButtonProperty("enter", true);
    QPushButton::enterEvent(e);
}
#endif

void PickerBase::leaveEvent(QEvent* e)
{
    setButtonProperty("enter", false);
    QPushButton::leaveEvent(e);
}

void PickerBase::mousePressEvent(QMouseEvent* e)
{
    setButtonProperty("pressed", true);
    QPushButton::mousePressEvent(e);
}

void PickerBase::mouseReleaseEvent(QMouseEvent* e)
{
    setButtonProperty("pressed", false);
    QPushButton::mouseReleaseEvent(e);
}

void PickerBase::setButtonProperty(const QString& name, const QVariant& value)
{
    for (PickerColumnButton* button : m_columns) {
        button->setProperty(name.toUtf8().constData(), value);
        button->style()->unpolish(button);
        button->style()->polish(button);
    }
}

QStringList PickerBase::panelInitialValue()
{
    return value();
}

void PickerBase::showPanel()
{
    PickerPanel* panel = new PickerPanel(this);
    
    for (PickerColumnButton* column : m_columns) {
        if (column->isVisible()) {
            panel->addColumn(column->items(), column->width(), column->align());
        }
    }
    
    panel->setValue(panelInitialValue());
    panel->setResetEnabled(isRestEnabled());
    panel->setSelectedBackgroundColor(m_lightSelectedBackgroundColor, m_darkSelectedBackgroundColor);
    
    connect(panel, &PickerPanel::confirmed, this, &PickerBase::onConfirmed);
    connect(panel, &PickerPanel::resetted, this, &PickerBase::reset);
    connect(panel, &PickerPanel::columnValueChanged, [this, panel](int i, const QString& v) {
        onColumnValueChanged(panel, i, v);
    });
    
    int w = panel->sizeHint().width() - width();
    panel->exec(mapToGlobal(QPoint(-w / 2, -37 * 4)));
}

void PickerBase::onConfirmed(const QStringList& value)
{
    for (int i = 0; i < value.size(); ++i) {
        setColumnValue(i, value[i]);
    }
}

void PickerBase::onColumnValueChanged(PickerPanel* panel, int index, const QString& value)
{
    // 子类可以重写此方法
}

// PickerPanel 实现
PickerPanel::PickerPanel(QWidget* parent)
    : QWidget(parent), m_itemHeight(37), m_isExpanded(false)
{
    initWidget();
}

void PickerPanel::initWidget()
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    m_view = new QFrame(this);
    m_itemMaskWidget = new ItemMaskWidget(this);
    m_hSeparatorWidget = new DateSeparatorWidget(Qt::Horizontal, m_view);
    m_yesButton = new PickerToolButton(FluentIcon(Fluent::IconType::ACCEPT), m_view);
    m_resetButton = new PickerToolButton(FluentIcon(Fluent::IconType::CANCEL), m_view);
    m_cancelButton = new PickerToolButton(FluentIcon(Fluent::IconType::CLOSE), m_view);
    
    m_hBoxLayout = new QHBoxLayout(this);
    m_listLayout = new QHBoxLayout();
    m_buttonLayout = new QHBoxLayout();
    m_vBoxLayout = new QVBoxLayout(m_view);
    
    setShadowEffect();
    
    m_yesButton->setFixedHeight(33);
    m_resetButton->setFixedHeight(33);
    m_cancelButton->setFixedHeight(33);
    
    m_hBoxLayout->setContentsMargins(12, 8, 12, 20);
    m_hBoxLayout->addWidget(m_view, 1, Qt::AlignCenter);
    m_hBoxLayout->setSizeConstraint(QHBoxLayout::SetMinimumSize);
    
    m_vBoxLayout->setSpacing(0);
    m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_vBoxLayout->addLayout(m_listLayout, 1);
    m_vBoxLayout->addWidget(m_hSeparatorWidget);
    m_vBoxLayout->addLayout(m_buttonLayout, 1);
    m_vBoxLayout->setSizeConstraint(QVBoxLayout::SetMinimumSize);
    
    m_buttonLayout->setSpacing(6);
    m_buttonLayout->setContentsMargins(3, 3, 3, 3);
    m_buttonLayout->addWidget(m_yesButton);
    m_buttonLayout->addWidget(m_resetButton);
    m_buttonLayout->addWidget(m_cancelButton);
    
    m_yesButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_resetButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_cancelButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    connect(m_yesButton, &QPushButton::clicked, this, &PickerPanel::fadeOut);
    connect(m_yesButton, &QPushButton::clicked, [this]() {
        emit confirmed(value());
    });
    connect(m_cancelButton, &QPushButton::clicked, this, &PickerPanel::fadeOut);
    connect(m_resetButton, &QPushButton::clicked, this, &PickerPanel::resetted);
    connect(m_resetButton, &QPushButton::clicked, this, &PickerPanel::fadeOut);
    
    setResetEnabled(false);
    
    m_view->setObjectName("view");
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::TIME_PICKER);
}

void PickerPanel::setShadowEffect(int blurRadius, const QPoint& offset, const QColor& color)
{
    m_shadowEffect = new QGraphicsDropShadowEffect(m_view);
    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(offset);
    m_shadowEffect->setColor(color);
    m_view->setGraphicsEffect(m_shadowEffect);
}

void PickerPanel::setResetEnabled(bool isEnabled)
{
    m_resetButton->setVisible(isEnabled);
}

bool PickerPanel::isResetEnabled() const
{
    return m_resetButton->isVisible();
}

void PickerPanel::setSelectedBackgroundColor(const QColor& light, const QColor& dark)
{
    m_itemMaskWidget->setCustomBackgroundColor(light, dark);
}

void PickerPanel::addColumn(const QStringList& items, int width, Qt::Alignment align)
{
    if (!m_listWidgets.isEmpty()) {
        m_listLayout->addWidget(new DateSeparatorWidget(Qt::Vertical));
    }
    
    CycleListWidget* w = new CycleListWidget(items, QSize(width, m_itemHeight), align, m_view);
    connect(w->verticalScrollBar(), &QScrollBar::valueChanged, 
            m_itemMaskWidget, QOverload<>::of(&QWidget::update));
    
    int N = m_listWidgets.size();
    connect(w, &CycleListWidget::currentItemChanged, [this, N](QListWidgetItem* i) {
        emit columnValueChanged(N, i->text());
    });
    
    m_listWidgets.append(w);
    m_listLayout->addWidget(w);
}

void PickerPanel::resizeEvent(QResizeEvent* e)
{
    m_itemMaskWidget->resize(m_view->width() - 3, m_itemHeight);
    QMargins m = m_hBoxLayout->contentsMargins();
    m_itemMaskWidget->move(m.left() + 2, m.top() + 148);
}

QStringList PickerPanel::value() const
{
    QStringList result;
    for (CycleListWidget* w : m_listWidgets) {
        result << w->currentItem()->text();
    }
    return result;
}

void PickerPanel::setValue(const QStringList& value)
{
    if (value.size() != m_listWidgets.size()) {
        return;
    }
    
    for (int i = 0; i < value.size(); ++i) {
        m_listWidgets[i]->setSelectedItem(value[i]);
    }
}

QString PickerPanel::columnValue(int index) const
{
    if (index >= 0 && index < m_listWidgets.size()) {
        return m_listWidgets[index]->currentItem()->text();
    }
    return QString();
}

void PickerPanel::setColumnValue(int index, const QString& value)
{
    if (index >= 0 && index < m_listWidgets.size()) {
        m_listWidgets[index]->setSelectedItem(value);
    }
}

CycleListWidget* PickerPanel::column(int index)
{
    if (index >= 0 && index < m_listWidgets.size()) {
        return m_listWidgets[index];
    }
    return nullptr;
}

void PickerPanel::exec(const QPoint& pos, bool ani)
{
    if (isVisible()) {
        return;
    }
    
    show();
    
    QRect rect = Screen::getCurrentScreenGeometry();
    int w = width() + 5;
    int h = height();
    
    QPoint newPos = pos;
    newPos.setX(qMin(pos.x() - layout()->contentsMargins().left(), rect.right() - w));
    newPos.setY(qMax(rect.top(), qMin(pos.y() - 4, rect.bottom() - h + 5)));
    move(newPos);
    
    if (!ani) {
        return;
    }
    
    m_isExpanded = false;
    m_ani = new QPropertyAnimation(m_view, "windowOpacity", this);
    connect(m_ani, &QPropertyAnimation::valueChanged, this, &PickerPanel::onAniValueChanged);
    m_ani->setStartValue(0.0);
    m_ani->setEndValue(1.0);
    m_ani->setDuration(150);
    m_ani->setEasingCurve(QEasingCurve::OutQuad);
    m_ani->start();
}

QList<CycleListWidget*> PickerPanel::listWidgets() const
{
    return m_listWidgets;
}

void PickerPanel::onAniValueChanged(const QVariant& value)
{
    double opacity = value.toDouble();
    QMargins m = layout()->contentsMargins();
    int w = m_view->width() + m.left() + m.right() + 120;
    int h = m_view->height() + m.top() + m.bottom() + 12;
    
    if (!m_isExpanded) {
        int y = int(h / 2.0 * (1 - opacity));
        setMask(QRegion(0, y, w, h - y * 2));
    } else {
        int y = int(h / 3.0 * (1 - opacity));
        setMask(QRegion(0, y, w, h - y * 2));
    }
}

void PickerPanel::fadeOut()
{
    m_isExpanded = true;
    m_ani = new QPropertyAnimation(this, "windowOpacity", this);
    connect(m_ani, &QPropertyAnimation::valueChanged, this, &PickerPanel::onAniValueChanged);
    connect(m_ani, &QPropertyAnimation::finished, this, &PickerPanel::deleteLater);
    m_ani->setStartValue(1.0);
    m_ani->setEndValue(0.0);
    m_ani->setDuration(150);
    m_ani->setEasingCurve(QEasingCurve::OutQuad);
    m_ani->start();
}

