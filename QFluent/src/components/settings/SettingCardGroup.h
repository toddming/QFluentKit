#ifndef SETTINGCARDGROUP_H
#define SETTINGCARDGROUP_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "../layout/ExpandLayout.h"

class SettingCardGroup : public QWidget
{
    Q_OBJECT

public:
    explicit SettingCardGroup(const QString &title, QWidget *parent = nullptr);

    void addSettingCard(QWidget *card);
    void addSettingCards(const QList<QWidget*> &cards);

    void adjustSize();

private:
    QLabel *titleLabel;
    QVBoxLayout *vBoxLayout;
    ExpandLayout *cardLayout;
};

#endif // SETTINGCARDGROUP_H
