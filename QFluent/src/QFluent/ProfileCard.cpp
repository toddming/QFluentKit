#include"ProfileCard.h"
#include <QColor>
#include <QFont>

#include "Theme.h"
#include "ImageLabel.h"
#include "Label.h"
#include "PushButton.h"

ProfileCard::ProfileCard(const QString& avatarPath,
                         const QString& name,
                         const QString& email,
                         QWidget *parent)
    : QWidget(parent)
{
    m_avatar = new AvatarWidget(avatarPath, this);
    m_nameLabel = new BodyLabel(name, this);
    m_emailLabel = new CaptionLabel(email, this);
    m_logoutButton = new HyperlinkButton(
        QStringLiteral("注销"),
        this
    );

    // 设置 m_emailLabel 颜色
    QColor emailColor = Theme::isDark() ? QColor(206, 206, 206) : QColor(96, 96, 96);
    m_emailLabel->setStyleSheet(QStringLiteral("QLabel{color: %1}").arg(emailColor.name()));

    // 设置 m_nameLabel 颜色
    QColor nameColor = Theme::isDark() ? QColor(255, 255, 255) : QColor(0, 0, 0);
    m_nameLabel->setStyleSheet(QStringLiteral("QLabel{color: %1}").arg(nameColor.name()));

    // 设置按钮字体
    Theme::instance()->setFont(m_logoutButton, 13);

    // 设置固定大小
    setFixedSize(307, 82);

    // 设置头像半径和位置
    m_avatar->setRadius(24);
    m_avatar->move(2, 6);

    // 设置标签位置
    m_nameLabel->move(64, 13);
    m_emailLabel->move(64, 32);
    m_logoutButton->move(52, 48);
}
