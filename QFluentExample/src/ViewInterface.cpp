#include "ViewInterface.h"

#include "QFluent/TableView.h"

ViewInterface::ViewInterface(QWidget *parent)
    : GalleryInterface("视图", "", parent)
{
    setObjectName("ViewInterface");

    auto tableWidget = new TableWidget(this);
    tableWidget->setFixedSize(725, 440);
    tableWidget->verticalHeader()->hide();
    tableWidget->setBorderVisible(true);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
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

    auto originalSize = demoData.size();
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
}


