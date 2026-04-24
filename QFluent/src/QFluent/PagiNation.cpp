#include "PagiNation.h"
#include "Label.h"
#include "ToolButton.h"
#include "FluentIcon.h"
#include <QLayout>
#include <cmath>
#include <algorithm>

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PaginationButton::PaginationButton(const QString& text, QWidget *parent)
    : TransparentTogglePushButton(text, parent)
{
    QSizePolicy CL(QSizePolicy::Maximum, QSizePolicy::Expanding);
    setSizePolicy(CL);
    connect(this, &PaginationButton::clicked, this, &PaginationButton::handleClick);
}

void PaginationButton::handleClick()
{
    if (this->isCheckable()) {
        this->setChecked(true);
    }
    emit cClick(this);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PagiNation::PagiNation (QWidget *parent, Fluent::Alignment align, int buttonCount): QFrame(parent), m_align(align), m_buttonCount(buttonCount)
{
    m_height = DEFAULT_HEIGHT;
    setFixedHeight(m_height + 4);
    init();
}

PagiNation::PagiNation (QPoint point, QWidget *parent, Fluent::Alignment align, int buttonCount): QFrame(parent), m_align(align), m_buttonCount(buttonCount)
{
    m_height = DEFAULT_HEIGHT;
    setFixedHeight(m_height + 4);
    move(point);
    init();
}

PagiNation::PagiNation (QSize size, QWidget *parent, Fluent::Alignment align, int buttonCount): QFrame(parent), m_align(align), m_buttonCount(buttonCount)
{
    m_height = size.height();
    resize(size);
    setMinimumWidth(size.width());
    setFixedHeight(m_height + 4);
    init();
}

PagiNation::PagiNation (QRect rect, QWidget *parent, Fluent::Alignment align, int buttonCount): QFrame(parent), m_align(align), m_buttonCount(buttonCount)
{
    m_height = rect.height();
    move(rect.x(), rect.y());
    resize(rect.size());
    setMinimumWidth(rect.width());
    setFixedHeight(m_height + 4);
    init();
}

// ✅ 删除析构函数 - QLayout会自动管理QSpacerItem
// PagiNation::~PagiNation() { ... } 已删除

/** 验证按钮数量 */
void PagiNation::validatePaginationButtonCount() {
    if (m_buttonCount % 2 != 1 || m_buttonCount < MIN_BUTTON_COUNT) {
        m_buttonCount = DEFAULT_BUTTON_COUNT;
    }
}

void PagiNation::init () {
    validatePaginationButtonCount();

    m_BJ = new QHBoxLayout(this);
    m_BJ->setSpacing(0);
    m_BJ->setContentsMargins(0, 0, 0, 0);

    // ✅ 不再在init中创建TH_left和TH_right
    // 改为在setWidgetAlign()中创建临时对象

    m_prevFBtn = new TransparentToolButton(Fluent::icon(QString(":/qfluent/images/pagination/%1_{color}.svg").arg("Begin")), this);
    m_prevFBtn->setFocusPolicy(Qt::NoFocus);
    m_prevFBtn->setIconSize(QSize(10, 10));
    m_prevFBtn->setFixedSize(m_height, m_height);
    connect(m_prevFBtn, &TransparentToolButton::clicked, this, &PagiNation::toPrev5);

    m_prevBtn = new TransparentToolButton(Fluent::icon(QString(":/qfluent/images/pagination/%1_{color}.svg").arg("Previous")), this);
    m_prevBtn->setFocusPolicy(Qt::NoFocus);
    m_prevBtn->setIconSize(QSize(10, 10));
    m_prevBtn->setFixedSize(m_height, m_height);
    connect(m_prevBtn, &TransparentToolButton::clicked, this, &PagiNation::toPrev1);

    m_mainBox = new QFrame(this);
    QSizePolicy CL(QSizePolicy::Maximum, QSizePolicy::Expanding);

    m_BJMain = new QHBoxLayout(m_mainBox);
    m_BJMain->setSpacing(4);
    m_BJMain->setContentsMargins(0, 0, 0, 0);

    m_mainBox->setSizePolicy(CL);
    m_mainBox->setObjectName("mainBox");
    m_mainBox->setStyleSheet("QFrame#mainBox{margin-left: 4px; margin-right: 4px;}");

    m_nextBtn = new TransparentToolButton(Fluent::icon(QString(":/qfluent/images/pagination/%1_{color}.svg").arg("Next")), this);
    m_nextBtn->setFocusPolicy(Qt::NoFocus);
    m_nextBtn->setIconSize(QSize(10, 10));
    m_nextBtn->setFixedSize(m_height, m_height);
    connect(m_nextBtn, &TransparentToolButton::clicked, this, &PagiNation::toNext1);

    m_nextFBtn = new TransparentToolButton(Fluent::icon(QString(":/qfluent/images/pagination/%1_{color}.svg").arg("End")), this);
    m_nextFBtn->setFocusPolicy(Qt::NoFocus);
    m_nextFBtn->setIconSize(QSize(10, 10));
    m_nextFBtn->setFixedSize(m_height, m_height);
    connect(m_nextFBtn, &TransparentToolButton::clicked, this, &PagiNation::toNext5);

    setWidgetAlign();
    computePage();
}

/** 计算当前页码展示 */
void PagiNation::computePage () {
    // 1. 验证m_pageSize
    if (m_pageSize <= 0) m_pageSize = DEFAULT_PAGE_SIZE;

    // 2. 计算总页数
    int totalNum = ceil(double(m_total) / double(m_pageSize));
    if (totalNum < m_pageNow) m_pageNow = totalNum == 0 ? 1 : totalNum;

    // 3. 只有一页或零页时的处理
    if (totalNum <= 1) {
        m_prevFBtn->setEnabled(false);
        m_prevBtn->setEnabled(false);
        m_nextBtn->setEnabled(false);
        m_nextFBtn->setEnabled(false);

        QList<PaginationData> list;
        PaginationData data = {PaginationItemType::Button, 1, true}; // 显示第1页
        list.push_back(data);
        renderBtn(list, true); // 传入true禁用页码按钮
        return;
    }

    // 4. 正常多页逻辑：设置导航按钮状态
    m_prevFBtn->setEnabled(m_pageNow > 1);
    m_prevBtn->setEnabled(m_pageNow > 1);
    m_nextBtn->setEnabled(m_pageNow < totalNum);
    m_nextFBtn->setEnabled(m_pageNow < totalNum);

    QList<PaginationData> list;

    // 5. 处理页码列表逻辑
    if (totalNum <= m_buttonCount) {
        for (int i = 1; i <= totalNum; i ++) {
            PaginationData data = {PaginationItemType::Button, i, m_pageNow == i};
            list.push_back(data);
        }
    } else {
        int midIndex = (m_buttonCount - 1) / 2;
        int signLocation = midIndex % 2 == 0 ? midIndex / 2 : (midIndex - 1) / 2;

        QString position = "center";
        int midLabel = m_pageNow;
        if (m_pageNow < midIndex + 1) {
            position = "left";
            midLabel = midIndex + 1;
        } else if (totalNum > m_buttonCount && m_pageNow > totalNum - midIndex) {
            // ✅ 修复：添加边界检查
            position = "right";
            midLabel = totalNum - midIndex;
        }

        QVector<PaginationData> leftArr(midIndex), rightArr(midIndex);
        for (int i = 0; i < midIndex; i ++) {
            if (position == "left") {
                leftArr[i] = {PaginationItemType::Button, i + 1, m_pageNow == i + 1};
            } else {
                if (i < signLocation) leftArr[i] = {PaginationItemType::Button, i + 1, false};
                else leftArr[i] = {PaginationItemType::Button, midLabel - (midIndex - i), false};
            }

            if (position == "right") {
                rightArr[i] = {PaginationItemType::Button, totalNum - (midIndex - (i + 1)), m_pageNow == totalNum - (midIndex - (i + 1))};
            } else {
                if (i > midIndex - signLocation - 1) rightArr[i] = {PaginationItemType::Button, totalNum - (midIndex - (i + 1)), false};
                else rightArr[i] = {PaginationItemType::Button, midLabel + (i + 1), false};
            }
        }

        PaginationData midBtnData = {PaginationItemType::Button, midLabel, position != "left" && position != "right"};

        // 拼接列表并处理分隔点 ...
        if (position == "left") {
            for (int i = 0; i < midIndex; i ++) list.push_back(leftArr[i]);
            list.push_back(midBtnData);
            for (int i = 0; i < midIndex; i ++) {
                if (midIndex - signLocation == i) list.push_back({PaginationItemType::Ellipsis, 0, false});
                list.push_back(rightArr[i]);
            }
        } else if (position == "right") {
            for (int i = 0; i < midIndex; i ++) {
                if (signLocation == i) list.push_back({PaginationItemType::Ellipsis, 0, false});
                list.push_back(leftArr[i]);
            }
            list.push_back(midBtnData);
            for (int i = 0; i < midIndex; i ++) list.push_back(rightArr[i]);
        } else {
            for (int i = 0; i < midIndex; i ++) {
                if (signLocation == i) list.push_back({PaginationItemType::Ellipsis, 0, false});
                list.push_back(leftArr[i]);
            }
            list.push_back(midBtnData);
            for (int i = 0; i < midIndex; i ++) {
                if (midIndex - signLocation == i) list.push_back({PaginationItemType::Ellipsis, 0, false});
                list.push_back(rightArr[i]);
            }
        }
    }
    renderBtn(list, false);
}

/** 渲染按钮列表 */
void PagiNation::renderBtn (QList<PaginationData> list, bool allDisabled) {
    // 先取消所有按钮的选中状态，避免闪烁
    QList<PaginationButton*> buttons = m_mainBox->findChildren<PaginationButton*>();
    for (PaginationButton* btn : buttons) {
        if (btn->isCheckable()) {
            btn->setCheckable(false);
        }
    }

    qDeleteAll(m_mainBox->findChildren<PaginationButton*>());
    qDeleteAll(m_mainBox->findChildren<QLabel*>());

    for (const auto& data : list) {
        if (data.type == PaginationItemType::Button) {
            PaginationButton *btn = new PaginationButton(QString::number(data.labelNum), m_mainBox);
            btn->setFocusPolicy(Qt::NoFocus);
            btn->setFixedHeight(m_height);
            connect(btn, &PaginationButton::cClick, this, &PagiNation::handleClick);
            if (data.chosen) {
                btn->setCheckable(true);
                btn->setChecked(true);
            }
            // 如果只有一页，禁用该页码按钮
            if (allDisabled) btn->setEnabled(false);

            btn->show();
            m_BJMain->addWidget(btn);
        } else if (data.type == PaginationItemType::Ellipsis) {
            BodyLabel *dot = new BodyLabel("...", m_mainBox);
            dot->setFixedHeight(m_height);
            dot->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            dot->show();
            m_BJMain->addWidget(dot);
        }
    }
}

/** 设置控件对齐 */
void PagiNation::setWidgetAlign () {
    // ✅ 清除布局中的所有项
    // QLayout 会自动删除 spacer（QLayoutItem 是 QSpacerItem 的基类）
    while (m_BJ->count() > 0) {
        QLayoutItem *item = m_BJ->takeAt(0);
        if (item != nullptr) {
            // takeAt 只是移除，如果是widget会保留
            // 但spacer会被自动删除（由QLayout管理）
            delete item;
        }
    }

    // 重新添加所有widgets
    m_BJ->setSpacing(0);
    m_BJ->setContentsMargins(0, 0, 0, 0);

    m_BJ->addWidget(m_prevFBtn);
    m_BJ->addSpacing(2);
    m_BJ->addWidget(m_prevBtn);
    m_BJ->addWidget(m_mainBox);
    m_BJ->addWidget(m_nextBtn);
    m_BJ->addSpacing(2);
    m_BJ->addWidget(m_nextFBtn);

    // ✅ 添加spacer，让QLayout管理它们（不保存指针）
    if (m_align == Fluent::Alignment::Align_Left) {
        m_BJ->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    } else if (m_align == Fluent::Alignment::Align_Right) {
        m_BJ->insertItem(0, new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    } else {
        m_BJ->insertItem(0, new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
        m_BJ->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }
}

void PagiNation::handleClick (PaginationButton *self) {
    int pageNow = self->text().toInt();
    if (pageNow != m_pageNow) {
        setPage(pageNow, true);
    }
}

void PagiNation::toPrev1 () {
    if (m_pageNow > 1) setPage(--m_pageNow, true);
}

void PagiNation::toPrev5 () {
    if (m_pageNow > 1) setPage(std::max(m_pageNow - 5, 1), true);
}

void PagiNation::toNext1 () {
    if (m_pageSize <= 0) m_pageSize = DEFAULT_PAGE_SIZE;
    int totalNum = ceil(double(m_total) / double(m_pageSize));
    if (m_pageNow < totalNum) setPage(++m_pageNow, true);
}

void PagiNation::toNext5 () {
    if (m_pageSize <= 0) m_pageSize = DEFAULT_PAGE_SIZE;
    int totalNum = ceil(double(m_total) / double(m_pageSize));
    if (m_pageNow < totalNum) {
        int nextPage = std::min(m_pageNow + 5, totalNum);
        setPage(nextPage, true);
    }
}

// ----------------- 公有接口 ---------------------
int PagiNation::page () const {
    return m_pageNow;
}

int PagiNation::pageSize () const {
    return m_pageSize;
}

int PagiNation::total () const {
    return m_total;
}

int PagiNation::buttonCount () const {
    return m_buttonCount;
}

Fluent::Alignment PagiNation::alignment () const {
    return m_align;
}

void PagiNation::setPaginationButtonCount (int buttonCount, bool needEmit) {
    m_buttonCount = buttonCount;
    validatePaginationButtonCount();
    computePage();
    if (needEmit) emit pageChanged(m_pageNow, m_pageSize);
}

void PagiNation::setAlign (Fluent::Alignment align, bool needEmit) {
    if (align == m_align) return;
    m_align = align;
    setWidgetAlign();
    if (needEmit) emit pageChanged(m_pageNow, m_pageSize);
}

void PagiNation::setPageSize (int pageSize, bool needEmit) {
    m_pageNow = 1;
    m_pageSize = (pageSize > 0) ? pageSize : DEFAULT_PAGE_SIZE;  // 验证
    computePage();
    if (needEmit) emit pageChanged(m_pageNow, m_pageSize);
}

void PagiNation::setTotal (int total, bool needEmit) {
    m_total = (total < 0) ? 0 : total;
    computePage();
    if (needEmit) emit pageChanged(m_pageNow, m_pageSize);
}

void PagiNation::setPage (int pageNow, bool needEmit) {
    m_pageNow = (pageNow > 0) ? pageNow : 1;  // 验证页码
    computePage();
    if (needEmit) emit pageChanged(m_pageNow, m_pageSize);
}

void PagiNation::setPage (int pageNow, int total, bool needEmit) {
    m_pageNow = (pageNow > 0) ? pageNow : 1;  // 验证页码
    m_total = (total < 0) ? 0 : total;
    computePage();
    if (needEmit) emit pageChanged(m_pageNow, m_pageSize);
}

void PagiNation::connectPageChange (function<void(int, int)> method) {
    connect(this, &PagiNation::pageChanged, method);
}

void PagiNation::connectPageChange (function<void(int)> method) {
    connect(this, &PagiNation::pageChanged, method);
}

void PagiNation::connectPageChange (function<void()> method) {
    connect(this, &PagiNation::pageChanged, method);
}
