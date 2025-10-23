#include "SampleCard.h"
#include "QFluent/IconWidget.h"
#include "TextWrap.h"
#include <QGridLayout>
#include "StyleSheet.h"

SampleCard::SampleCard(const QIcon &icon, const QString &title, const QString &content, 
                       const QString &routeKey, int index, QWidget *parent)
    : CardWidget(parent)
    , m_index(index)
    , m_routeKey(routeKey)
{
    m_iconWidget = new IconWidget(icon, this);
    m_titleLabel = new QLabel(title, this);
    m_contentLabel = new QLabel(TextWrap::wrap(content, 45, false).first, this);

    initWidget();
}

void SampleCard::initWidget()
{
    setCursor(Qt::PointingHandCursor);
    setFixedSize(360, 90);

    m_iconWidget->setFixedSize(48, 48);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    QVBoxLayout *vBoxLayout = new QVBoxLayout();

    hBoxLayout->setSpacing(28);
    hBoxLayout->setContentsMargins(20, 0, 0, 0);
    vBoxLayout->setSpacing(2);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setAlignment(Qt::AlignVCenter);

    hBoxLayout->setAlignment(Qt::AlignVCenter);
    hBoxLayout->addWidget(m_iconWidget);
    hBoxLayout->addLayout(vBoxLayout);
    vBoxLayout->addStretch(1);
    vBoxLayout->addWidget(m_titleLabel);
    vBoxLayout->addWidget(m_contentLabel);
    vBoxLayout->addStretch(1);

    m_titleLabel->setObjectName("titleLabel");
    m_contentLabel->setObjectName("contentLabel");
}

void SampleCard::mouseReleaseEvent(QMouseEvent *event)
{
    CardWidget::mouseReleaseEvent(event);
    emit clicked(m_routeKey, m_index);
}

SampleCardView::SampleCardView(const QString &title, QWidget *parent)
    : QWidget(parent)
    , m_flowLayout(nullptr)
{
    m_titleLabel = new QLabel(title, this);
    m_vBoxLayout = new QVBoxLayout(this);

    initWidget();
    createFlowLayout();
}

void SampleCardView::initWidget()
{
    m_vBoxLayout->setContentsMargins(36, 0, 36, 0);
    m_vBoxLayout->setSpacing(10);

    m_vBoxLayout->addWidget(m_titleLabel);
    
    m_titleLabel->setObjectName("viewTitleLabel");
    
    auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/sample_card.qss");
    StyleSheetManager::instance()->registerWidget(styleSource, this);
}

void SampleCardView::createFlowLayout()
{
    m_flowLayout = new FlowLayout();
    m_flowLayout->setContentsMargins(0, 0, 0, 0);
    m_flowLayout->setHorizontalSpacing(12);
    m_flowLayout->setVerticalSpacing(12);
    
    m_vBoxLayout->addLayout(m_flowLayout);
}

void SampleCardView::addSampleCard(const QIcon &icon, const QString &title, 
                                   const QString &content, const QString &routeKey, int index)
{
    SampleCard *card = new SampleCard(icon, title, content, routeKey, index, this);
    m_flowLayout->addWidget(card);
}

void SampleCardView::addSampleCard(const QString &iconPath, const QString &title, 
                                   const QString &content, const QString &routeKey, int index)
{
    addSampleCard(QIcon(iconPath), title, content, routeKey, index);
}
