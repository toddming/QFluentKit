#include "HomeInterface.h"
#include "Card/LinkCard.h"
#include "Card/SampleCard.h"
#include "FluentIcon.h"
#include "Theme.h"
#include "StyleSheet.h"
#include "MainWindow.h"

#include "QFluent/ScrollBar.h"

BannerWidget::BannerWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupLinks();
}

void BannerWidget::setupUI()
{
    setFixedHeight(336);

    m_vBoxLayout = new QVBoxLayout(this);
    m_galleryLabel = new QLabel("QFluentKit Gallery", this);
    m_banner = QPixmap(":/res/header.png");
    m_linkCardView = new LinkCardView(this);
    m_linkCardView->setMinimumHeight(300);
    m_galleryLabel->setObjectName("galleryLabel");

    m_vBoxLayout->setSpacing(0);
    m_vBoxLayout->setContentsMargins(0, 20, 0, 0);
    m_vBoxLayout->addWidget(m_galleryLabel);
    m_vBoxLayout->addWidget(m_linkCardView, 1, Qt::AlignBottom);
    m_vBoxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

void BannerWidget::setupLinks()
{
    m_linkCardView->addCard(
                QIcon(":/res/example.png"),
                "快速开始",
                "跟着示例,马上让你的 Qt 应用焕然一新",
                ""
                );

    m_linkCardView->addCard(
                Fluent::icon(Fluent::IconType::GITHUB),
                "设计规范",
                "遵循微软 Fluent Design 系统,提供一致的视觉与交互体验",
                ""
                );

    m_linkCardView->addCard(
                Fluent::icon(Fluent::IconType::CODE),
                "组件示例",
                "查看所有控件的实时演示:按钮、导航栏、卡片、消息框等",
                ""
                );

    m_linkCardView->addCard(
                Fluent::icon(Fluent::IconType::UPDATE),
                "安装指南",
                "支持 Qt 5.15+ Qt6 (MinGW/MSVC),快速集成",
                ""
                );
}

void BannerWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    int w = width();
    int h = height() + 2;
    path.addRoundedRect(QRectF(0, 0, w, h), 10, 10);
    path.addRect(QRectF(0, h-50, 50, 50));
    path.addRect(QRectF(w-50, 0, 50, 50));
    path.addRect(QRectF(w-50, h-50, 50, 50));
    path = path.simplified();

    // 初始化线性渐变效果
    QLinearGradient gradient(0, 0, 0, h);

    // 绘制背景颜色
    if (!Theme::isDark()) {
        gradient.setColorAt(0, QColor(243, 243, 243, 0));
        gradient.setColorAt(1, QColor(247, 249, 252, 255));
    } else {
        gradient.setColorAt(0, QColor(0, 0, 0, 0));
        gradient.setColorAt(1, QColor(39, 39, 39, 255));
    }

    // 绘制横幅图片
    QPixmap scaled = m_banner.scaledToWidth(size().width(), Qt::SmoothTransformation);
    int y = (scaled.height() - size().height()) / 3;
    QPixmap pixmap = scaled.copy(0, y, size().width(), size().height());

    painter.drawPixmap(QRect(0, 0, size().width(), size().height()), pixmap);
    painter.fillPath(path, QBrush(gradient));
}

HomeInterface::HomeInterface(QWidget *parent)
    : ScrollArea(parent)
{
    auto overlayVerticalScrollBar = new ScrollBar(this->verticalScrollBar(), this);
    overlayVerticalScrollBar->setAnimationEnabled(true);

    setGrabGestureEnabled(true); // 支持触摸

    m_view = new QWidget(this);
    m_vBoxLayout = new QVBoxLayout(m_view);
    m_banner = new BannerWidget(this);

    initWidget();
    loadSamples();
}

void HomeInterface::initWidget()
{
    m_view->setObjectName("view");
    setObjectName("homeInterface");

    StyleSheet::registerWidget(this, ":/res/style/{theme}/home_interface.qss");

    setWidget(m_view);
    setWidgetResizable(true);
    setViewportMargins(0, 0, 0, 0);
    m_vBoxLayout->setContentsMargins(0, 0, 0, 36);
    m_vBoxLayout->setSpacing(40);

    m_vBoxLayout->addWidget(m_banner);
    m_vBoxLayout->setAlignment(Qt::AlignTop);
}

void HomeInterface::loadSamples()
{
    // 基础输入样例
    SampleCardView *vistaGeralView = new SampleCardView("功能总览", m_view);
    vistaGeralView->addSampleCard(
        ":/res/controls/IconElement.png",
        "图标",
        "内置丰富SVG图标, 根据深色浅色主题自动切换",
        "iconInterface",
        2
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/Button.png",
        "基本输入",
        "多种样式的按钮、复选框、下拉框、滑动条",
        "basicInputInterface",
        3
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/DatePicker.png",
        "日期和时间",
        "日历选择器、时间选择器",
        "DateTimeInputInterface",
        4
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/ItemsRepeater.png",
        "布局",
        "带动画效果和不带动画效果的流式布局",
        "LayoutInterface",
        5
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/Clipboard.png",
        "菜单",
        "圆角菜单、自定义组件菜单、以及可选中的菜单",
        "MenuInterface",
        6
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/Pivot.png",
        "导航",
        "顶部导航栏和标签导航",
        "NavigationViewInterface",
        7
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/TitleBar.png",
        "窗口",
        "登录窗口示例、左侧导航工具栏窗口、以及分割风格的窗口",
        "WindowInterface",
        8
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/PipsPager.png",
        "滚动",
        "页码切换组件",
        "ScrollInterface",
        9
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/ProgressBar.png",
        "状态",
        "不同弹出位置的信息条、进度条以及进度环",
        "StatusInfoInterface",
        10
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/TextBox.png",
        "文本",
        "文本编辑框、微调框、以及日期时间编辑框",
        "TextInterface",
        11
    );
    vistaGeralView->addSampleCard(
        ":/res/controls/TabView.png",
        "视图",
        "表格控件",
        "ViewInterface",
        12
    );
    m_vBoxLayout->addWidget(vistaGeralView);

    connect(vistaGeralView, &SampleCardView::clicked, this, [=](const QString &routeKey, int index) {
        auto main = qobject_cast<MainWindow*>(this->window());
        main->setCurrentInterface(routeKey, index);
    });
}
