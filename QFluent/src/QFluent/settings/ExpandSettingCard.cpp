#include "ExpandSettingCard.h"
#include <QRectF>
#include <QPainterPath>
#include <QFrame>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QRectF>
#include <QColor>
#include <QPainter>
#include <QIcon>
#include <QPainterPath>
#include <QFrame>
#include <QWidget>
#include <QAbstractButton>
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>

#include "Theme.h"
#include "FluentIcon.h"
#include "../scrollbar/ScrollBar.h"
#include "StyleSheet.h"


ExpandButton::ExpandButton(QWidget *parent) : QAbstractButton(parent),
    m_angle(0.0f),
    m_isHover(false),
    m_isPressed(false) {
    setFixedSize(30, 30);
    m_rotateAni = new QPropertyAnimation(this, "angle", this);
    connect(this, &ExpandButton::clicked, this, &ExpandButton::onClicked);
}

void ExpandButton::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);

    // draw background
    int r = Theme::instance()->isDarkTheme() ? 255 : 0;
    QColor color = Qt::transparent;

    if (isEnabled()) {
        if (m_isPressed) {
            color = QColor(r, r, r, 10);
        } else if (m_isHover) {
            color = QColor(r, r, r, 14);
        }
    } else {
        painter.setOpacity(0.36);
    }

    painter.setBrush(color);
    painter.drawRoundedRect(rect(), 4, 4);

    // draw icon
    painter.translate(width() / 2, height() / 2);
    painter.rotate(m_angle);

    FluentIcon(FluentIconType::ARROW_DOWN).render(&painter, QRectF(-5, -5, 9.6, 9.6));
}

void ExpandButton::enterEvent(QEnterEvent *e) {
    setHover(true);
}

void ExpandButton::leaveEvent(QEvent *e) {
    setHover(false);
}

void ExpandButton::mousePressEvent(QMouseEvent *e) {
    QAbstractButton::mousePressEvent(e);
    setPressed(true);
}

void ExpandButton::mouseReleaseEvent(QMouseEvent *e) {
    QAbstractButton::mouseReleaseEvent(e);
    setPressed(false);
}

void ExpandButton::setHover(bool isHover) {
    m_isHover = isHover;
    update();
}

void ExpandButton::setPressed(bool isPressed) {
    m_isPressed = isPressed;
    update();
}

void ExpandButton::onClicked() {
    setExpand(m_angle < 180.0f);
}

void ExpandButton::setExpand(bool isExpand) {
    m_rotateAni->stop();
    m_rotateAni->setEndValue(isExpand ? 180.0f : 0.0f);
    m_rotateAni->setDuration(200);
    m_rotateAni->start();
}

float ExpandButton::getAngle() const {
    return m_angle;
}

void ExpandButton::setAngle(float angle) {
    m_angle = angle;
    update();
}

SpaceWidget::SpaceWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedHeight(1);
}

HeaderSettingCard::HeaderSettingCard(const QIcon &icon, const QString &title, const QString &content, QWidget *parent)
    : SettingCard(icon, title, content, parent) {
    m_expandButton = new ExpandButton(this);

    hBoxLayout()->addWidget(m_expandButton, 0, Qt::AlignRight); // Assuming hBoxLayout is from SettingCard
    hBoxLayout()->addSpacing(8);

    titleLabel()->setObjectName("titleLabel"); // Assuming titleLabel from SettingCard
    installEventFilter(this);
}

bool HeaderSettingCard::eventFilter(QObject *obj, QEvent *e) {
    if (obj == this) {
        if (e->type() == QEvent::Enter) {
            expandButton()->setHover(true);
        } else if (e->type() == QEvent::Leave) {
            expandButton()->setHover(false);
        } else if (e->type() == QEvent::MouseButtonPress && static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton) {
            expandButton()->setPressed(true);
        } else if (e->type() == QEvent::MouseButtonRelease && static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton) {
            expandButton()->setPressed(false);
            expandButton()->click();
        }
    }
    return SettingCard::eventFilter(obj, e);
}

void HeaderSettingCard::addWidget(QWidget *widget) {
    int N = hBoxLayout()->count();
    hBoxLayout()->removeItem(hBoxLayout()->itemAt(N - 1));
    hBoxLayout()->addWidget(widget, 0, Qt::AlignRight);
    hBoxLayout()->addSpacing(19);
    hBoxLayout()->addWidget(expandButton(), 0, Qt::AlignRight);
    hBoxLayout()->addSpacing(8);
}

void HeaderSettingCard::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    if (Theme::instance()->isDarkTheme()) {
        painter.setBrush(QColor(255, 255, 255, 13));
    } else {
        painter.setBrush(QColor(255, 255, 255, 170));
    }

    ExpandSettingCard *p = qobject_cast<ExpandSettingCard *>(parent()); // Assuming parent is ExpandSettingCard
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(QRectF(rect().adjusted(1, 1, -1, -1)), 6, 6);

    if (p && p->isExpand()) {
        path.addRect(1, height() - 8, width() - 2, 8);
    }

    painter.drawPath(path.simplified());
}

ExpandBorderWidget::ExpandBorderWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    parent->installEventFilter(this);
}

bool ExpandBorderWidget::eventFilter(QObject *obj, QEvent *e) {
    if (obj == parent() && e->type() == QEvent::Resize) {
        resize(static_cast<QResizeEvent *>(e)->size());
    }
    return QWidget::eventFilter(obj, e);
}

void ExpandBorderWidget::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setBrush(Qt::NoBrush);

    if (Theme::instance()->isDarkTheme()) {
        painter.setPen(QColor(0, 0, 0, 50));
    } else {
        painter.setPen(QColor(0, 0, 0, 19));
    }

    ExpandSettingCard *p = qobject_cast<ExpandSettingCard *>(parent());
    int r = 6;
    int d = 12;
    int ch = p ? p->card()->height() : 0;
    int h = height();
    int w = width();

    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), r, r);

    if (ch < h) {
        painter.drawLine(1, ch, w - 1, ch);
    }
}

ExpandSettingCard::ExpandSettingCard(const QIcon &icon, const QString &title, const QString &content, QWidget *parent)
    : QScrollArea(parent),
    m_isExpand(false) {
    scrollWidget = new QFrame(this);
    m_view = new QFrame(scrollWidget);
    m_card = new HeaderSettingCard(icon, title, content, this);

    scrollLayout = new QVBoxLayout(scrollWidget);
    m_viewLayout = new QVBoxLayout(m_view);
    m_spaceWidget = new SpaceWidget(scrollWidget);
    borderWidget = new ExpandBorderWidget(this);
    setVerticalScrollBar(new ScrollBar(this));

    expandAni = new QPropertyAnimation(this->verticalScrollBar(), "value", this);

    initWidget();
}

void ExpandSettingCard::initWidget() {
    setWidget(scrollWidget);
    setWidgetResizable(true);
    setFixedHeight(m_card->height());
    setViewportMargins(0, m_card->height(), 0, 0);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(0);
    scrollLayout->addWidget(m_view);
    scrollLayout->addWidget(m_spaceWidget);

    expandAni->setEasingCurve(QEasingCurve::OutQuad);
    expandAni->setDuration(200);

    m_view->setObjectName("view");
    scrollWidget->setObjectName("scrollWidget");
    setProperty("isExpand", false);

    StyleSheetManager::instance()->registerWidget(m_card, ThemeType::ThemeStyle::EXPAND_SETTING_CARD);
    StyleSheetManager::instance()->registerWidget(this, ThemeType::ThemeStyle::EXPAND_SETTING_CARD);

    m_card->installEventFilter(this);
    connect(expandAni, &QPropertyAnimation::valueChanged, this, &ExpandSettingCard::onExpandValueChanged);
    connect(m_card->expandButton(), &ExpandButton::clicked, this, &ExpandSettingCard::toggleExpand);
}

void ExpandSettingCard::addWidget(QWidget *widget) {
    m_card->addWidget(widget);
    adjustViewSize();
}

void ExpandSettingCard::wheelEvent(QWheelEvent *e) {
    // pass
}

void ExpandSettingCard::setExpand(bool isExpand) {
    if (this->isExpand() == isExpand) {
        return;
    }

    adjustViewSize();

    m_isExpand = isExpand;
    setProperty("isExpand", isExpand);
    setStyle(QApplication::style());

    if (isExpand) {
        int h = m_viewLayout->sizeHint().height();
        verticalScrollBar()->setValue(h);
        expandAni->setStartValue(h);
        expandAni->setEndValue(0);
    } else {
        expandAni->setStartValue(0);
        expandAni->setEndValue(verticalScrollBar()->maximum());
    }

    expandAni->start();
    m_card->expandButton()->setExpand(isExpand);
}

void ExpandSettingCard::toggleExpand() {
    setExpand(!m_isExpand);
}

void ExpandSettingCard::resizeEvent(QResizeEvent *e) {
    m_card->resize(width(), m_card->height());
    scrollWidget->resize(width(), scrollWidget->height());
}

void ExpandSettingCard::onExpandValueChanged(const QVariant &value) {
    int vh = m_viewLayout->sizeHint().height();
    int h = viewportMargins().top();
    setFixedHeight(qMax(h + vh - verticalScrollBar()->value(), h));
}

void ExpandSettingCard::adjustViewSize() {
    int h = m_viewLayout->sizeHint().height();
    m_spaceWidget->setFixedHeight(h);

    if (m_isExpand) {
        setFixedHeight(m_card->height() + h);
    }
}

void ExpandSettingCard::setValue(const QVariant &value) {
    // pass
}

GroupSeparator::GroupSeparator(QWidget *parent) : QWidget(parent) {
    setFixedHeight(3);
}

void GroupSeparator::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    if (Theme::instance()->isDarkTheme()) {
        painter.setPen(QColor(0, 0, 0, 50));
    } else {
        painter.setPen(QColor(0, 0, 0, 19));
    }

    painter.drawLine(0, 1, width(), 1);
}

ExpandGroupSettingCard::ExpandGroupSettingCard(const QIcon &icon, const QString &title, const QString &content, QWidget *parent)
    : ExpandSettingCard(icon, title, content, parent) {
    viewLayout()->setContentsMargins(0, 0, 0, 0);
    viewLayout()->setSpacing(0);
}

void ExpandGroupSettingCard::addGroupWidget(QWidget *widget) {
    if (viewLayout()->count() >= 1) {
        viewLayout()->addWidget(new GroupSeparator(view()));
    }

    widget->setParent(view());
    widgets.append(widget);
    viewLayout()->addWidget(widget);
    adjustViewSize();
}

void ExpandGroupSettingCard::removeGroupWidget(QWidget *widget) {
    if (!widgets.contains(widget)) {
        return;
    }

    int layoutIndex = viewLayout()->indexOf(widget);
    int index = widgets.indexOf(widget);

    viewLayout()->removeWidget(widget);
    widgets.removeOne(widget);

    if (widgets.isEmpty()) {
        return adjustViewSize();
    }

    if (layoutIndex >= 1) {
        QWidget *separator = viewLayout()->itemAt(layoutIndex - 1)->widget();
        delete separator;
        viewLayout()->removeWidget(separator);
    } else if (index == 0) {
        QWidget *separator = viewLayout()->itemAt(0)->widget();
        delete separator;
        viewLayout()->removeWidget(separator);
    }

    adjustViewSize();
}

void ExpandGroupSettingCard::adjustViewSize() {
    int h = 0;
    for (QWidget *w : widgets) {
        h += w->sizeHint().height() + 3;
    }
    spaceWidget()->setFixedHeight(h);

    if (isExpand()) {
        setFixedHeight(card()->height() + h);
    }
}

void SimpleExpandGroupSettingCard::adjustViewSize() {
    int h = viewLayout()->sizeHint().height();
    spaceWidget()->setFixedHeight(h);

    if (isExpand()) {
        setFixedHeight(card()->height() + h);
    }
}
