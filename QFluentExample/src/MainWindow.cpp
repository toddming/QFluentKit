#include "MainWindow.h"

#include "Router.h"
#include "Window/FluentTitleBar.h"
#include "QFluent/Dialog/MessageDialog.h"

#include "FluentIcon.h"
#include "HomeInterface.h"
#include "IconInterface.h"
#include "BasicInputInterface.h"
#include "SettingInterface.h"
#include "DateTimeInputInterface.h"
#include "DialogInputInterface.h"
#include "LayoutInterface.h"
#include "MenuInterface.h"
#include "NavigationViewInterface.h"
#include "MaterialInterface.h"
#include "StatusInfoInterface.h"
#include "TextInterface.h"
#include "ViewInterface.h"
#include "WindowInterface.h"

#include "ConfigManager.h"

#ifdef _WIN32
#if !USE_QWINDOWKIT
#include <dwmapi.h>
#endif
#endif

using FIT = Fluent::IconType;
using NIP = Fluent::NavigationItemPosition;

MainWindow::MainWindow()
{
    setWindowTitle("QFluentKit");
    setWindowIcon(QPixmap(":/res/example.png"));
    resize(1024, 768);

    setWindowButtonHints(WindowButtonHint::WindowIcon | WindowButtonHint::Title |
                         WindowButtonHint::Minimize | WindowButtonHint::Maximize |
                         WindowButtonHint::Close | WindowButtonHint::ThemeToggle |
                         WindowButtonHint::RouteBack);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    m_navPanel = new NavigationPanel(this);
    m_stacked = new StackedWidget(this);
    layout->addWidget(m_navPanel, 0);
    layout->addWidget(m_stacked, 1);


#if USE_QWINDOWKIT
    setContentsMargins(0, 48, 0, 0);

    auto title = titleBar();
    connect(qrouter, &Router::emptyChanged, this, [title](bool empty){
        title->backButton()->setEnabled(!empty);
    });
#else
    setContentsMargins(0, 5, 0, 0);
#endif

    m_navPanel->setExpandWidth(240);

    initWidget();
}

void MainWindow::initWidget()
{
    auto userCard = m_navPanel->addUserCard("userCard", ":/res/Shizuka.png", "Shizuka", "shizuka@gmail.com",
                                       nullptr, NIP::TOP, false);
    userCard->setTitleFontSize(12);
    userCard->setSubtitleFontSize(10);

    QString iconPath = QString(":/res/icons/%1_{color}.svg");
    addSubInterface("1", FluentIcon(FIT::HOME), "主页", new HomeInterface(this), true, NIP::TOP, "主页");
    addSubInterface("2", FluentIcon(FIT::EMOJI_TAB_SYMBOLS), "图标", new IconInterface(this), true, NIP::TOP, "图标");
    m_navPanel->addSeparator();
    addSubInterface("3", FluentIcon(FIT::CHECKBOX), "基本输入", new BasicInputInterface(this), true, NIP::SCROLL, "基本输入");
    addSubInterface("4", FluentIcon(FIT::DATE_TIME), "日期和时间", new DateTimeInputInterface(this), true, NIP::SCROLL, "日期和时间");
    addSubInterface("5", FluentIcon(FIT::MESSAGE), "对话框", new DialogInputInterface(this), true, NIP::SCROLL, "对话框");
    addSubInterface("6", FluentIcon(FIT::LAYOUT), "布局", new LayoutInterface(this), true, NIP::SCROLL, "布局");
    addSubInterface("7", FluentIcon(iconPath.arg("Menu")), "菜单", new MenuInterface(this), true, NIP::SCROLL, "菜单");
    addSubInterface("8", FluentIcon(FIT::MENU), "导航", new NavigationViewInterface(this), true, NIP::SCROLL, "导航");
    addSubInterface("9", FluentIcon(FIT::APPLICATION), "窗口", new WindowInterface(this), true, NIP::SCROLL, "窗口");
    addSubInterface("10", FluentIcon(FIT::PALETTE), "材料", new MaterialInterface(this), true, NIP::SCROLL, "材料");
    addSubInterface("11", FluentIcon(FIT::CHAT), "状态", new StatusInfoInterface(this), true, NIP::SCROLL, "状态");
    addSubInterface("12", FluentIcon(iconPath.arg("Text")), "文本", new TextInterface(this), true, NIP::SCROLL, "文本");
    addSubInterface("13", FluentIcon(iconPath.arg("Grid")), "视图", new ViewInterface(this), true, NIP::SCROLL, "视图");

    m_navPanel->addSeparator(NIP::BOTTOM);
    addSubInterface("14", FluentIcon(FIT::SETTING), "设置", new SettingInterface(this), true, NIP::BOTTOM, "设置");

    qrouter->setDefaultRouteKey(m_stacked, "homeInterface");
    m_navPanel->setCurrentItem("1");

    // connect(this, &MainWindow::backRequested, this, [=](){
    //     qrouter->pop();
    //     m_navPanel->setCurrentItem(QString::number(m_stacked->currentIndex() + 1));
    // });
    connect(userCard, &NavigationUserCard::clicked, this, [this](){
        showDialog();
    });
}

void MainWindow::setCurrentInterface(const QString &routeKey, int index)
{
    qrouter->push(m_stacked, routeKey);
    m_navPanel->setCurrentItem(QString::number(index));
}

void MainWindow::showDialog()
{
    auto box = new MessageDialog("你是遇到问题了吗🧐",
                                 "遇到问题？欢迎加我 QQ（1912229135）反馈～看到后我会第一时间修复，感谢你让这个项目变得越来越棒！",
                                 this->window());
    box->setIsClosableOnMaskClicked(true);
    box->exec();
}


void MainWindow::addSubInterface(const QString& routeKey, const FluentIconBase& icon, const QString& text,
                                   QWidget* widget, bool selectable,
                                   Fluent::NavigationItemPosition position, const QString& tooltip,
                                   const QString& parentRouteKey)
{
    m_navPanel->addItem(routeKey, icon, text, [=](){qrouter->push(m_stacked, widget->objectName());}, selectable, position, tooltip, parentRouteKey);
    m_stacked->addWidget(widget);
}
