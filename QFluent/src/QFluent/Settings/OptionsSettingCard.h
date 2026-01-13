#pragma once

#include "ExpandSettingCard.h"

class RadioButton;
class QAbstractButton;
class QFLUENT_EXPORT OptionsSettingCard : public ExpandSettingCard {
    Q_OBJECT

public:
    OptionsSettingCard(const QIcon& icon,
                       const QString& title,
                       const QString& content = QString(),
                       const QVector<QString>& texts = QVector<QString>(),
                       QWidget* parent = nullptr);

    void setValue(const QVariant& value);

signals:
    void optionChanged(int index, const QString& text);

private slots:
    void onButtonClicked(QAbstractButton *button);

private:
    QVector<QString> texts;
    QString configName;
    QLabel* choiceLabel;
    QButtonGroup* buttonGroup;

    void adjustViewSize();
};
