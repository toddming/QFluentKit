#include "Flyout.h"

#include <QApplication>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QCursor>
#include <algorithm>

#include "Theme.h"
#include "Screen.h"
#include "TextWrap.h"
#include "StyleSheet.h"
#include "FluentIcon.h"
#include "QFluent/Label.h"
#include "QFluent/ToolButton.h"
#include "QFluent/ImageLabel.h"

// ============================================================================
// FlyoutIconWidget 实现
// ============================================================================
FlyoutIconWidget::FlyoutIconWidget(const QIcon& icon, QWidget* parent)
    : QWidget(parent)
    , m_icon(icon)
{
    setFixedSize(36, 54);
}

void FlyoutIconWidget::setIcon(const QIcon& icon)
{
    m_icon = icon;
    update();
}

void FlyoutIconWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    if (m_icon.isNull()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QRectF rect(8, (height() - 20) / 2.0, 20, 20);
    m_icon.paint(&painter, rect.toRect());
}

// ============================================================================
// FlyoutViewBase 实现
// ============================================================================
FlyoutViewBase::FlyoutViewBase(QWidget* parent)
    : QWidget(parent)
{
}

void FlyoutViewBase::addWidget(QWidget* widget, int stretch, Qt::Alignment align)
{
    Q_UNUSED(widget);
    Q_UNUSED(stretch);
    Q_UNUSED(align);
    // 基类实现为空，由子类重写
}

QColor FlyoutViewBase::backgroundColor() const
{
    return Theme::instance()->isDarkTheme()
        ? QColor(40, 40, 40)
        : QColor(248, 248, 248);
}

QColor FlyoutViewBase::borderColor() const
{
    return Theme::instance()->isDarkTheme()
        ? QColor(0, 0, 0, 45)
        : QColor(0, 0, 0, 17);
}

void FlyoutViewBase::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(backgroundColor());
    painter.setPen(borderColor());

    QRect rect = this->rect().adjusted(1, 1, -1, -1);
    painter.drawRoundedRect(rect, 8, 8);
}

// ============================================================================
// FlyoutView 实现
// ============================================================================
FlyoutView::FlyoutView(const QString& title,
                       const QString& content,
                       const QIcon& icon,
                       const QPixmap& image,
                       bool isClosable,
                       QWidget* parent)
    : FlyoutViewBase(parent)
    , m_title(title)
    , m_content(content)
    , m_icon(icon)
    , m_image(image)
    , m_isClosable(isClosable)
    , m_vBoxLayout(nullptr)
    , m_viewLayout(nullptr)
    , m_widgetLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_contentLabel(nullptr)
    , m_iconWidget(nullptr)
    , m_imageLabel(nullptr)
    , m_closeButton(nullptr)
{
    m_vBoxLayout = new QVBoxLayout(this);
    m_viewLayout = new QHBoxLayout();
    m_widgetLayout = new QVBoxLayout();

    m_titleLabel = new QLabel(title, this);
    m_contentLabel = new QLabel(content, this);
    m_iconWidget = new FlyoutIconWidget(icon, this);
    m_imageLabel = new ImageLabel(this);
    m_closeButton = new TransparentToolButton(FluentIcon(Fluent::IconType::CLOSE), this);

    initWidgets();
}

void FlyoutView::initWidgets()
{
    if (!m_imageLabel || !m_closeButton || !m_titleLabel ||
        !m_contentLabel || !m_iconWidget) {
        return;
    }

    m_imageLabel->setImage(m_image);

    m_closeButton->setFixedSize(32, 32);
    m_closeButton->setIconSize(QSize(12, 12));
    m_closeButton->setVisible(m_isClosable);

    m_titleLabel->setVisible(!m_title.isEmpty());
    m_contentLabel->setVisible(!m_content.isEmpty());
    m_iconWidget->setHidden(m_icon.isNull());

    connect(m_closeButton, &TransparentToolButton::clicked,
            this, &FlyoutView::closed);

    m_titleLabel->setObjectName("titleLabel");
    m_contentLabel->setObjectName("contentLabel");

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::TEACHING_TIP);

    initLayout();
}

void FlyoutView::initLayout()
{
    if (!m_vBoxLayout || !m_viewLayout || !m_widgetLayout) {
        return;
    }

    m_vBoxLayout->setContentsMargins(1, 1, 1, 1);
    m_widgetLayout->setContentsMargins(0, 8, 0, 8);
    m_viewLayout->setSpacing(4);
    m_widgetLayout->setSpacing(0);
    m_vBoxLayout->setSpacing(0);

    // 添加图标组件
    if (m_title.isEmpty() || m_content.isEmpty()) {
        m_iconWidget->setFixedHeight(36);
    }

    m_vBoxLayout->addLayout(m_viewLayout);
    m_viewLayout->addWidget(m_iconWidget, 0, Qt::AlignTop);

    // 添加文本
    adjustText();
    m_widgetLayout->addWidget(m_titleLabel);
    m_widgetLayout->addWidget(m_contentLabel);
    m_viewLayout->addLayout(m_widgetLayout);

    // 添加关闭按钮
    m_closeButton->setVisible(m_isClosable);
    m_viewLayout->addWidget(m_closeButton, 0, Qt::AlignRight | Qt::AlignTop);

    // 调整边距
    QMargins margins(6, 5, 6, 5);
    margins.setLeft(m_icon.isNull() ? 20 : 5);
    margins.setRight(m_isClosable ? 6 : 20);
    m_viewLayout->setContentsMargins(margins);

    // 添加图片
    adjustImage();
    addImageToLayout();
}

void FlyoutView::addWidget(QWidget* widget, int stretch, Qt::Alignment align)
{
    if (!widget || !m_widgetLayout) {
        return;
    }

    m_widgetLayout->addSpacing(8);
    m_widgetLayout->addWidget(widget, stretch, align);
}

void FlyoutView::addImageToLayout()
{
    if (!m_imageLabel || !m_vBoxLayout) {
        return;
    }

    m_imageLabel->setBorderRadius(8, 8, 0, 0);
    m_imageLabel->setHidden(m_imageLabel->isNull());
    m_vBoxLayout->insertWidget(0, m_imageLabel);
}

void FlyoutView::adjustText()
{
    if (!m_titleLabel || !m_contentLabel) {
        return;
    }

    QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
    int screenWidth = screen ? screen->geometry().width() : 1920;
    int w = qMin(900, screenWidth - 200);

    // 调整标题
    int titleChars = qMax(qMin(w / 10, 120), 30);
    m_titleLabel->setText(TextWrap::wrap(m_title, titleChars, false).first);

    // 调整内容
    int contentChars = qMax(qMin(w / 9, 120), 30);
    m_contentLabel->setText(TextWrap::wrap(m_content, contentChars, false).first);
}

void FlyoutView::adjustImage()
{
    if (!m_imageLabel || !m_vBoxLayout) {
        return;
    }

    int w = m_vBoxLayout->sizeHint().width() - 2;
    m_imageLabel->scaledToWidth(w);
}

void FlyoutView::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    adjustImage();
    adjustSize();
}

// ============================================================================
// Flyout 实现
// ============================================================================
Flyout::Flyout(FlyoutViewBase* view,
               QWidget* parent,
               bool isDeleteOnClose,
               bool isMacInputMethodEnabled)
    : QWidget(parent)
    , m_view(view)
    , m_hBoxLayout(nullptr)
    , m_aniManager(nullptr)
    , m_shadowEffect(nullptr)
    , m_fadeOutAni(nullptr)
    , m_isDeleteOnClose(isDeleteOnClose)
    , m_isMacInputMethodEnabled(isMacInputMethodEnabled)
{
    if (!m_view) {
        return;
    }

    m_hBoxLayout = new QHBoxLayout(this);
    m_hBoxLayout->setContentsMargins(15, 8, 15, 20);
    m_hBoxLayout->addWidget(m_view);

    setShadowEffect();

    setAttribute(Qt::WA_TranslucentBackground);

#ifdef Q_OS_MAC
    if (isMacInputMethodEnabled) {
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
        QApplication::instance()->installEventFilter(this);
    } else {
        setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    }
#else
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
#endif
}

Flyout::~Flyout()
{
}

bool Flyout::eventFilter(QObject* watched, QEvent* event)
{
#ifdef Q_OS_MAC
    if (!m_isMacInputMethodEnabled) {
        return QWidget::eventFilter(watched, event);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        QPoint globalPos;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        globalPos = mouseEvent->globalPosition().toPoint();
#else
        globalPos = mouseEvent->globalPos();
#endif

        if (!this->geometry().contains(globalPos)) {
            close();
        }
    }
#else
    Q_UNUSED(watched);
    Q_UNUSED(event);
#endif

return QWidget::eventFilter(watched, event);
}

void Flyout::closeEvent(QCloseEvent* event)
{
    emit closed();

    if (m_isDeleteOnClose) {
        deleteLater();
    }

    QWidget::closeEvent(event);
}

void Flyout::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (m_view) {
        m_view->adjustSize();
    }
}

void Flyout::setShadowEffect(int blurRadius, const QPoint& offset)
{
    if (!m_view) {
        return;
    }

    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(blurRadius);
    m_shadowEffect->setOffset(offset);
    m_shadowEffect->setColor(QColor(0, 0, 0, 50));
    m_view->setGraphicsEffect(m_shadowEffect);
}

void Flyout::exec(const QPoint& pos, FlyoutAnimationType aniType)
{
    // 清理之前的动画管理器
    if (m_aniManager) {
        m_aniManager->deleteLater();
        m_aniManager = nullptr;
    }

    m_aniManager = FlyoutAnimationManager::make(aniType, this);
    if (m_aniManager) {
        m_aniManager->exec(pos);
    }
}

Flyout* Flyout::make(FlyoutViewBase* view,
                     QWidget* target,
                     QWidget* parent,
                     FlyoutAnimationType aniType,
                     bool isDeleteOnClose,
                     bool isMacInputMethodEnabled)
{
    if (!view) {
        return nullptr;
    }

    Flyout* flyout = new Flyout(view, parent, isDeleteOnClose, isMacInputMethodEnabled);

    if (!target) {
        return flyout;
    }

    flyout->show();

    FlyoutAnimationManager* manager = FlyoutAnimationManager::make(aniType, flyout);
    if (manager) {
        QPoint pos = manager->position(target);
        flyout->exec(pos, aniType);
        manager->deleteLater();
    }

    return flyout;
}

Flyout* Flyout::create(const QString& title,
                       const QString& content,
                       const QIcon& icon,
                       const QPixmap& image,
                       bool isClosable,
                       QWidget* target,
                       QWidget* parent,
                       FlyoutAnimationType aniType,
                       bool isDeleteOnClose,
                       bool isMacInputMethodEnabled)
{
    FlyoutView* view = new FlyoutView(title, content, icon, image, isClosable);
    Flyout* flyout = make(view, target, parent, aniType, isDeleteOnClose, isMacInputMethodEnabled);

    if (flyout && view) {
        connect(view, &FlyoutView::closed, flyout, &Flyout::close);
    }

    return flyout;
}

void Flyout::fadeOut()
{
    // 清理之前的淡出动画
    if (m_fadeOutAni) {
        m_fadeOutAni->stop();
        m_fadeOutAni->deleteLater();
    }

    m_fadeOutAni = new QPropertyAnimation(this, "windowOpacity", this);
    connect(m_fadeOutAni, &QPropertyAnimation::finished, this, &Flyout::close);

    m_fadeOutAni->setStartValue(1.0);
    m_fadeOutAni->setEndValue(0.0);
    m_fadeOutAni->setDuration(120);
    m_fadeOutAni->start();
}

// ============================================================================
// FlyoutAnimationManager 实现
// ============================================================================
FlyoutAnimationManager::FlyoutAnimationManager(Flyout* flyout)
    : QObject(flyout)
    , m_flyout(flyout)
    , m_aniGroup(nullptr)
    , m_slideAni(nullptr)
    , m_opacityAni(nullptr)
{
    if (!m_flyout) {
        return;
    }

    m_aniGroup = new QParallelAnimationGroup(this);
    m_slideAni = new QPropertyAnimation(flyout, "pos", this);
    m_opacityAni = new QPropertyAnimation(flyout, "windowOpacity", this);

    m_slideAni->setDuration(187);
    m_opacityAni->setDuration(187);

    m_opacityAni->setStartValue(0.0);
    m_opacityAni->setEndValue(1.0);

    m_slideAni->setEasingCurve(QEasingCurve::OutQuad);
    m_opacityAni->setEasingCurve(QEasingCurve::OutQuad);

    m_aniGroup->addAnimation(m_slideAni);
    m_aniGroup->addAnimation(m_opacityAni);
}

void FlyoutAnimationManager::exec(const QPoint& pos)
{
    Q_UNUSED(pos);
    // 基类实现为空，由子类重写
}

QPoint FlyoutAnimationManager::position(QWidget* target)
{
    Q_UNUSED(target);
    // 基类实现为空，由子类重写
    return QPoint();
}

QPoint FlyoutAnimationManager::adjustPosition(const QPoint& pos)
{
    if (!m_flyout) {
        return pos;
    }

    QRect rect = Screen::getCurrentScreenGeometry();
    int w = m_flyout->sizeHint().width() + 5;
    int h = m_flyout->sizeHint().height();
    int x = qMax(rect.left(), qMin(pos.x(), rect.right() - w));
    int y = qMax(rect.top(), qMin(pos.y() - 4, rect.bottom() - h + 5));

    return QPoint(x, y);
}

FlyoutAnimationManager* FlyoutAnimationManager::make(FlyoutAnimationType aniType, Flyout* flyout)
{
    if (!flyout) {
        return nullptr;
    }

    switch (aniType) {
        case FlyoutAnimationType::PULL_UP:
            return new PullUpFlyoutAnimationManager(flyout);
        case FlyoutAnimationType::DROP_DOWN:
            return new DropDownFlyoutAnimationManager(flyout);
        case FlyoutAnimationType::SLIDE_LEFT:
            return new SlideLeftFlyoutAnimationManager(flyout);
        case FlyoutAnimationType::SLIDE_RIGHT:
            return new SlideRightFlyoutAnimationManager(flyout);
        case FlyoutAnimationType::FADE_IN:
            return new FadeInFlyoutAnimationManager(flyout);
        case FlyoutAnimationType::NONE:
            return new DummyFlyoutAnimationManager(flyout);
        default:
            return new PullUpFlyoutAnimationManager(flyout);
    }
}

// ============================================================================
// PullUpFlyoutAnimationManager 实现
// ============================================================================
PullUpFlyoutAnimationManager::PullUpFlyoutAnimationManager(Flyout* flyout)
    : FlyoutAnimationManager(flyout)
{
}

QPoint PullUpFlyoutAnimationManager::position(QWidget* target)
{
    if (!target || !m_flyout || !m_flyout->layout()) {
        return QPoint();
    }

    QPoint pos = target->mapToGlobal(QPoint());
    int x = pos.x() + target->width() / 2 - m_flyout->sizeHint().width() / 2;
    int y = pos.y() - m_flyout->sizeHint().height() +
            m_flyout->layout()->contentsMargins().bottom();

    return QPoint(x, y);
}

void PullUpFlyoutAnimationManager::exec(const QPoint& pos)
{
    if (!m_slideAni || !m_aniGroup) {
        return;
    }

    QPoint adjustedPos = adjustPosition(pos);
    m_slideAni->setStartValue(adjustedPos + QPoint(0, 8));
    m_slideAni->setEndValue(adjustedPos);
    m_aniGroup->start();
}

// ============================================================================
// DropDownFlyoutAnimationManager 实现
// ============================================================================
DropDownFlyoutAnimationManager::DropDownFlyoutAnimationManager(Flyout* flyout)
    : FlyoutAnimationManager(flyout)
{
}

QPoint DropDownFlyoutAnimationManager::position(QWidget* target)
{
    if (!target || !m_flyout || !m_flyout->layout()) {
        return QPoint();
    }

    QPoint pos = target->mapToGlobal(QPoint(0, target->height()));
    int x = pos.x() + target->width() / 2 - m_flyout->sizeHint().width() / 2;
    int y = pos.y() - m_flyout->layout()->contentsMargins().top() + 8;

    return QPoint(x, y);
}

void DropDownFlyoutAnimationManager::exec(const QPoint& pos)
{
    if (!m_slideAni || !m_aniGroup) {
        return;
    }

    QPoint adjustedPos = adjustPosition(pos);
    m_slideAni->setStartValue(adjustedPos - QPoint(0, 8));
    m_slideAni->setEndValue(adjustedPos);
    m_aniGroup->start();
}

// ============================================================================
// SlideLeftFlyoutAnimationManager 实现
// ============================================================================
SlideLeftFlyoutAnimationManager::SlideLeftFlyoutAnimationManager(Flyout* flyout)
    : FlyoutAnimationManager(flyout)
{
}

QPoint SlideLeftFlyoutAnimationManager::position(QWidget* target)
{
    if (!target || !m_flyout || !m_flyout->layout()) {
        return QPoint();
    }

    QPoint pos = target->mapToGlobal(QPoint(0, 0));
    int x = pos.x() - m_flyout->sizeHint().width() + 8;
    int y = pos.y() - m_flyout->sizeHint().height() / 2 + target->height() / 2 +
            m_flyout->layout()->contentsMargins().top();

    return QPoint(x, y);
}

void SlideLeftFlyoutAnimationManager::exec(const QPoint& pos)
{
    if (!m_slideAni || !m_aniGroup) {
        return;
    }

    QPoint adjustedPos = adjustPosition(pos);
    m_slideAni->setStartValue(adjustedPos + QPoint(8, 0));
    m_slideAni->setEndValue(adjustedPos);
    m_aniGroup->start();
}

// ============================================================================
// SlideRightFlyoutAnimationManager 实现
// ============================================================================
SlideRightFlyoutAnimationManager::SlideRightFlyoutAnimationManager(Flyout* flyout)
    : FlyoutAnimationManager(flyout)
{
}

QPoint SlideRightFlyoutAnimationManager::position(QWidget* target)
{
    if (!target || !m_flyout || !m_flyout->layout()) {
        return QPoint();
    }

    QPoint pos = target->mapToGlobal(QPoint(0, 0));
    int x = pos.x() + target->width() - 8;
    int y = pos.y() - m_flyout->sizeHint().height() / 2 + target->height() / 2 +
            m_flyout->layout()->contentsMargins().top();

    return QPoint(x, y);
}

void SlideRightFlyoutAnimationManager::exec(const QPoint& pos)
{
    if (!m_slideAni || !m_aniGroup) {
        return;
    }

    QPoint adjustedPos = adjustPosition(pos);
    m_slideAni->setStartValue(adjustedPos - QPoint(8, 0));
    m_slideAni->setEndValue(adjustedPos);
    m_aniGroup->start();
}

// ============================================================================
// FadeInFlyoutAnimationManager 实现
// ============================================================================
FadeInFlyoutAnimationManager::FadeInFlyoutAnimationManager(Flyout* flyout)
    : FlyoutAnimationManager(flyout)
{
}

QPoint FadeInFlyoutAnimationManager::position(QWidget* target)
{
    if (!target || !m_flyout || !m_flyout->layout()) {
        return QPoint();
    }

    QPoint pos = target->mapToGlobal(QPoint());
    int x = pos.x() + target->width() / 2 - m_flyout->sizeHint().width() / 2;
    int y = pos.y() - m_flyout->sizeHint().height() +
            m_flyout->layout()->contentsMargins().bottom();

    return QPoint(x, y);
}

void FadeInFlyoutAnimationManager::exec(const QPoint& pos)
{
    if (!m_flyout || !m_slideAni || !m_aniGroup) {
        return;
    }

    m_flyout->move(adjustPosition(pos));
    m_aniGroup->removeAnimation(m_slideAni);
    m_aniGroup->start();
}

// ============================================================================
// DummyFlyoutAnimationManager 实现
// ============================================================================
DummyFlyoutAnimationManager::DummyFlyoutAnimationManager(Flyout* flyout)
    : FlyoutAnimationManager(flyout)
{
}

QPoint DummyFlyoutAnimationManager::position(QWidget* target)
{
    if (!target || !m_flyout || !m_flyout->layout()) {
        return QPoint();
    }

    QMargins m = m_flyout->layout()->contentsMargins();
    return target->mapToGlobal(QPoint(-m.left(),
                                     -m_flyout->sizeHint().height() + m.bottom() - 8));
}

void DummyFlyoutAnimationManager::exec(const QPoint& pos)
{
    if (!m_flyout) {
        return;
    }

    m_flyout->move(adjustPosition(pos));
}
