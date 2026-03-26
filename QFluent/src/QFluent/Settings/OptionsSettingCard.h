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
    QVector<QString> m_texts;
    QString m_configName;
    QLabel* m_choiceLabel;
    QButtonGroup* m_buttonGroup;

    void adjustViewSize();
};
