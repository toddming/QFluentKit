#include "OptionsSettingCard.h"
#include <QVBoxLayout>
#include <QObject>
#include <QLabel>
#include <QButtonGroup>
#include <QVector>
#include "../RadioButton.h"

OptionsSettingCard::OptionsSettingCard(const QIcon& icon,
                                       const QString& title,
                                       const QString& content,
                                       const QVector<QString>& texts,
                                       QWidget* parent)
    : ExpandSettingCard(icon, title, content, parent),
    m_texts(texts),
    m_choiceLabel(new QLabel(this)),
    m_buttonGroup(new QButtonGroup(this)) {

    m_choiceLabel->setObjectName("titleLabel");
    addWidget(m_choiceLabel);

    // 设置布局参数
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(viewLayout());
    if (layout) {
        layout->setSpacing(19);
        layout->setContentsMargins(48, 18, 0, 18);
    }

    // 创建按钮
    for (int i = 0; i < m_texts.size(); ++i) {
        auto button = new RadioButton(m_texts[i], view());
        m_buttonGroup->addButton(button);
        layout->addWidget(button);
    }

    adjustViewSize();

    connect(m_buttonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, &OptionsSettingCard::onButtonClicked);
}

void OptionsSettingCard::onButtonClicked(QAbstractButton *button) {
    if (button->text() == m_choiceLabel->text())
        return;

    // QVariant value = button->property(m_configName.toUtf8().constData());

    int index = m_buttonGroup->buttons().indexOf(button);
    const QString text = button->text();
    emit optionChanged(index, text);

    m_choiceLabel->setText(button->text());
    m_choiceLabel->adjustSize();
}

void OptionsSettingCard::setValue(const QVariant& value) {

    for (auto btn : m_buttonGroup->buttons()) {
        bool isChecked = btn->text() == value.toString();

        btn->setChecked(isChecked);

        if (isChecked) {
            m_choiceLabel->setText(btn->text());
            m_choiceLabel->adjustSize();
        }
    }
}

void OptionsSettingCard::adjustViewSize() {
    // 实现视图大小调整逻辑
}
