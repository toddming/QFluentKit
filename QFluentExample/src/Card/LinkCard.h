#ifndef LINK_CARD_H
#define LINK_CARD_H

#include <QFrame>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QUrl>
#include <QDesktopServices>
#include <QMouseEvent>

#include "QFluent/ScrollArea.h"

class IconWidget;
class FluentIcon;

class LinkCard : public QFrame
{
    Q_OBJECT

public:
    LinkCard(const QIcon &icon, const QString &title, const QString &content, 
             const QString &url, QWidget *parent = nullptr);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QUrl m_url;
    IconWidget *m_iconWidget;
    QLabel *m_titleLabel;
    QLabel *m_contentLabel;
    IconWidget *m_urlWidget;

    void initWidget();
};

class LinkCardView : public ScrollArea
{
    Q_OBJECT

public:
    explicit LinkCardView(QWidget *parent = nullptr);

    void addCard(const QIcon &icon, const QString &title, const QString &content, const QString &url);
    void addCard(const QString &iconPath, const QString &title, const QString &content, const QString &url);

private:
    QWidget *m_view;
    QHBoxLayout *m_hBoxLayout;

    void initWidget();
};

#endif // LINK_CARD_H
