#include "SettingCard.h"
#include <QApplication>
#include <QStyle>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>

#include "Theme.h"
#include "../PushButton.h"
#include "../SwitchButton.h"
#include "../ComboBox.h"
#include "StyleSheet.h"


class SettingIconWidget;

SettingCard::SettingCard(const QIcon &icon, const QString &title,
                         const QString &content, QWidget *parent)
    : QFrame(parent)
    , m_iconLabel(new SettingIconWidget(icon, this))
    , m_titleLabel(new QLabel(title, this))
    , m_contentLabel(new QLabel(content, this))
    , m_hBoxLayout(new QHBoxLayout(this))
    , m_vBoxLayout(new QVBoxLayout)
{
    if (content.isEmpty()) {
        m_contentLabel->hide();
        setFixedHeight(50);
    } else {
        setFixedHeight(70);
    }

    m_iconLabel->setFixedSize(16, 16);

    // 初始化布局
    m_hBoxLayout->setSpacing(0);
    m_hBoxLayout->setContentsMargins(16, 0, 0, 0);
    m_hBoxLayout->setAlignment(Qt::AlignVCenter);

    m_vBoxLayout->setSpacing(0);
    m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_vBoxLayout->setAlignment(Qt::AlignVCenter);

    // 组装布局
    m_hBoxLayout->addWidget(m_iconLabel, 0, Qt::AlignLeft);
    m_hBoxLayout->addSpacing(16);
    m_hBoxLayout->addLayout(m_vBoxLayout);

    m_vBoxLayout->addWidget(m_titleLabel, 0, Qt::AlignLeft);
    m_vBoxLayout->addWidget(m_contentLabel, 0, Qt::AlignLeft);

    m_hBoxLayout->addSpacing(16);
    m_hBoxLayout->addStretch(1);

    m_contentLabel->setObjectName("contentLabel");

    StyleSheetManager::instance()->registerWidget(this, Fluent::ThemeStyle::SETTING_CARD);
}

void SettingCard::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void SettingCard::setContent(const QString &content)
{
    m_contentLabel->setText(content);
    m_contentLabel->setVisible(!content.isEmpty());

    setFixedHeight(content.isEmpty() ? 50 : 70);
}

void SettingCard::setValue(const QVariant &value)
{
    Q_UNUSED(value);
    // 留给子类实现
}

void SettingCard::setIconSize(int width, int height)
{
    m_iconLabel->setFixedSize(width, height);
    // 如果 SettingIconWidget 支持动态缩放图标，可调用其 setIcon 方法
    // 此处仅调整容器大小，图标 pixmap 不自动缩放 —— 如需缩放，需重设 pixmap
}

void SettingCard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor brushColor, penColor;

    if (Theme::instance()->isDarkTheme()) {
        brushColor = QColor(255, 255, 255, 13);
        penColor = QColor(0, 0, 0, 50);
    } else {
        brushColor = QColor(255, 255, 255, 170);
        penColor = QColor(0, 0, 0, 19);
    }

    painter.setBrush(brushColor);
    painter.setPen(penColor);

    // 绘制圆角矩形，内边距 1px
    QRect rect = this->rect().adjusted(1, 1, -1, -1);
    painter.drawRoundedRect(rect, 6, 6);
}

PushSettingCard::PushSettingCard(const QString &buttonText,
                                 const QIcon &icon,
                                 const QString &title,
                                 const QString &content,
                                 QWidget *parent)
    : SettingCard(icon, title, content, parent)
    , m_button(new QPushButton(buttonText, this))
{
    // 添加按钮到布局右侧
    hBoxLayout()->addWidget(m_button, 0, Qt::AlignRight);
    hBoxLayout()->addSpacing(16);

    // 连接信号
    connect(m_button, &QPushButton::clicked, this, &PushSettingCard::clicked);
}



PrimaryPushSettingCard::PrimaryPushSettingCard(const QString &buttonText,
                                               const QIcon &icon,
                                               const QString &title,
                                               const QString &content,
                                               QWidget *parent)
    : PushSettingCard(buttonText, icon, title, content, parent)
{
    // 设置对象名，用于样式表控制
    button()->setObjectName("primaryButton");
}


HyperlinkCard::HyperlinkCard(const QString &url,
                             const QString &text,
                             const QIcon &icon,
                             const QString &title,
                             const QString &content,
                             QWidget *parent)
    : SettingCard(icon, title, content, parent)
{
    HyperlinkButton *linkButton = new HyperlinkButton(text, this);
    hBoxLayout()->addWidget(linkButton, 0, Qt::AlignRight);
    hBoxLayout()->addSpacing(16);

    connect(linkButton, &HyperlinkButton::clicked, this, [url](){
        QDesktopServices::openUrl(QUrl(url));
    });
}



SwitchSettingCard::SwitchSettingCard(const QIcon &icon,
                                     const QString &title,
                                     const QString &content,
                                     QWidget *parent)
    : SettingCard(icon, title, content, parent)
{
    m_switchButton = new SwitchButton("Off", this, SwitchButton::IndicatorPosition::Right);

    hBoxLayout()->addWidget(m_switchButton, 0, Qt::AlignRight);
    hBoxLayout()->addSpacing(16);

}

void SwitchSettingCard::onCheckedChanged(bool isChecked)
{
    setValue(isChecked);
    emit checkedChanged(isChecked);
}

void SwitchSettingCard::setValue(bool isChecked)
{
    m_switchButton->setChecked(isChecked);
    m_switchButton->setText(isChecked ? "On" : "Off");
}

void SwitchSettingCard::setChecked(bool isChecked)
{
    setValue(isChecked);
}

bool SwitchSettingCard::isChecked()
{
    return m_switchButton->isChecked();
}



ComboBoxSettingCard::ComboBoxSettingCard(const QStringList &items,
                                         const QIcon &icon,
                                         const QString &title,
                                         const QString &content,
                                         QWidget *parent)
    : SettingCard(icon, title, content, parent)
{
    m_comboBox = new ComboBox(this);
    m_comboBox->addItems(items);
    m_comboBox->setCurrentIndex(0);

    hBoxLayout()->addWidget(m_comboBox, 0, Qt::AlignRight);
    hBoxLayout()->addSpacing(16);

    connect(m_comboBox, &ComboBox::currentIndexChanged, this, &ComboBoxSettingCard::onCurrentIndexChanged);
}

void ComboBoxSettingCard::onCurrentIndexChanged(int value)
{
    emit currentIndexChanged(value);
}

void ComboBoxSettingCard::setValue(int value)
{
    m_comboBox->setCurrentIndex(value);
}
