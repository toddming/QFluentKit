#include "SettingCardGroup.h"
#include <QFont>
#include <QApplication>

#include "Theme.h"

SettingCardGroup::SettingCardGroup(const QString &title, QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(title, this))
    , vBoxLayout(new QVBoxLayout(this))
    , cardLayout(new ExpandLayout())
{
    // 设置布局属性
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setAlignment(Qt::AlignTop);
    vBoxLayout->setSpacing(0);

    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(2);

    // 添加控件到布局
    vBoxLayout->addWidget(titleLabel);
    vBoxLayout->addSpacing(12);
    vBoxLayout->addLayout(cardLayout, 1);

    Theme::instance()->registerWidget(this, ThemeType::ThemeStyle::SETTING_CARD_GROUP);

    // 设置字体大小为 20
    QFont font = titleLabel->font();
    font.setPointSize(20);
    titleLabel->setFont(font);
    titleLabel->adjustSize();
}

void SettingCardGroup::addSettingCard(QWidget *card)
{
    if (!card) return;

    card->setParent(this); // 确保 parent 正确
    cardLayout->addWidget(card);
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
    int h = cardLayout->heightForWidth(width()) + 56;
    resize(width(), h);
}
