#include "SettingCardGroup.h"
#include <QFont>
#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>

#include "Theme.h"
#include "StyleSheet.h"

SettingCardGroup::SettingCardGroup(const QString &title, QWidget *parent)
    : QWidget(parent)
    , m_titleLabel(new QLabel(title, this))
    , m_vBoxLayout(new QVBoxLayout(this))
    , m_cardLayout(new ExpandLayout())
{
    // 设置布局属性
    m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_vBoxLayout->setAlignment(Qt::AlignTop);
    m_vBoxLayout->setSpacing(0);

    m_cardLayout->setContentsMargins(0, 0, 0, 0);
    m_cardLayout->setSpacing(2);

    // 添加控件到布局
    m_vBoxLayout->addWidget(m_titleLabel);
    m_vBoxLayout->addSpacing(12);
    m_vBoxLayout->addLayout(m_cardLayout, 1);

    StyleSheet::registerWidget(this, Fluent::ThemeStyle::SETTING_CARD_GROUP);

    // 设置字体大小为 20
    QFont font = m_titleLabel->font();
    font.setPointSize(20);
    m_titleLabel->setFont(font);
    m_titleLabel->adjustSize();
}

void SettingCardGroup::addSettingCard(QWidget *card)
{
    if (!card) return;

    card->setParent(this); // 确保 parent 正确
    m_cardLayout->addWidget(card);
    adjustSize(); // 调整自身大小
}

void SettingCardGroup::addSettingCards(const QList<QWidget *> &cards)
{
    for (QWidget *card : cards) {
        addSettingCard(card);
    }
}

void SettingCardGroup::adjustSize()
{
    // 根据 ExpandLayout 的 heightForWidth 计算高度
    int h = m_cardLayout->heightForWidth(width()) + 56;
    resize(width(), h);
}
