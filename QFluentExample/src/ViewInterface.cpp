#include "ViewInterface.h"

#include "QFluent/ListView.h"
#include "QFluent/TableView.h"

ViewInterface::ViewInterface(QWidget *parent)
    : GalleryInterface("视图", "", parent)
{
    setObjectName("ViewInterface");

    auto tableWidget = new TableWidget(this);
    tableWidget->setFixedSize(725, 440);
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
    QStringList stands = {
        "隐者之紫", "黄金体验", "虚无之王", "纸月之王",
        "骇人恶兽", "男子领域", "20世纪男孩", "牙 Act 4",
        "铁球破坏者", "性感手枪", "天生完美",
        "软又湿", "佩斯利公园", "奇迹于你", "行走的心",
        "护霜旅行者", "十一月雨", "调情圣手", "片刻静候"
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
    hBoxLayout->setContentsMargins(0, 8, 0, 0);
    setObjectName("frame");
}

void Frame::addWidget(QWidget *widget)
{
    hBoxLayout->addWidget(widget);
}

void Frame::applyStyleSheet()
{
    auto styleSource = std::make_shared<TemplateStyleSheetFile>(":/res/style/{theme}/view_interface.qss");
    StyleSheetManager::instance()->registerWidget(styleSource, this);
}
