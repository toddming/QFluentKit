#include"ProfileCard.h"
#include <QColor>

#include "Theme.h"

ProfileCard::ProfileCard(const QString& avatarPath,
                         const QString& name,
                         const QString& email,
                         QWidget *parent)
    : QWidget(parent)
{
    avatar = new AvatarWidget(avatarPath, this);
    nameLabel = new BodyLabel(name, this);
    emailLabel = new CaptionLabel(email, this);
    logoutButton = new HyperlinkButton(
        QStringLiteral("注销"),
        this
    );

    // 设置 emailLabel 颜色
    QColor emailColor = Theme::instance()->isDarkMode() ? QColor(206, 206, 206) : QColor(96, 96, 96);
    emailLabel->setStyleSheet(QStringLiteral("QLabel{color: %1}").arg(emailColor.name()));

    // 设置 nameLabel 颜色
    QColor nameColor = Theme::instance()->isDarkMode() ? QColor(255, 255, 255) : QColor(0, 0, 0);
    nameLabel->setStyleSheet(QStringLiteral("QLabel{color: %1}").arg(nameColor.name()));

    // 设置按钮字体
    Theme::instance()->setFont(logoutButton, 13);

    // 设置固定大小
    setFixedSize(307, 82);

    // 设置头像半径和位置
    avatar->setRadius(24);
    avatar->move(2, 6);

    // 设置标签位置
    nameLabel->move(64, 13);
    emailLabel->move(64, 32);
    logoutButton->move(52, 48);
}
