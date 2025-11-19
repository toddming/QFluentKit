#include "InfoBar.h"
#include <QApplication>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QParallelAnimationGroup>
#include <QPoint>
#include <QEvent>
#include <QSize>
#include <QRectF>
#include <QPainter>
#include <QIcon>
#include <QColor>
#include <QObject>
#include <QMap>
#include <QPointer>
#include <functional>
#include <QCloseEvent>

#include <QDebug>

#include "ToolButton.h"
#include "TextWrap.h"
#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"


QMap<Fluent::BarPosition, std::function<InfoBarManager*()>> InfoBarManager::m_managers;
namespace {
struct RegisterInfoBarManagers {
    RegisterInfoBarManagers() {
        InfoBarManager::registerManager(Fluent::BarPosition::TOP,
                                        []() { return TopInfoBarManager::getInstance(); });

        InfoBarManager::registerManager(Fluent::BarPosition::TOP_RIGHT,
                                        []() { return TopRightInfoBarManager::getInstance(); });

        InfoBarManager::registerManager(Fluent::BarPosition::BOTTOM_RIGHT,
                                        []() { return BottomRightInfoBarManager::getInstance(); });

        InfoBarManager::registerManager(Fluent::BarPosition::TOP_LEFT,
                                        []() { return TopLeftInfoBarManager::getInstance(); });

        InfoBarManager::registerManager(Fluent::BarPosition::BOTTOM_LEFT,
                                        []() { return BottomLeftInfoBarManager::getInstance(); });

        InfoBarManager::registerManager(Fluent::BarPosition::BOTTOM,
                                        []() { return BottomInfoBarManager::getInstance(); });
    }
};

static RegisterInfoBarManagers registerInfoBarManagersInstance;
}


InfoIconWidget::InfoIconWidget(Fluent::BarType type, QWidget* parent) : QWidget(parent) {
    setFixedSize(36, 36);
    m_type = type;
}

void InfoIconWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QRectF rect(10, 10, 15, 15);
    FluentIcon(QString(":/res/images/info_bar/%1_{color}.svg").arg(InfoBarManager::toString(m_type))).render(&painter, rect);
}

// InfoBar 实现
InfoBar::InfoBar(Fluent::BarType type, const QString& title, const QString& content,
                 Qt::Orientation orient, bool isClosable, int duration,
                 Fluent::BarPosition position, QWidget* parent)
    : QFrame(parent), m_title(title), m_content(content), m_orient(orient), m_type(type),
      m_duration(duration), m_isClosable(isClosable), m_position(position) {

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow);

    m_titleLabel = new QLabel(this);
    m_contentLabel = new QLabel(this);
    m_closeButton = new TransparentToolButton(FluentIcon(Fluent::IconType::CLOSE), this);
    m_iconWidget = new InfoIconWidget(type, this);

    m_hBoxLayout = new QHBoxLayout(this);

    if (orient == Qt::Horizontal) {
        m_textLayout = new QHBoxLayout();
        m_widgetLayout = new QHBoxLayout();
    } else {
        m_textLayout = new QVBoxLayout();
        m_widgetLayout = new QVBoxLayout();
    }

    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityAni = new QPropertyAnimation(m_opacityEffect, "opacity", this);

    m_lightBackgroundColor = QColor();
    m_darkBackgroundColor = QColor();

    __initWidget();
}


void InfoBar::__initWidget() {
    m_opacityEffect->setOpacity(1.0);
    setGraphicsEffect(m_opacityEffect);

    m_closeButton->setFixedSize(36, 36);
    m_closeButton->setIconSize(QSize(12, 12));
    m_closeButton->setCursor(Qt::PointingHandCursor);
    m_closeButton->setVisible(m_isClosable);

    __setQss();
    __initLayout();

    connect(m_closeButton, &QToolButton::clicked, this, &InfoBar::close);
}

void InfoBar::__initLayout() {
    m_hBoxLayout->setContentsMargins(6, 6, 6, 6);
    m_hBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_textLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_textLayout->setAlignment(Qt::AlignTop);
    m_textLayout->setContentsMargins(1, 8, 0, 8);

    m_hBoxLayout->setSpacing(0);
    m_textLayout->setSpacing(5);

    // 添加图标
    m_hBoxLayout->addWidget(m_iconWidget, 0, Qt::AlignTop | Qt::AlignLeft);

    // 添加标题
    m_textLayout->addWidget(m_titleLabel, 1, Qt::AlignTop);
    m_titleLabel->setVisible(!m_title.isEmpty());

    // 添加内容
    if (m_orient == Qt::Horizontal) {
        m_textLayout->addSpacing(7);
    }
    m_textLayout->addWidget(m_contentLabel, 1, Qt::AlignTop);
    m_contentLabel->setVisible(!m_content.isEmpty());
    m_hBoxLayout->addLayout(m_textLayout);

    // 添加 widget 布局
    if (m_orient == Qt::Horizontal) {
        m_hBoxLayout->addLayout(m_widgetLayout);
        m_widgetLayout->setSpacing(10);
    } else {
        m_textLayout->addLayout(m_widgetLayout);
    }

    // 添加关闭按钮
    m_hBoxLayout->addSpacing(12);
    m_hBoxLayout->addWidget(m_closeButton, 0, Qt::AlignTop | Qt::AlignLeft);

    _adjustText();
}

void InfoBar::__setQss() {
    m_titleLabel->setObjectName("titleLabel");
    m_contentLabel->setObjectName("contentLabel");
    setProperty("type", InfoBarManager::toString(m_type));
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::INFO_BAR);
}

void InfoBar::__fadeOut() {
    if (!isVisible() || (parentWidget() && !parentWidget()->isVisible())) {
        close();
        return;
    }
    m_opacityAni->setDuration(200);
    m_opacityAni->setStartValue(1.0);
    m_opacityAni->setEndValue(0.0);
    connect(m_opacityAni, &QPropertyAnimation::finished, this, &InfoBar::close);
    m_opacityAni->start();
}

void InfoBar::_adjustText() {
    int w = parent() ? (parentWidget()->width() - 50) : 900;

    // 调整标题
    int chars = qMax(qMin(w / 10, 120), 30);
    m_titleLabel->setText(TextWrap::wrap(m_title, chars, false).first);

    // 调整内容
    chars = qMax(qMin(w / 9, 120), 30);
    m_contentLabel->setText(TextWrap::wrap(m_content, chars, false).first);
    adjustSize();
}

void InfoBar::addWidget(QWidget* widget, int stretch) {
    m_widgetLayout->addSpacing(6);
    Qt::Alignment align = (m_orient == Qt::Vertical) ? Qt::AlignTop : Qt::AlignVCenter;
    m_widgetLayout->addWidget(widget, stretch, Qt::AlignLeft | align);
}

void InfoBar::setCustomBackgroundColor(const QColor& light, const QColor& dark) {
    m_lightBackgroundColor = light;
    m_darkBackgroundColor = dark;
    update();
}

bool InfoBar::eventFilter(QObject* obj, QEvent* event) {
    if (obj == parent() && (event->type() == QEvent::Resize || event->type() == QEvent::WindowStateChange)) {
        _adjustText();
    }
    return QFrame::eventFilter(obj, event);
}

void InfoBar::closeEvent(QCloseEvent* event) {
    emit closedSignal();
    deleteLater();
    event->ignore();
}

void InfoBar::showEvent(QShowEvent* event) {
    _adjustText();
    QFrame::showEvent(event);

    if (m_duration >= 0) {
        QTimer::singleShot(m_duration, this, &InfoBar::__fadeOut);
    }

    if (m_position != Fluent::BarPosition::NONE) {
        InfoBarManager* manager = InfoBarManager::make(m_position);
        manager->add(this);
    }

    if (parent()) {
        parent()->installEventFilter(this);
    }
}

void InfoBar::hideEvent(QHideEvent* event) {
    QFrame::hideEvent(event);
}

void InfoBar::paintEvent(QPaintEvent* event) {
    if (!isVisible() || (parentWidget() && !parentWidget()->isVisible())) {
        return;
    }
    QFrame::paintEvent(event);

    if (m_darkBackgroundColor == QColor() || m_lightBackgroundColor == QColor())
        return;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    if (Theme::instance()->isDarkTheme()) { painter.setBrush(m_darkBackgroundColor); } else { painter.setBrush(m_lightBackgroundColor); }

    QRect rect = this->rect().adjusted(1, 1, -1, -1);
    painter.drawRoundedRect(rect, 6, 6);
}

InfoBar* InfoBar::newInfoBar(Fluent::BarType type, const QString& title, const QString& content,
                             Qt::Orientation orient, bool isClosable, int duration,
                             Fluent::BarPosition position, QWidget* parent) {
    InfoBar* w = new InfoBar(type, title, content, orient, isClosable, duration, position, parent);
    w->show();
    return w;
}

InfoBar* InfoBar::info(const QString& title, const QString& content, Qt::Orientation orient,
                       bool isClosable, int duration, Fluent::BarPosition position, QWidget* parent) {
    return newInfoBar(Fluent::BarType::INFORMATION, title, content, orient, isClosable, duration, position, parent);
}

InfoBar* InfoBar::success(const QString& title, const QString& content, Qt::Orientation orient,
                          bool isClosable, int duration, Fluent::BarPosition position, QWidget* parent) {
    return newInfoBar(Fluent::BarType::SUCCESS, title, content, orient, isClosable, duration, position, parent);
}

InfoBar* InfoBar::warning(const QString& title, const QString& content, Qt::Orientation orient,
                          bool isClosable, int duration, Fluent::BarPosition position, QWidget* parent) {
    return newInfoBar(Fluent::BarType::WARNING, title, content, orient, isClosable, duration, position, parent);
}

InfoBar* InfoBar::error(const QString& title, const QString& content, Qt::Orientation orient,
                        bool isClosable, int duration, Fluent::BarPosition position, QWidget* parent) {
    return newInfoBar(Fluent::BarType::ERROR, title, content, orient, isClosable, duration, position, parent);
}


// InfoBarManager 实现
InfoBarManager::InfoBarManager(QObject* parent) : QObject(parent) {

}

void InfoBarManager::add(InfoBar* infoBar) {
    QWidget* p = infoBar->parentWidget();
    if (!p) return;

    if (!m_infoBars.contains(p)) {
        p->installEventFilter(this);
        m_infoBars[p] = QList<QPointer<InfoBar>>();
    }

    if (m_infoBars[p].contains(infoBar)) return;

    // 添加 drop 动画
    if (!m_infoBars[p].isEmpty()) {
        QPropertyAnimation* dropAni = new QPropertyAnimation(infoBar, "pos");
        dropAni->setDuration(200);
        dropAni->setEasingCurve(QEasingCurve::OutQuad);  // 新增，与slideAni匹配
        m_dropAnis.append(dropAni);
        infoBar->setProperty("dropAni", QVariant::fromValue(dropAni));
    }

    // 添加 slide 动画
    m_infoBars[p].append(infoBar);
    QPropertyAnimation* slideAni = _createSlideAni(infoBar);
    m_slideAnis.append(slideAni);
    infoBar->setProperty("slideAni", QVariant::fromValue(slideAni));
    connect(infoBar, &InfoBar::closedSignal, [this, infoBar]() { this->remove(infoBar); });

    slideAni->start();
}

void InfoBarManager::remove(InfoBar* infoBar) {
    QWidget* p = infoBar->parentWidget();
    if (!m_infoBars.contains(p) || !m_infoBars[p].contains(infoBar)) return;

    m_infoBars[p].removeOne(infoBar);

    // 移除 drop 动画
    QVariant dropVar = infoBar->property("dropAni");
    if (dropVar.isValid()) {
        QPropertyAnimation* dropAni = dropVar.value<QPropertyAnimation*>();
        m_dropAnis.removeOne(dropAni);
        dropAni->deleteLater();
    }

    // 移除 slide 动画
    QVariant slideVar = infoBar->property("slideAni");
    if (slideVar.isValid()) {
        QPropertyAnimation* slideAni = slideVar.value<QPropertyAnimation*>();
        m_slideAnis.removeOne(slideAni);
    }

    // 更新剩余 info bar 的位置
    _updateDropAni(p);
    for (const QPointer<InfoBar>& bar : m_infoBars[p]) {
        if (bar.isNull()) continue;
        QVariant aniVar = bar->property("dropAni");
        if (!aniVar.isValid()) continue;
        QPropertyAnimation* ani = aniVar.value<QPropertyAnimation*>();
        ani->start();  // 每个单独启动，实现并行
    }
}

QPropertyAnimation* InfoBarManager::_createSlideAni(InfoBar* infoBar) {
    QPropertyAnimation* slideAni = new QPropertyAnimation(infoBar, "pos");
    slideAni->setEasingCurve(QEasingCurve::OutQuad);
    slideAni->setDuration(200);
    slideAni->setStartValue(_slideStartPos(infoBar));
    slideAni->setEndValue(_pos(infoBar));

    return slideAni;
}

void InfoBarManager::_updateDropAni(QWidget* parent) {
    for (const QPointer<InfoBar>& bar : m_infoBars[parent]) {
        if (bar.isNull()) continue;
        QVariant aniVar = bar->property("dropAni");
        if (!aniVar.isValid()) continue;
        QPropertyAnimation* ani = aniVar.value<QPropertyAnimation*>();
        ani->setStartValue(bar->pos());
        ani->setEndValue(_pos(bar.data()));
    }
}

bool InfoBarManager::eventFilter(QObject* obj, QEvent* event) {
    QWidget* widget = qobject_cast<QWidget*>(obj);
    if (!widget || !m_infoBars.contains(widget)) return false;

    QSize size;
    if (event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        size = resizeEvent->size();
    } else if (event->type() == QEvent::WindowStateChange) {
        size = widget->size();  // 对于 WindowStateChange，使用当前大小
    } else {
        return QObject::eventFilter(obj, event);
    }

    for (const QPointer<InfoBar>& bar : m_infoBars[widget]) {
        if (!bar.isNull()) {
            bar->move(_pos(bar.data(), size));
        }
    }
    return QObject::eventFilter(obj, event);
}

void InfoBarManager::registerManager(Fluent::BarPosition position, std::function<InfoBarManager*()> creator) {
    if (!m_managers.contains(position)) {
        m_managers[position] = creator;
    }
}

InfoBarManager* InfoBarManager::make(Fluent::BarPosition position) {
    if (!m_managers.contains(position)) {
        qWarning() << "Invalid position:" << static_cast<int>(position);
        return nullptr;
    }
    return m_managers[position]();
}

QString InfoBarManager::toString(Fluent::BarType type)
{
    static const QMap<Fluent::BarType, QString> map = {
        {Fluent::BarType::INFORMATION, "Info"},
        {Fluent::BarType::SUCCESS, "Success"},
        {Fluent::BarType::WARNING, "Warning"},
        {Fluent::BarType::ERROR, "Error"}
    };
    return map.value(type, "Info");
}

// 子类实现
TopInfoBarManager* TopInfoBarManager::s_instance = nullptr;
QPoint TopInfoBarManager::_pos(InfoBar* infoBar, const QSize& parentSize) {
    QWidget* p = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : p->size();
    int x = (size.width() - infoBar->width()) / 2;
    int y = m_margin;
    int index = m_infoBars[p].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[p][i].isNull()) {
            y += m_infoBars[p][i]->height() + m_spacing;
        }
    }

    return QPoint(x, y);
}

QPoint TopInfoBarManager::_slideStartPos(InfoBar* infoBar) {
    QPoint pos = _pos(infoBar);
    return QPoint(pos.x(), pos.y() - 16);
}

TopRightInfoBarManager* TopRightInfoBarManager::s_instance = nullptr;
QPoint TopRightInfoBarManager::_pos(InfoBar* infoBar, const QSize& parentSize) {
    QWidget* p = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : p->size();
    int x = size.width() - infoBar->width() - m_margin;
    int y = m_margin;
    int index = m_infoBars[p].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[p][i].isNull()) {
            y += m_infoBars[p][i]->height() + m_spacing;
        }
    }
    return QPoint(x, y);
}

QPoint TopRightInfoBarManager::_slideStartPos(InfoBar* infoBar) {
    return QPoint(infoBar->parentWidget()->width(), _pos(infoBar).y());
}

BottomRightInfoBarManager* BottomRightInfoBarManager::s_instance = nullptr;
QPoint BottomRightInfoBarManager::_pos(InfoBar* infoBar, const QSize& parentSize) {
    QWidget* p = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : p->size();
    int x = size.width() - infoBar->width() - m_margin;
    int y = size.height() - infoBar->height() - m_margin;
    int index = m_infoBars[p].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[p][i].isNull()) {
            y -= m_infoBars[p][i]->height() + m_spacing;
        }
    }
    return QPoint(x, y);
}

QPoint BottomRightInfoBarManager::_slideStartPos(InfoBar* infoBar) {
    return QPoint(infoBar->parentWidget()->width(), _pos(infoBar).y());
}

TopLeftInfoBarManager* TopLeftInfoBarManager::s_instance = nullptr;
QPoint TopLeftInfoBarManager::_pos(InfoBar* infoBar, const QSize& parentSize) {
    QWidget* p = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : p->size();
    int y = m_margin;
    int index = m_infoBars[p].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[p][i].isNull()) {
            y += m_infoBars[p][i]->height() + m_spacing;
        }
    }
    return QPoint(m_margin, y);
}

QPoint TopLeftInfoBarManager::_slideStartPos(InfoBar* infoBar) {
    return QPoint(-infoBar->width(), _pos(infoBar).y());
}

BottomLeftInfoBarManager* BottomLeftInfoBarManager::s_instance = nullptr;
QPoint BottomLeftInfoBarManager::_pos(InfoBar* infoBar, const QSize& parentSize) {
    QWidget* p = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : p->size();
    int y = size.height() - infoBar->height() - m_margin;
    int index = m_infoBars[p].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[p][i].isNull()) {
            y -= m_infoBars[p][i]->height() + m_spacing;
        }
    }
    return QPoint(m_margin, y);
}

QPoint BottomLeftInfoBarManager::_slideStartPos(InfoBar* infoBar) {
    return QPoint(-infoBar->width(), _pos(infoBar).y());
}

BottomInfoBarManager* BottomInfoBarManager::s_instance = nullptr;
QPoint BottomInfoBarManager::_pos(InfoBar* infoBar, const QSize& parentSize) {
    QWidget* p = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : p->size();
    int x = (size.width() - infoBar->width()) / 2;
    int y = size.height() - infoBar->height() - m_margin;
    int index = m_infoBars[p].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[p][i].isNull()) {
            y -= m_infoBars[p][i]->height() + m_spacing;
        }
    }
    return QPoint(x, y);
}

QPoint BottomInfoBarManager::_slideStartPos(InfoBar* infoBar) {
    QPoint pos = _pos(infoBar);
    return QPoint(pos.x(), pos.y() + 16);
}

