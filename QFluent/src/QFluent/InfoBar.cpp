#include "InfoBar.h"

#include <QApplication>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainter>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QHideEvent>

#include "ToolButton.h"
#include "TextWrap.h"
#include "Theme.h"
#include "FluentIcon.h"
#include "StyleSheet.h"

// 静态成员初始化
QMap<Fluent::MessagePosition, std::function<InfoBarManager*()>> InfoBarManager::s_managers;

namespace {
/**
 * @brief 自动注册所有信息栏管理器
 */
struct InfoBarManagerRegistrar {
    InfoBarManagerRegistrar() {
        InfoBarManager::registerManager(Fluent::MessagePosition::TOP,
                                       []() { return TopInfoBarManager::getInstance(); });
        InfoBarManager::registerManager(Fluent::MessagePosition::TOP_RIGHT,
                                       []() { return TopRightInfoBarManager::getInstance(); });
        InfoBarManager::registerManager(Fluent::MessagePosition::BOTTOM_RIGHT,
                                       []() { return BottomRightInfoBarManager::getInstance(); });
        InfoBarManager::registerManager(Fluent::MessagePosition::TOP_LEFT,
                                       []() { return TopLeftInfoBarManager::getInstance(); });
        InfoBarManager::registerManager(Fluent::MessagePosition::BOTTOM_LEFT,
                                       []() { return BottomLeftInfoBarManager::getInstance(); });
        InfoBarManager::registerManager(Fluent::MessagePosition::BOTTOM,
                                       []() { return BottomInfoBarManager::getInstance(); });
    }
};

// 全局注册实例
static InfoBarManagerRegistrar s_registrar;

} // anonymous namespace

// ============================================================================
// InfoIconWidget 实现
// ============================================================================

InfoIconWidget::InfoIconWidget(Fluent::MessageType type, QWidget* parent)
    : QWidget(parent)
    , m_type(type)
{
    setFixedSize(36, 36);
}

void InfoIconWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QRectF iconRect(10, 10, 15, 15);
    QString iconPath = QString(":/res/images/info_bar/%1_{color}.svg")
                          .arg(InfoBarManager::toString(m_type));
    FluentIcon icon(iconPath);
    icon.render(&painter, iconRect);
}

// ============================================================================
// InfoBar 实现
// ============================================================================

InfoBar::InfoBar(Fluent::MessageType type,
                 const QString& title,
                 const QString& content,
                 Qt::Orientation orientation,
                 bool isClosable,
                 int duration,
                 Fluent::MessagePosition position,
                 QWidget* parent)
    : QFrame(parent)
    , m_title(title)
    , m_content(content)
    , m_orientation(orientation)
    , m_type(type)
    , m_duration(duration)
    , m_isClosable(isClosable)
    , m_position(position)
    , m_titleLabel(nullptr)
    , m_contentLabel(nullptr)
    , m_closeButton(nullptr)
    , m_iconWidget(nullptr)
    , m_horizontalLayout(nullptr)
    , m_textLayout(nullptr)
    , m_widgetLayout(nullptr)
    , m_opacityEffect(nullptr)
    , m_opacityAnimation(nullptr)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow);

    // 创建UI组件
    m_titleLabel = new QLabel(this);
    m_contentLabel = new QLabel(this);
    m_closeButton = new TransparentToolButton(FluentIcon(Fluent::IconType::CLOSE), this);
    m_iconWidget = new InfoIconWidget(type, this);

    // 创建布局
    m_horizontalLayout = new QHBoxLayout(this);

    if (orientation == Qt::Horizontal) {
        m_textLayout = new QHBoxLayout();
        m_widgetLayout = new QHBoxLayout();
    } else {
        m_textLayout = new QVBoxLayout();
        m_widgetLayout = new QVBoxLayout();
    }

    // 创建动画效果
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);

    initWidget();
}

void InfoBar::initWidget()
{
    // 设置透明度效果
    m_opacityEffect->setOpacity(1.0);
    setGraphicsEffect(m_opacityEffect);

    // 配置关闭按钮
    if (m_closeButton) {
        m_closeButton->setFixedSize(36, 36);
        m_closeButton->setIconSize(QSize(12, 12));
        m_closeButton->setCursor(Qt::PointingHandCursor);
        m_closeButton->setVisible(m_isClosable);
        connect(m_closeButton, &QToolButton::clicked, this, &InfoBar::close);
    }

    setStyleSheet();
    initLayout();
}

void InfoBar::initLayout()
{
    if (!m_horizontalLayout || !m_textLayout || !m_widgetLayout) {
        return;
    }

    // 设置布局边距和约束
    m_horizontalLayout->setContentsMargins(6, 6, 6, 6);
    m_horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_horizontalLayout->setSpacing(0);

    m_textLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_textLayout->setAlignment(Qt::AlignTop);
    m_textLayout->setContentsMargins(1, 8, 0, 8);
    m_textLayout->setSpacing(5);

    // 添加图标
    if (m_iconWidget) {
        m_horizontalLayout->addWidget(m_iconWidget, 0, Qt::AlignTop | Qt::AlignLeft);
    }

    // 添加标题
    if (m_titleLabel) {
        m_textLayout->addWidget(m_titleLabel, 1, Qt::AlignTop);
        m_titleLabel->setVisible(!m_title.isEmpty());
    }

    // 添加内容
    if (m_orientation == Qt::Horizontal) {
        m_textLayout->addSpacing(7);
    }

    if (m_contentLabel) {
        m_textLayout->addWidget(m_contentLabel, 1, Qt::AlignTop);
        m_contentLabel->setVisible(!m_content.isEmpty());
    }

    m_horizontalLayout->addLayout(m_textLayout);

    // 添加自定义控件布局
    if (m_orientation == Qt::Horizontal) {
        m_horizontalLayout->addLayout(m_widgetLayout);
        m_widgetLayout->setSpacing(10);
    } else {
        m_textLayout->addLayout(m_widgetLayout);
    }

    // 添加关闭按钮
    m_horizontalLayout->addSpacing(12);
    if (m_closeButton) {
        m_horizontalLayout->addWidget(m_closeButton, 0, Qt::AlignTop | Qt::AlignLeft);
    }

    adjustText();
}

void InfoBar::setStyleSheet()
{
    if (m_titleLabel) {
        m_titleLabel->setObjectName("titleLabel");
    }
    if (m_contentLabel) {
        m_contentLabel->setObjectName("contentLabel");
    }

    setProperty("type", InfoBarManager::toString(m_type));
    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::INFO_BAR);
}

void InfoBar::fadeOut()
{
    // 检查窗口和父窗口的可见性
    if (!isVisible() || (parentWidget() && !parentWidget()->isVisible())) {
        close();
        return;
    }

    if (!m_opacityAnimation) {
        close();
        return;
    }

    // 配置淡出动画
    m_opacityAnimation->setDuration(200);
    m_opacityAnimation->setStartValue(1.0);
    m_opacityAnimation->setEndValue(0.0);

    // 动画完成后关闭窗口
    connect(m_opacityAnimation, &QPropertyAnimation::finished, this, &InfoBar::close, Qt::UniqueConnection);
    m_opacityAnimation->start();
}

void InfoBar::adjustText()
{
    // 计算可用宽度
    int availableWidth = parentWidget() ? (parentWidget()->width() - 50) : 900;

    // 调整标题文本
    if (m_titleLabel && !m_title.isEmpty()) {
        int maxChars = qMax(qMin(availableWidth / 10, 120), 30);
        m_titleLabel->setText(TextWrap::wrap(m_title, maxChars, false).first);
    }

    // 调整内容文本
    if (m_contentLabel && !m_content.isEmpty()) {
        int maxChars = qMax(qMin(availableWidth / 9, 120), 30);
        m_contentLabel->setText(TextWrap::wrap(m_content, maxChars, false).first);
    }

    adjustSize();
}

void InfoBar::addWidget(QWidget* widget, int stretch)
{
    if (!widget || !m_widgetLayout) {
        return;
    }

    m_widgetLayout->addSpacing(6);
    Qt::Alignment alignment = (m_orientation == Qt::Vertical) ? Qt::AlignTop : Qt::AlignVCenter;
    m_widgetLayout->addWidget(widget, stretch, Qt::AlignLeft | alignment);
}

void InfoBar::setCustomBackgroundColor(const QColor& light, const QColor& dark)
{
    m_lightBackgroundColor = light;
    m_darkBackgroundColor = dark;
    update();
}

bool InfoBar::eventFilter(QObject* obj, QEvent* event)
{
    if (!obj || !event) {
        return QFrame::eventFilter(obj, event);
    }

    if (obj == parent() &&
        (event->type() == QEvent::Resize || event->type() == QEvent::WindowStateChange)) {
        adjustText();
    }

    return QFrame::eventFilter(obj, event);
}

void InfoBar::closeEvent(QCloseEvent* event)
{
    if (!event) {
        return;
    }

    emit closedSignal();
    deleteLater();
    event->ignore();
}

void InfoBar::showEvent(QShowEvent* event)
{
    adjustText();
    QFrame::showEvent(event);

    // 设置自动关闭定时器
    if (m_duration >= 0) {
        QTimer::singleShot(m_duration, this, &InfoBar::fadeOut);
    }

    // 添加到管理器
    if (m_position != Fluent::MessagePosition::NONE) {
        InfoBarManager* manager = InfoBarManager::make(m_position);
        if (manager) {
            manager->add(this);
        }
    }

    // 监听父窗口事件
    if (parent()) {
        parent()->installEventFilter(this);
    }
}

void InfoBar::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);

    // 如果设置了自定义背景色，进行绘制
    if (m_lightBackgroundColor.isValid() || m_darkBackgroundColor.isValid()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QColor backgroundColor = Theme::instance()->isDarkTheme() ? m_darkBackgroundColor : m_lightBackgroundColor;
        if (backgroundColor.isValid()) {
            painter.setBrush(backgroundColor);
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(rect(), 5, 5);
        }
    }
}

void InfoBar::hideEvent(QHideEvent* event)
{
    QFrame::hideEvent(event);
}

// 静态工厂方法
InfoBar* InfoBar::newInfoBar(Fluent::MessageType type,
                             const QString& title,
                             const QString& content,
                             Qt::Orientation orientation,
                             bool isClosable,
                             int duration,
                             Fluent::MessagePosition position,
                             QWidget* parent)
{
    InfoBar* infoBar = new InfoBar(type, title, content, orientation, isClosable,
                                   duration, position, parent);
    infoBar->show();
    return infoBar;
}

InfoBar* InfoBar::info(const QString& title,
                      const QString& content,
                      Qt::Orientation orientation,
                      bool isClosable,
                      int duration,
                      Fluent::MessagePosition position,
                      QWidget* parent)
{
    return newInfoBar(Fluent::MessageType::INFORMATION, title, content,
                     orientation, isClosable, duration, position, parent);
}

InfoBar* InfoBar::success(const QString& title,
                         const QString& content,
                         Qt::Orientation orientation,
                         bool isClosable,
                         int duration,
                         Fluent::MessagePosition position,
                         QWidget* parent)
{
    return newInfoBar(Fluent::MessageType::SUCCESS, title, content,
                     orientation, isClosable, duration, position, parent);
}

InfoBar* InfoBar::warning(const QString& title,
                         const QString& content,
                         Qt::Orientation orientation,
                         bool isClosable,
                         int duration,
                         Fluent::MessagePosition position,
                         QWidget* parent)
{
    return newInfoBar(Fluent::MessageType::WARNING, title, content,
                     orientation, isClosable, duration, position, parent);
}

InfoBar* InfoBar::error(const QString& title,
                       const QString& content,
                       Qt::Orientation orientation,
                       bool isClosable,
                       int duration,
                       Fluent::MessagePosition position,
                       QWidget* parent)
{
    return newInfoBar(Fluent::MessageType::ERROR, title, content,
                     orientation, isClosable, duration, position, parent);
}

// ============================================================================
// InfoBarManager 实现
// ============================================================================

InfoBarManager::InfoBarManager(QObject* parent)
    : QObject(parent)
{
}

void InfoBarManager::add(InfoBar* infoBar)
{
    if (!infoBar) {
        return;
    }

    QWidget* parent = infoBar->parentWidget();
    if (!parent) {
        return;
    }

    // 安装事件过滤器
    parent->installEventFilter(this);

    // 创建并添加drop动画
    for (const QPointer<InfoBar>& bar : m_infoBars[parent]) {
        if (bar.isNull()) {
            continue;
        }

        QVariant aniVariant = bar->property("dropAni");
        if (aniVariant.isValid()) {
            QPropertyAnimation* existingAni = aniVariant.value<QPropertyAnimation*>();
            if (existingAni && existingAni->state() == QAbstractAnimation::Running) {
                existingAni->stop();
            }
        }

        QPropertyAnimation* dropAnimation = new QPropertyAnimation(bar.data(), "pos");
        dropAnimation->setDuration(200);
        dropAnimation->setEasingCurve(QEasingCurve::OutQuad);
        m_dropAnimations.append(dropAnimation);
        bar->setProperty("dropAni", QVariant::fromValue(dropAnimation));
    }

    // 添加信息栏并创建滑动动画
    m_infoBars[parent].append(infoBar);
    QPropertyAnimation* slideAnimation = createSlideAnimation(infoBar);
    m_slideAnimations.append(slideAnimation);
    infoBar->setProperty("slideAni", QVariant::fromValue(slideAnimation));

    // 连接关闭信号
    connect(infoBar, &InfoBar::closedSignal, this, [this, infoBar]() {
        this->remove(infoBar);
    }, Qt::UniqueConnection);

    slideAnimation->start();
}

void InfoBarManager::remove(InfoBar* infoBar)
{
    if (!infoBar) {
        return;
    }

    QWidget* parent = infoBar->parentWidget();
    if (!parent || !m_infoBars.contains(parent) || !m_infoBars[parent].contains(infoBar)) {
        return;
    }

    m_infoBars[parent].removeOne(infoBar);

    // 清理drop动画
    QVariant dropVariant = infoBar->property("dropAni");
    if (dropVariant.isValid()) {
        QPropertyAnimation* dropAnimation = dropVariant.value<QPropertyAnimation*>();
        if (dropAnimation) {
            m_dropAnimations.removeOne(dropAnimation);
            dropAnimation->deleteLater();
        }
    }

    // 清理slide动画
    QVariant slideVariant = infoBar->property("slideAni");
    if (slideVariant.isValid()) {
        QPropertyAnimation* slideAnimation = slideVariant.value<QPropertyAnimation*>();
        if (slideAnimation) {
            m_slideAnimations.removeOne(slideAnimation);
            // slide动画由infoBar的父对象管理，不需要手动删除
        }
    }

    // 更新剩余信息栏位置
    updateDropAnimations(parent);
    for (const QPointer<InfoBar>& bar : m_infoBars[parent]) {
        if (bar.isNull()) {
            continue;
        }

        QVariant aniVariant = bar->property("dropAni");
        if (!aniVariant.isValid()) {
            continue;
        }

        QPropertyAnimation* animation = aniVariant.value<QPropertyAnimation*>();
        if (animation) {
            animation->start();
        }
    }
}

QPropertyAnimation* InfoBarManager::createSlideAnimation(InfoBar* infoBar)
{
    if (!infoBar) {
        return nullptr;
    }

    QPropertyAnimation* slideAnimation = new QPropertyAnimation(infoBar, "pos");
    slideAnimation->setEasingCurve(QEasingCurve::OutQuad);
    slideAnimation->setDuration(200);
    slideAnimation->setStartValue(calculateSlideStartPosition(infoBar));
    slideAnimation->setEndValue(calculatePosition(infoBar));

    return slideAnimation;
}

void InfoBarManager::updateDropAnimations(QWidget* parent)
{
    if (!parent || !m_infoBars.contains(parent)) {
        return;
    }

    for (const QPointer<InfoBar>& bar : m_infoBars[parent]) {
        if (bar.isNull()) {
            continue;
        }

        QVariant aniVariant = bar->property("dropAni");
        if (!aniVariant.isValid()) {
            continue;
        }

        QPropertyAnimation* animation = aniVariant.value<QPropertyAnimation*>();
        if (animation) {
            animation->setStartValue(bar->pos());
            animation->setEndValue(calculatePosition(bar.data()));
        }
    }
}

bool InfoBarManager::eventFilter(QObject* obj, QEvent* event)
{
    if (!obj || !event) {
        return QObject::eventFilter(obj, event);
    }

    QWidget* widget = qobject_cast<QWidget*>(obj);
    if (!widget || !m_infoBars.contains(widget)) {
        return QObject::eventFilter(obj, event);
    }

    QSize size;
    if (event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        size = resizeEvent->size();
    } else if (event->type() == QEvent::WindowStateChange) {
        size = widget->size();
    } else {
        return QObject::eventFilter(obj, event);
    }

    // 更新所有信息栏位置
    for (const QPointer<InfoBar>& bar : m_infoBars[widget]) {
        if (!bar.isNull()) {
            bar->move(calculatePosition(bar.data(), size));
        }
    }

    return QObject::eventFilter(obj, event);
}

void InfoBarManager::registerManager(Fluent::MessagePosition position,
                                     std::function<InfoBarManager*()> creator)
{
    if (!s_managers.contains(position)) {
        s_managers[position] = creator;
    }
}

InfoBarManager* InfoBarManager::make(Fluent::MessagePosition position)
{
    if (!s_managers.contains(position)) {
        return nullptr;
    }
    return s_managers[position]();
}

QString InfoBarManager::toString(Fluent::MessageType type)
{
    static const QMap<Fluent::MessageType, QString> typeMap = {
        {Fluent::MessageType::INFORMATION, "Info"},
        {Fluent::MessageType::SUCCESS, "Success"},
        {Fluent::MessageType::WARNING, "Warning"},
        {Fluent::MessageType::ERROR, "Error"}
    };
    return typeMap.value(type, "Info");
}

// ============================================================================
// TopInfoBarManager 实现
// ============================================================================

TopInfoBarManager* TopInfoBarManager::s_instance = nullptr;

TopInfoBarManager* TopInfoBarManager::getInstance()
{
    if (!s_instance) {
        s_instance = new TopInfoBarManager();
    }
    return s_instance;
}

QPoint TopInfoBarManager::calculatePosition(InfoBar* infoBar, const QSize& parentSize)
{
    if (!infoBar || !infoBar->parentWidget()) {
        return QPoint(0, 0);
    }

    QWidget* parent = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : parent->size();

    int x = (size.width() - infoBar->width()) / 2;
    int y = m_margin;

    int index = m_infoBars[parent].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[parent][i].isNull()) {
            y += m_infoBars[parent][i]->height() + m_spacing;
        }
    }

    return QPoint(x, y);
}

QPoint TopInfoBarManager::calculateSlideStartPosition(InfoBar* infoBar)
{
    if (!infoBar) {
        return QPoint(0, 0);
    }

    QPoint endPosition = calculatePosition(infoBar);
    return QPoint(endPosition.x(), endPosition.y() - 16);
}

// ============================================================================
// TopRightInfoBarManager 实现
// ============================================================================

TopRightInfoBarManager* TopRightInfoBarManager::s_instance = nullptr;

TopRightInfoBarManager* TopRightInfoBarManager::getInstance()
{
    if (!s_instance) {
        s_instance = new TopRightInfoBarManager();
    }
    return s_instance;
}

QPoint TopRightInfoBarManager::calculatePosition(InfoBar* infoBar, const QSize& parentSize)
{
    if (!infoBar || !infoBar->parentWidget()) {
        return QPoint(0, 0);
    }

    QWidget* parent = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : parent->size();

    int x = size.width() - infoBar->width() - m_margin;
    int y = m_margin;

    int index = m_infoBars[parent].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[parent][i].isNull()) {
            y += m_infoBars[parent][i]->height() + m_spacing;
        }
    }

    return QPoint(x, y);
}

QPoint TopRightInfoBarManager::calculateSlideStartPosition(InfoBar* infoBar)
{
    if (!infoBar || !infoBar->parentWidget()) {
        return QPoint(0, 0);
    }

    QPoint endPosition = calculatePosition(infoBar);
    return QPoint(infoBar->parentWidget()->width(), endPosition.y());
}

// ============================================================================
// BottomRightInfoBarManager 实现
// ============================================================================

BottomRightInfoBarManager* BottomRightInfoBarManager::s_instance = nullptr;

BottomRightInfoBarManager* BottomRightInfoBarManager::getInstance()
{
    if (!s_instance) {
        s_instance = new BottomRightInfoBarManager();
    }
    return s_instance;
}

QPoint BottomRightInfoBarManager::calculatePosition(InfoBar* infoBar, const QSize& parentSize)
{
    if (!infoBar || !infoBar->parentWidget()) {
        return QPoint(0, 0);
    }

    QWidget* parent = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : parent->size();

    int x = size.width() - infoBar->width() - m_margin;
    int y = size.height() - infoBar->height() - m_margin;

    int index = m_infoBars[parent].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[parent][i].isNull()) {
            y -= m_infoBars[parent][i]->height() + m_spacing;
        }
    }

    return QPoint(x, y);
}

QPoint BottomRightInfoBarManager::calculateSlideStartPosition(InfoBar* infoBar)
{
    if (!infoBar || !infoBar->parentWidget()) {
        return QPoint(0, 0);
    }

    QPoint endPosition = calculatePosition(infoBar);
    return QPoint(infoBar->parentWidget()->width(), endPosition.y());
}

// ============================================================================
// TopLeftInfoBarManager 实现
// ============================================================================

TopLeftInfoBarManager* TopLeftInfoBarManager::s_instance = nullptr;

TopLeftInfoBarManager* TopLeftInfoBarManager::getInstance()
{
    if (!s_instance) {
        s_instance = new TopLeftInfoBarManager();
    }
    return s_instance;
}

QPoint TopLeftInfoBarManager::calculatePosition(InfoBar* infoBar, const QSize& parentSize)
{
    if (!infoBar || !infoBar->parentWidget()) {
        return QPoint(0, 0);
    }

    QWidget* parent = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : parent->size();
    Q_UNUSED(size);

    int y = m_margin;
    int index = m_infoBars[parent].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[parent][i].isNull()) {
            y += m_infoBars[parent][i]->height() + m_spacing;
        }
    }

    return QPoint(m_margin, y);
}

QPoint TopLeftInfoBarManager::calculateSlideStartPosition(InfoBar* infoBar)
{
    if (!infoBar) {
        return QPoint(0, 0);
    }

    QPoint endPosition = calculatePosition(infoBar);
    return QPoint(-infoBar->width(), endPosition.y());
}

// ============================================================================
// BottomLeftInfoBarManager 实现
// ============================================================================

BottomLeftInfoBarManager* BottomLeftInfoBarManager::s_instance = nullptr;

BottomLeftInfoBarManager* BottomLeftInfoBarManager::getInstance()
{
    if (!s_instance) {
        s_instance = new BottomLeftInfoBarManager();
    }
    return s_instance;
}

QPoint BottomLeftInfoBarManager::calculatePosition(InfoBar* infoBar, const QSize& parentSize)
{
    if (!infoBar || !infoBar->parentWidget()) {
        return QPoint(0, 0);
    }

    QWidget* parent = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : parent->size();

    int y = size.height() - infoBar->height() - m_margin;
    int index = m_infoBars[parent].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[parent][i].isNull()) {
            y -= m_infoBars[parent][i]->height() + m_spacing;
        }
    }

    return QPoint(m_margin, y);
}

QPoint BottomLeftInfoBarManager::calculateSlideStartPosition(InfoBar* infoBar)
{
    if (!infoBar) {
        return QPoint(0, 0);
    }

    QPoint endPosition = calculatePosition(infoBar);
    return QPoint(-infoBar->width(), endPosition.y());
}

// ============================================================================
// BottomInfoBarManager 实现
// ============================================================================

BottomInfoBarManager* BottomInfoBarManager::s_instance = nullptr;

BottomInfoBarManager* BottomInfoBarManager::getInstance()
{
    if (!s_instance) {
        s_instance = new BottomInfoBarManager();
    }
    return s_instance;
}

QPoint BottomInfoBarManager::calculatePosition(InfoBar* infoBar, const QSize& parentSize)
{
    if (!infoBar || !infoBar->parentWidget()) {
        return QPoint(0, 0);
    }

    QWidget* parent = infoBar->parentWidget();
    QSize size = parentSize.isValid() ? parentSize : parent->size();

    int x = (size.width() - infoBar->width()) / 2;
    int y = size.height() - infoBar->height() - m_margin;

    int index = m_infoBars[parent].indexOf(infoBar);
    for (int i = 0; i < index; ++i) {
        if (!m_infoBars[parent][i].isNull()) {
            y -= m_infoBars[parent][i]->height() + m_spacing;
        }
    }

    return QPoint(x, y);
}

QPoint BottomInfoBarManager::calculateSlideStartPosition(InfoBar* infoBar)
{
    if (!infoBar) {
        return QPoint(0, 0);
    }

    QPoint endPosition = calculatePosition(infoBar);
    return QPoint(endPosition.x(), endPosition.y() + 16);
}
