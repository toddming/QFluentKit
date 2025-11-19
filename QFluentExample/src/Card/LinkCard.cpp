#include "LinkCard.h"
#include "QFluent/IconWidget.h"
#include "TextWrap.h"
#include "FluentGlobal.h"
#include <QApplication>
#include <QLabel>

#include "StyleSheet.h"

LinkCard::LinkCard(const QIcon &icon, const QString &title, const QString &content, 
                   const QString &url, QWidget *parent)
    : QFrame(parent)
    , m_url(url)
{
    m_iconWidget = new IconWidget(icon, this);
    m_titleLabel = new QLabel(title, this);
    m_contentLabel = new QLabel(TextWrap::wrap(content, 28, false).first, this);
    m_urlWidget = new IconWidget(FluentIcon(Fluent::IconType::LINK), this); // 需要链接图标

    initWidget();
}

void LinkCard::initWidget()
{
    setCursor(Qt::PointingHandCursor);
    setFixedSize(198, 220);

    m_iconWidget->setFixedSize(54, 54);
    m_urlWidget->setFixedSize(16, 16);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(24, 24, 0, 13);
    vBoxLayout->addWidget(m_iconWidget);
    vBoxLayout->addSpacing(16);
    vBoxLayout->addWidget(m_titleLabel);
    vBoxLayout->addSpacing(8);
    vBoxLayout->addWidget(m_contentLabel);
    vBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    
    // 手动设置URL小部件位置
    m_urlWidget->move(170, 192);

    m_titleLabel->setObjectName("titleLabel");
    m_contentLabel->setObjectName("contentLabel");
    
}

void LinkCard::mouseReleaseEvent(QMouseEvent *event)
{
    QFrame::mouseReleaseEvent(event);
    QDesktopServices::openUrl(m_url);
}

LinkCardView::LinkCardView(QWidget *parent)
    : ScrollArea(parent)
{
    m_view = new QWidget(this);
    m_hBoxLayout = new QHBoxLayout(m_view);

    initWidget();
}

void LinkCardView::initWidget()
{
    m_hBoxLayout->setContentsMargins(36, 0, 0, 0);
    m_hBoxLayout->setSpacing(12);
    m_hBoxLayout->setAlignment(Qt::AlignLeft);

    setWidget(m_view);
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_view->setObjectName("view");
    
    auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/link_card.qss");
    StyleSheetManager::instance()->registerWidget(styleSource, this);

}

void LinkCardView::addCard(const QIcon &icon, const QString &title, const QString &content, const QString &url)
{
    LinkCard *card = new LinkCard(icon, title, content, url, m_view);
    m_hBoxLayout->addWidget(card, 0, Qt::AlignLeft);
}

void LinkCardView::addCard(const QString &iconPath, const QString &title, const QString &content, const QString &url)
{
    addCard(QIcon(iconPath), title, content, url);
}
