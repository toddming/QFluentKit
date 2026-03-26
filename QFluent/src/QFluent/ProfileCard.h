#pragma once

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
    AvatarWidget *m_avatar;
    BodyLabel *m_nameLabel;
    CaptionLabel *m_emailLabel;
    HyperlinkButton *m_logoutButton;
};
