#include "SplitWindow.h"

#include <QHBoxLayout>

#include "StyleSheet.h"
#include "QFluent/Label.h"
#include "QFluent/StackedWidget.h"
#include "QFluent/Navigation/NavigationBar.h"

using FIT = Fluent::IconType;
using NIP = Fluent::NavigationItemPosition;

SplitWidget::SplitWidget()
{

    setWindowButtonHints(WindowButtonHint::Icon | WindowButtonHint::Title |
                         WindowButtonHint::Minimize | WindowButtonHint::Maximize |
                         WindowButtonHint::Close | WindowButtonHint::RouteBack);

    setWindowEffect(WindowEffect::Normal);

    setContentsMargins(0, 0, 0, 0);

    resize(800, 600);

    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));

    m_navPanel = new NavigationPanel(this);
    m_stacked = new StackedWidget(this);
    m_stacked->setProperty("noRadius", true);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(m_navPanel);
    hBoxLayout->setSpacing(0);
    hBoxLayout->setContentsMargins(0, 48, 0, 0);

    layout->addLayout(hBoxLayout, 0);
    layout->addWidget(m_stacked, 1);

    m_navPanel->setExpandWidth(200);

    initWidget();
}

void SplitWidget::initWidget()
{

    addSubInterface("1", FluentIcon(FIT::HOME), "主页", createWidget("主页", this), true, NIP::TOP);
    m_navPanel->addSeparator();
    m_navPanel->addItemHeader("基础", NIP::SCROLL);
    addSubInterface("3", FluentIcon(FIT::CHAT), "对话", createWidget("对话", this), true, NIP::SCROLL);
    addSubInterface("4", FluentIcon(FIT::GAME), "游戏", createWidget("游戏", this), true, NIP::SCROLL);

    m_navPanel->addItemHeader("进阶", NIP::SCROLL);
    addSubInterface("5", FluentIcon(FIT::LEAF), "能效", createWidget("能效", this), true, NIP::SCROLL);
    addSubInterface("6", FluentIcon(FIT::FINGERPRINT), "加密", createWidget("加密", this), true, NIP::SCROLL);
    m_navPanel->addSeparator(NIP::BOTTOM);
    auto avatarWidget = new NavigationAvatarWidget("Administrator", QImage(":/res/avatar.png"), this);
    m_navPanel->addWidget("7", avatarWidget, nullptr, NIP::BOTTOM);

    addSubInterface("8", FluentIcon(FIT::SETTING), "设置", createWidget("设置", this), true, NIP::BOTTOM);

    m_navPanel->setCurrentItem("1");
}

QWidget* SplitWidget::createWidget(const QString &text, QWidget* parent)
{
    auto w = new QWidget(parent);
    auto hLay = new QHBoxLayout(w);
    auto label = new DisplayLabel(text, w);
    label->setAlignment(Qt::AlignCenter);
    hLay->addWidget(label);
    return w;
}

void SplitWidget::addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                                   QWidget* widget, bool selectable,
                                   Fluent::NavigationItemPosition position, const QString& tooltip,
                                   const QString& parentRouteKey)
{
    m_navPanel->addItem(routeKey, icon, text, [=](){m_stacked->setCurrentWidget(widget, false);}, selectable, position, tooltip, parentRouteKey);
    m_stacked->addWidget(widget);
}
