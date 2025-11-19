#ifndef SETTINGCARDGROUP_H
#define SETTINGCARDGROUP_H

#include <QWidget>

#include "FluentGlobal.h"
#include "../layout/ExpandLayout.h"

class QLabel;
class QVBoxLayout;
class QFLUENT_EXPORT SettingCardGroup : public QWidget
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
