#include "Dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QApplication>


// ========================================
// Ui_MessageBox 实现
// ========================================

void Ui_MessageBox::setupUi(const QString &title, const QString &content, QWidget *dialog)
{
    if (!dialog) return;
    m_dialog = dialog;
    m_content = content;

    // 创建控件
    titleLabel = new QLabel(title, dialog);
    contentLabel = new BodyLabel(dialog);

    buttonGroup = new QFrame(dialog);
    yesButton = new PrimaryPushButton(QObject::tr("OK"), buttonGroup);
    cancelButton = new QPushButton(QObject::tr("Cancel"), buttonGroup);
    yesButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    cancelButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);

    // 布局
    auto *vBoxLayout = new QVBoxLayout(dialog);
    auto *textLayout = new QVBoxLayout();
    auto *buttonLayout = new QHBoxLayout(buttonGroup);

    textLayout->setSpacing(12);
    textLayout->setContentsMargins(24, 24, 24, 24);
    textLayout->addWidget(titleLabel, 0, Qt::AlignTop);
    textLayout->addWidget(contentLabel, 0, Qt::AlignTop);

    buttonLayout->setSpacing(12);
    buttonLayout->setContentsMargins(24, 24, 24, 24);
    buttonLayout->addWidget(yesButton, 1, Qt::AlignTop);
    buttonLayout->addWidget(cancelButton, 1, Qt::AlignTop);
    buttonGroup->setFixedHeight(81);

    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addLayout(textLayout, 1);
    vBoxLayout->addWidget(buttonGroup, 0, Qt::AlignBottom);
    vBoxLayout->setSizeConstraint(QVBoxLayout::SetMinimumSize);

    yesButton->setFocus();

    setQss();
    adjustText();

    dialog->installEventFilter(dialog);
}

void Ui_MessageBox::adjustText()
{
    if (!m_dialog || !titleLabel || !contentLabel) {
        return;
    }

    int chars = 100;

    bool isTopLevelWindow = m_dialog->isWindow();

    if (isTopLevelWindow) {
        QWidget *parentWidget = m_dialog->parentWidget(); // 获取父控件（QWidget 类型）
        if (parentWidget) {
            int w = qMax(titleLabel->width(), parentWidget->width());
            chars = qMax(qMin(w / 9, 140), 30);
        } else {
            chars = 100;
        }
    } else {
        QWidget *topWindow = m_dialog->window();
        if (topWindow) {
            int w = qMax(titleLabel->width(), topWindow->width());
            chars = qMax(qMin(w / 9, 100), 30);
        } else {
            chars = 100;
        }
    }

    contentLabel->setText(TextWrap::wrap(m_content, chars, false).first);
}

void Ui_MessageBox::setQss()
{
    titleLabel->setObjectName("titleLabel");
    contentLabel->setObjectName("contentLabel");
    buttonGroup->setObjectName("buttonGroup");
    cancelButton->setObjectName("cancelButton");

    Theme::instance()->registerWidget(m_dialog, ThemeType::ThemeStyle::DIALOG);
    Theme::instance()->registerWidget(contentLabel, ThemeType::ThemeStyle::DIALOG);

    yesButton->adjustSize();
    cancelButton->adjustSize();
}

void Ui_MessageBox::setContentCopyable(bool isCopyable)
{
    contentLabel->setTextInteractionFlags(
        isCopyable ?
            (Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard) :
            Qt::NoTextInteraction
    );
}

// ========================================
// MessageBox 实现
// ===================
MessageBox::MessageBox(const QString &title, const QString &content, QWidget *parent)
    : MaskDialogBase(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui.setupUi(title, content, this->centerWidget());

    setMaskColor(QColor(0, 0, 0, 76));

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 50));

    connectButtons();

    centerWidget()->setFixedSize(qMax(ui.contentLabel->width(), ui.titleLabel->width()) + 48,
                                 ui.contentLabel->y() + ui.contentLabel->height() + 105);

    ui.setContentCopyable(true);
}

void MessageBox::connectButtons()
{
    connect(ui.yesButton, &QPushButton::clicked, this, [this]() {
        accept();
        emit yesClicked();
    });

    connect(ui.cancelButton, &QPushButton::clicked, this, [this]() {
        reject();
        emit cancelClicked();
    });
}

bool MessageBox::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this && event->type() == QEvent::Resize) {
        ui.adjustText();
        return true;
    }
    return MaskDialogBase::eventFilter(watched, event);
}
