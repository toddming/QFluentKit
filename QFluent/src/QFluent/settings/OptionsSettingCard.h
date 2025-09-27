#pragma once

#include <QObject>
#include <QLabel>
#include <QButtonGroup>
#include <QVector>


#include "../RadioButton.h"
#include "ExpandSettingCard.h"

class OptionsSettingCard : public ExpandSettingCard {
    Q_OBJECT

public:
    OptionsSettingCard(const QIcon& icon,
                       const QString& title,
                       const QString& content = QString(),
                       const QVector<QString>& texts = QVector<QString>(),
                       QWidget* parent = nullptr);

    void setValue(const QVariant& value);

private slots:
    void onButtonClicked(RadioButton* button);

private:
    QVector<QString> texts;
    QString configName;
    QLabel* choiceLabel;
    QButtonGroup* buttonGroup;

    void adjustViewSize();
};
