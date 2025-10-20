#ifndef PROFILECARD_H
#define PROFILECARD_H

#include <QWidget>

class BodyLabel;
class CaptionLabel;
class AvatarWidget;
class HyperlinkButton;
class ProfileCard : public QWidget
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
