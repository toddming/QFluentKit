#ifndef PROFILECARD_H
#define PROFILECARD_H

#include <QWidget>
#include "FluentGlobal.h"

class BodyLabel;
class CaptionLabel;
class AvatarWidget;
class HyperlinkButton;
class QFLUENT_EXPORT ProfileCard : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileCard(const QString& avatarPath,
                         const QString& name,
                         const QString& email,
                         QWidget *parent = nullptr);

private:
    AvatarWidget *avatar;
    BodyLabel *nameLabel;
    CaptionLabel *emailLabel;
    HyperlinkButton *logoutButton;
};

#endif // PROFILECARD_H
