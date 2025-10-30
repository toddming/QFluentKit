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
    texts(texts),
    choiceLabel(new QLabel(this)),
    buttonGroup(new QButtonGroup(this)) {

    choiceLabel->setObjectName("titleLabel");
    addWidget(choiceLabel);

    // 设置布局参数
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(viewLayout());
    if (layout) {
        layout->setSpacing(19);
        layout->setContentsMargins(48, 18, 0, 18);
    }

    // 创建按钮
    for (int i = 0; i < texts.size(); ++i) {
        auto button = new RadioButton(texts[i], view());
        buttonGroup->addButton(button);
        layout->addWidget(button);
    }

    adjustViewSize();

    connect(buttonGroup, &QButtonGroup::buttonClicked, this, &OptionsSettingCard::onButtonClicked);
}

void OptionsSettingCard::onButtonClicked(QAbstractButton *button) {
    if (button->text() == choiceLabel->text())
        return;

    // QVariant value = button->property(configName.toUtf8().constData());

    int index = buttonGroup->buttons().indexOf(button);
    const QString text = button->text();
    emit optionChanged(index, text);

    choiceLabel->setText(button->text());
    choiceLabel->adjustSize();
}

void OptionsSettingCard::setValue(const QVariant& value) {

    for (auto btn : buttonGroup->buttons()) {
        bool isChecked = btn->property(configName.toUtf8().constData()).toString() == value.toString();
        btn->setChecked(isChecked);

        if (isChecked) {
            choiceLabel->setText(btn->text());
            choiceLabel->adjustSize();
        }
    }
}

void OptionsSettingCard::adjustViewSize() {
    // 实现视图大小调整逻辑
}
