#include "ViewInterface.h"

#include "QFluent/ListView.h"
#include "QFluent/TableView.h"
#include "QFluent/PagiNation.h"

ViewInterface::ViewInterface(QWidget *parent)
    : GalleryInterface("视图", "", parent)
{
    setObjectName("ViewInterface");

    auto tableWidget = new TableWidget(this);
    tableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidget->setFixedSize(625, 440);
    tableWidget->verticalHeader()->hide();
    tableWidget->setBorderVisible(true);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QVector<QStringList> demoData = {
        {"会唱歌的饭团", "即食食品", "12", "2025-11-20", "热销"},
        {"隐形墨水笔记本", "文具用品", "5", "长期有效", "限量"},
        {"自动整理袜子机", "生活小物", "3", "——", "售罄预警"},
        {"彩虹味汽水", "饮料", "28", "2026-01-15", "新品"},
        {"防瞌睡薄荷糖", "糖果零食", "45", "2026-03-10", "热销"},
        {"能预知天气的伞", "日用百货", "2", "——", "绝版"},
        {"会讲故事的枕头", "家居用品", "7", "长期有效", "新品"},
        {"一键冷静喷雾", "情绪管理", "19", "2025-12-30", "正常"},
        {"时间暂停咖啡", "饮品", "9", "2025-11-25", "限量"},
        {"自动回消息贴纸", "数码周边", "0", "——", "已售罄"}
    };

    auto originalSize = demoData.size() * 5;
    for (int i = 0; i < originalSize; ++i) {
        demoData.append(demoData[i]);
    }

    int rowCount = demoData.size();
    int columnCount = 5;
    tableWidget->setRowCount(rowCount);
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels({"商品名称", "类型", "库存数量", "保质期", "状态"});

    for (int i = 0; i < rowCount; ++i) {
        for (int j = 0; j < columnCount; ++j) {
            tableWidget->setItem(i, j, new QTableWidgetItem(demoData[i][j]));
        }
    }

    addExampleCard("简单的表格组件", tableWidget);

    auto listWidget = new ListWidget(this);
    listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    QStringList stands = {
        "安卓", "Android Auto", "Android TV", "内置 Google 的汽车",
        "谷歌浏览器", "Chromebook and ChromeOS", "联系人", "数字健康",
        "文件", "财经", "Find Hub", "Fitbit",
        "谷歌航班", "Gboard", "Gemini", "谷歌邮箱",
        "谷歌艺术与文化", "谷歌助手", "谷歌身份验证器", "谷歌日历",
        "Google Cast", "谷歌聊天", "谷歌课堂", "谷歌文档",
        "谷歌云端硬盘", "谷歌地球", "谷歌家庭联系", "Google Fi Wireless",
        "谷歌健康", "Google Fonts", "谷歌表单", "Google Home",
        "谷歌备忘录", "谷歌地图", "谷歌会议", "谷歌信息",
        "Google Nest", "谷歌新闻", "Google One", "谷歌支付",
        "谷歌相册", "Google Play", "Google Play 图书", "Google Play 游戏",
        "Google Play Pass", "Google Play 保护机制", "谷歌表格", "谷歌协作平台",
        "谷歌幻灯片", "谷歌任务", "谷歌翻译", "谷歌趋势",
        "Google TV", "Google TV Streamer", "谷歌语音", "谷歌钱包",
        "Google Workspace", "谷歌智能镜头", "NotebookLM", "Pixel",
        "Pixel Buds", "Pixel 平板电脑", "Pixel Watch", "谷歌学术",
        "谷歌搜索", "谷歌购物", "街景", "谷歌旅行",
        "Waze", "Wear OS by Google", "YouTube", "YouTube Kids",
        "YouTube Music", "YouTube TV", "Android Enterprise", "Blogger",
        "商家资料", "Chrome Enterprise", "ChromeOS", "Demand Gen",
        "谷歌广告", "Google AdMob", "Google AdSense", "Google Ad Manager",
        "谷歌分析", "谷歌助手", "谷歌云", "谷歌地图平台",
        "谷歌营销平台", "谷歌搜索中心", "Google Workspace", "本地服务广告",
        "Manufacturer Center", "Merchant Center", "效果最大化", "Pixel for Business",
        "搜索广告", "购物广告", "Tag Manager", "YouTube 广告",
        "AI for Developers", "安卓", "云计算", "Firebase",
        "Flutter", "Google AdMob", "谷歌广告", "谷歌分析",
        "谷歌云", "Google for Developers", "Google Identity Platform", "谷歌地图平台",
        "谷歌支付", "Google Play", "谷歌钱包", "Interactive Media Ads",
        "谷歌搜索", "TensorFlow", "Web"
    };
    foreach (const QString &stand, stands) {
        auto item = new QListWidgetItem(stand);
        // item->setIcon(QIcon(":/res/Slices.png"));
        listWidget->addItem(item);
    }
    auto listFrame = new Frame(this);
    listFrame->addWidget(listWidget);
    listFrame->setFixedSize(300, 400);

    addExampleCard("简单的列表组件", listFrame);

    auto pagiNation = new PagiNation(this);
    pagiNation->setAlign(Fluent::Alignment::Align_Left);
    pagiNation->setPageSize(10);
    pagiNation->setTotal(500);
    addExampleCard("页码组件", pagiNation);
}

Frame::Frame(QWidget *parent)
    : QFrame(parent)
    , hBoxLayout(new QHBoxLayout(this))
{
    setupUI();
    applyStyleSheet();
}


void Frame::setupUI()
{
    hBoxLayout->setContentsMargins(0, 6, 0, 6);
    setObjectName("frame");
}

void Frame::addWidget(QWidget *widget)
{
    hBoxLayout->addWidget(widget);
}

void Frame::applyStyleSheet()
{
    StyleSheet::registerWidget(this, ":/res/style/{theme}/view_interface.qss");
}
