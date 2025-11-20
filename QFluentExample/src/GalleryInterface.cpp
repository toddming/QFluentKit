#include "GalleryInterface.h"

#include "FluentIcon.h"
#include "Theme.h"
#include "StyleSheet.h"

SeparatorWidget::SeparatorWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(6, 16);
}

void SeparatorWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QPen pen(Qt::SolidLine);
    pen.setCosmetic(true);

    QColor c = Theme::instance()->isDarkTheme() ? QColor(255, 255, 255, 21) : QColor(0, 0, 0, 15);
    pen.setColor(c);
    painter.setPen(pen);

    int x = width() / 2;
    painter.drawLine(x, 0, x, height());
}

ToolBar::ToolBar(const QString &title, const QString &subtitle, QWidget *parent)
    : QWidget(parent)
{
    titleLabel = new TitleLabel(title, this);
    Theme::instance()->setFont(titleLabel, 28, QFont::Normal);
    subtitleLabel = new CaptionLabel(subtitle, this);
    subtitleLabel->setTextColor(QColor(96, 96, 96), QColor(216, 216, 216));

    documentButton = new PushButton("在线文档", FluentIcon(Fluent::IconType::DOCUMENT), this);
    sourceButton = new PushButton("源代码", FluentIcon(Fluent::IconType::GITHUB), this);
    themeButton = new ToolButton(FluentIcon(Fluent::IconType::CONSTRACT), this);
    separator = new SeparatorWidget(this);
    supportButton = new ToolButton(FluentIcon(Fluent::IconType::HEART), this);
    feedbackButton = new ToolButton(FluentIcon(Fluent::IconType::FEEDBACK), this);

    vBoxLayout = new QVBoxLayout(this);
    buttonLayout = new QHBoxLayout();

    initWidget();
}

void ToolBar::initWidget()
{
    setFixedHeight(138);
    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(36, 22, 36, 12);
    vBoxLayout->addWidget(titleLabel);
    vBoxLayout->addSpacing(4);
    vBoxLayout->addWidget(subtitleLabel);
    vBoxLayout->addSpacing(4);
    vBoxLayout->addLayout(buttonLayout, 1);
    vBoxLayout->setAlignment(Qt::AlignTop);

    buttonLayout->setSpacing(4);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addWidget(documentButton, 0, Qt::AlignLeft);
    buttonLayout->addWidget(sourceButton, 0, Qt::AlignLeft);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(themeButton, 0, Qt::AlignRight);
    buttonLayout->addWidget(separator, 0, Qt::AlignRight);
    buttonLayout->addWidget(supportButton, 0, Qt::AlignRight);
    buttonLayout->addWidget(feedbackButton, 0, Qt::AlignRight);
    buttonLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);



    // 假设setTextColor方法存在
    // subtitleLabel->setTextColor(QColor(96, 96, 96), QColor(216, 216, 216));
}

ExampleCard::ExampleCard(const QString &title, QWidget *widget, const QString &sourcePath,
                         int stretch, QWidget *parent)
    : QWidget(parent)
    , widget(widget)
    , stretch(stretch)
    , sourcePath(sourcePath)
{
    titleLabel = new StrongBodyLabel(title, this);
    Theme::instance()->setFont(titleLabel, 14, QFont::Normal);
    card = new QFrame(this);
    sourceWidget = new QFrame(card);
    sourcePathLabel = new BodyLabel("源代码", sourceWidget);
    linkIcon = new IconWidget(FluentIcon(Fluent::IconType::LINK), sourceWidget);

    vBoxLayout = new QVBoxLayout(this);
    cardLayout = new QVBoxLayout(card);
    topLayout = new QHBoxLayout();
    bottomLayout = new QHBoxLayout(sourceWidget);

    initWidget();
}

void ExampleCard::initWidget()
{
    linkIcon->setFixedSize(16, 16);
    initLayout();

    sourceWidget->setCursor(Qt::PointingHandCursor);
    sourceWidget->installEventFilter(this);

    card->setObjectName("card");
    sourceWidget->setObjectName("sourceWidget");
}

void ExampleCard::initLayout()
{
    vBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);
    cardLayout->setSizeConstraint(QLayout::SetMinimumSize);
    topLayout->setSizeConstraint(QLayout::SetMinimumSize);

    vBoxLayout->setSpacing(12);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setContentsMargins(12, 12, 12, 12);
    bottomLayout->setContentsMargins(18, 18, 18, 18);
    cardLayout->setContentsMargins(0, 0, 0, 0);

    vBoxLayout->addWidget(titleLabel, 0, Qt::AlignTop);
    vBoxLayout->addWidget(card, 0, Qt::AlignTop);
    vBoxLayout->setAlignment(Qt::AlignTop);

    cardLayout->setSpacing(0);
    cardLayout->setAlignment(Qt::AlignTop);
    cardLayout->addLayout(topLayout, 0);
    cardLayout->addWidget(sourceWidget, 0, Qt::AlignBottom);

    widget->setParent(card);
    topLayout->addWidget(widget);
    if (stretch == 0) {
        topLayout->addStretch(1);
    }

    widget->show();

    bottomLayout->addWidget(sourcePathLabel, 0, Qt::AlignLeft);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(linkIcon, 0, Qt::AlignRight);
    bottomLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

bool ExampleCard::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == sourceWidget && e->type() == QEvent::MouseButtonRelease) {
        QDesktopServices::openUrl(QUrl(sourcePath));
    }
    return QWidget::eventFilter(obj, e);
}

GalleryInterface::GalleryInterface(const QString &title, const QString &subtitle, QWidget *parent)
    : ScrollArea(parent)
{
    view = new QWidget(this);
    toolBar = new ToolBar(title, subtitle.isEmpty() ? "https://github.com/toddming/QFluentExample" : subtitle, this);
    vBoxLayout = new QVBoxLayout(view);

    setViewportMargins(0, toolBar->height(), 0, 0);
    setWidget(view);
    setWidgetResizable(true);

    vBoxLayout->setSpacing(30);
    vBoxLayout->setAlignment(Qt::AlignTop);
    vBoxLayout->setContentsMargins(36, 20, 36, 36);

    view->setObjectName("view");

    auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/gallery_interface.qss");
    StyleSheetManager::instance()->registerWidget(styleSource, this);
}

ExampleCard* GalleryInterface::addExampleCard(const QString &title, QWidget *widget,
                                              const QString &sourcePath, int stretch)
{
    ExampleCard *card = new ExampleCard(title, widget, sourcePath, stretch, view);
    vBoxLayout->addWidget(card, 0, Qt::AlignTop);
    return card;
}

void GalleryInterface::scrollToCard(int index)
{

}

void GalleryInterface::resizeEvent(QResizeEvent *e)
{
    ScrollArea::resizeEvent(e);
    toolBar->resize(width(), toolBar->height());
}

QLayout *GalleryInterface::layout() const
{
    return vBoxLayout;
}
