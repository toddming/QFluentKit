#include "PagiNation.h"
#include "Label.h"
#include "ToolButton.h"
#include <QLayout>
#include <cmath>
#include <algorithm>

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Button::Button(const QString& text, QWidget *parent)
    : TransparentTogglePushButton(text, parent)
{
    QSizePolicy CL(QSizePolicy::Maximum, QSizePolicy::Expanding);
    setSizePolicy(CL);
    connect(this, SIGNAL(clicked()), this, SLOT(handleClick()));
}

void Button::handleClick()
{
    if (this->isCheckable()) {
        this->setChecked(true);
    }
    emit cClick(this);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PagiNation::PagiNation (QWidget *parent, Fluent::Alignment align, int buttonCount): QFrame(parent), _align(align), _buttonCount(buttonCount)
{
    _height = DEFAULT_HEIGHT;
    setFixedHeight(_height + 4);
    init();
}

PagiNation::PagiNation (QPoint point, QWidget *parent, Fluent::Alignment align, int buttonCount): QFrame(parent), _align(align), _buttonCount(buttonCount)
{
    _height = DEFAULT_HEIGHT;
    setFixedHeight(_height + 4);
    move(point);
    init();
}

PagiNation::PagiNation (QSize size, QWidget *parent, Fluent::Alignment align, int buttonCount): QFrame(parent), _align(align), _buttonCount(buttonCount)
{
    _height = size.height();
    resize(size);
    setMinimumWidth(size.width());
    setFixedHeight(_height + 4);
    init();
}

PagiNation::PagiNation (QRect rect, QWidget *parent, Fluent::Alignment align, int buttonCount): QFrame(parent), _align(align), _buttonCount(buttonCount)
{
    _height = rect.height();
    move(rect.x(), rect.y());
    resize(rect.size());
    setMinimumWidth(rect.width());
    setFixedHeight(_height + 4);
    init();
}

// ✅ 删除析构函数 - QLayout会自动管理QSpacerItem
// PagiNation::~PagiNation() { ... } 已删除

/** 验证按钮数量 */
void PagiNation::validateButtonCount() {
    if (_buttonCount % 2 != 1 || _buttonCount < MIN_BUTTON_COUNT) {
        _buttonCount = DEFAULT_BUTTON_COUNT;
    }
}

void PagiNation::init () {
    validateButtonCount();

    BJ = new QHBoxLayout(this);
    BJ->setSpacing(0);
    BJ->setContentsMargins(0, 0, 0, 0);

    // ✅ 不再在init中创建TH_left和TH_right
    // 改为在setWidgetAlign()中创建临时对象

    prevFBtn = new TransparentToolButton(FluentIcon(QString(":/res/images/pagination/%1_{color}.svg").arg("Begin")), this);
    prevFBtn->setFocusPolicy(Qt::NoFocus);
    prevFBtn->setIconSize(QSize(10, 10));
    prevFBtn->setFixedSize(_height, _height);
    connect(prevFBtn, SIGNAL(clicked()), this, SLOT(toPrev5()));

    prevBtn = new TransparentToolButton(FluentIcon(QString(":/res/images/pagination/%1_{color}.svg").arg("Previous")), this);
    prevBtn->setFocusPolicy(Qt::NoFocus);
    prevBtn->setIconSize(QSize(10, 10));
    prevBtn->setFixedSize(_height, _height);
    connect(prevBtn, SIGNAL(clicked()), this, SLOT(toPrev1()));

    mainBox = new QFrame(this);
    QSizePolicy CL(QSizePolicy::Maximum, QSizePolicy::Expanding);

    BJ_main = new QHBoxLayout(mainBox);
    BJ_main->setSpacing(4);
    BJ_main->setContentsMargins(0, 0, 0, 0);

    mainBox->setSizePolicy(CL);
    mainBox->setObjectName("mainBox");
    mainBox->setStyleSheet("QFrame#mainBox{margin-left: 4px; margin-right: 4px;}");

    nextBtn = new TransparentToolButton(FluentIcon(QString(":/res/images/pagination/%1_{color}.svg").arg("Next")), this);
    nextBtn->setFocusPolicy(Qt::NoFocus);
    nextBtn->setIconSize(QSize(10, 10));
    nextBtn->setFixedSize(_height, _height);
    connect(nextBtn, SIGNAL(clicked()), this, SLOT(toNext1()));

    nextFBtn = new TransparentToolButton(FluentIcon(QString(":/res/images/pagination/%1_{color}.svg").arg("End")), this);
    nextFBtn->setFocusPolicy(Qt::NoFocus);
    nextFBtn->setIconSize(QSize(10, 10));
    nextFBtn->setFixedSize(_height, _height);
    connect(nextFBtn, SIGNAL(clicked()), this, SLOT(toNext5()));

    setWidgetAlign();
    computePage();
}

/** 计算当前页码展示 */
void PagiNation::computePage () {
    // 1. 验证_pageSize
    if (_pageSize <= 0) _pageSize = DEFAULT_PAGE_SIZE;

    // 2. 计算总页数
    int totalNum = ceil(double(_total) / double(_pageSize));
    if (totalNum < _pageNow) _pageNow = totalNum == 0 ? 1 : totalNum;

    // 3. 只有一页或零页时的处理
    if (totalNum <= 1) {
        prevFBtn->setEnabled(false);
        prevBtn->setEnabled(false);
        nextBtn->setEnabled(false);
        nextFBtn->setEnabled(false);

        QList<__PagiNation_DATA> list;
        __PagiNation_DATA data = {PaginationItemType::Button, 1, true}; // 显示第1页
        list.push_back(data);
        renderBtn(list, true); // 传入true禁用页码按钮
        return;
    }

    // 4. 正常多页逻辑：设置导航按钮状态
    prevFBtn->setEnabled(_pageNow > 1);
    prevBtn->setEnabled(_pageNow > 1);
    nextBtn->setEnabled(_pageNow < totalNum);
    nextFBtn->setEnabled(_pageNow < totalNum);

    QList<__PagiNation_DATA> list;

    // 5. 处理页码列表逻辑
    if (totalNum <= _buttonCount) {
        for (int i = 1; i <= totalNum; i ++) {
            __PagiNation_DATA data = {PaginationItemType::Button, i, _pageNow == i};
            list.push_back(data);
        }
    } else {
        int midIndex = (_buttonCount - 1) / 2;
        int signLocation = midIndex % 2 == 0 ? midIndex / 2 : (midIndex - 1) / 2;

        QString position = "center";
        int midLabel = _pageNow;
        if (_pageNow < midIndex + 1) {
            position = "left";
            midLabel = midIndex + 1;
        } else if (totalNum > _buttonCount && _pageNow > totalNum - midIndex) {
            // ✅ 修复：添加边界检查
            position = "right";
            midLabel = totalNum - midIndex;
        }

        QVector<__PagiNation_DATA> leftArr(midIndex), rightArr(midIndex);
        for (int i = 0; i < midIndex; i ++) {
            if (position == "left") {
                leftArr[i] = {PaginationItemType::Button, i + 1, _pageNow == i + 1};
            } else {
                if (i < signLocation) leftArr[i] = {PaginationItemType::Button, i + 1, false};
                else leftArr[i] = {PaginationItemType::Button, midLabel - (midIndex - i), false};
            }

            if (position == "right") {
                rightArr[i] = {PaginationItemType::Button, totalNum - (midIndex - (i + 1)), _pageNow == totalNum - (midIndex - (i + 1))};
            } else {
                if (i > midIndex - signLocation - 1) rightArr[i] = {PaginationItemType::Button, totalNum - (midIndex - (i + 1)), false};
                else rightArr[i] = {PaginationItemType::Button, midLabel + (i + 1), false};
            }
        }

        __PagiNation_DATA midBtnData = {PaginationItemType::Button, midLabel, position != "left" && position != "right"};

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
void PagiNation::renderBtn (QList<__PagiNation_DATA> list, bool allDisabled) {
    // 先取消所有按钮的选中状态，避免闪烁
    QList<Button*> buttons = mainBox->findChildren<Button*>();
    for (Button* btn : buttons) {
        if (btn->isCheckable()) {
            btn->setCheckable(false);
        }
    }

    qDeleteAll(mainBox->findChildren<Button*>());
    qDeleteAll(mainBox->findChildren<QLabel*>());

    for (const auto& data : list) {
        if (data.type == PaginationItemType::Button) {
            Button *btn = new Button(QString::number(data.labelNum), mainBox);
            btn->setFocusPolicy(Qt::NoFocus);
            btn->setFixedHeight(_height);
            connect(btn, SIGNAL(cClick(Button*)), this, SLOT(handleClick(Button*)));
            if (data.choosed) {
                btn->setCheckable(true);
                btn->setChecked(true);
            }
            // 如果只有一页，禁用该页码按钮
            if (allDisabled) btn->setEnabled(false);

            btn->show();
            BJ_main->addWidget(btn);
        } else if (data.type == PaginationItemType::Ellipsis) {
            BodyLabel *dot = new BodyLabel("...", mainBox);
            dot->setFixedHeight(_height);
            dot->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            dot->show();
            BJ_main->addWidget(dot);
        }
    }
}

/** 设置控件对齐 */
void PagiNation::setWidgetAlign () {
    // ✅ 清除布局中的所有项
    // QLayout 会自动删除 spacer（QLayoutItem 是 QSpacerItem 的基类）
    while (BJ->count() > 0) {
        QLayoutItem *item = BJ->takeAt(0);
        if (item != nullptr) {
            // takeAt 只是移除，如果是widget会保留
            // 但spacer会被自动删除（由QLayout管理）
            delete item;
        }
    }

    // 重新添加所有widgets
    BJ->setSpacing(0);
    BJ->setContentsMargins(0, 0, 0, 0);

    BJ->addWidget(prevFBtn);
    BJ->addSpacing(2);
    BJ->addWidget(prevBtn);
    BJ->addWidget(mainBox);
    BJ->addWidget(nextBtn);
    BJ->addSpacing(2);
    BJ->addWidget(nextFBtn);

    // ✅ 添加spacer，让QLayout管理它们（不保存指针）
    if (_align == Fluent::Alignment::Align_Left) {
        BJ->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    } else if (_align == Fluent::Alignment::Align_Right) {
        BJ->insertItem(0, new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    } else {
        BJ->insertItem(0, new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
        BJ->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }
}

void PagiNation::handleClick (Button *self) {
    int pageNow = self->text().toInt();
    if (pageNow != _pageNow) {
        setPage(pageNow, true);
    }
}

void PagiNation::toPrev1 () {
    if (_pageNow > 1) setPage(--_pageNow, true);
}

void PagiNation::toPrev5 () {
    if (_pageNow > 1) setPage(std::max(_pageNow - 5, 1), true);
}

void PagiNation::toNext1 () {
    if (_pageSize <= 0) _pageSize = DEFAULT_PAGE_SIZE;
    int totalNum = ceil(double(_total) / double(_pageSize));
    if (_pageNow < totalNum) setPage(++_pageNow, true);
}

void PagiNation::toNext5 () {
    if (_pageSize <= 0) _pageSize = DEFAULT_PAGE_SIZE;
    int totalNum = ceil(double(_total) / double(_pageSize));
    if (_pageNow < totalNum) {
        int nextPage = std::min(_pageNow + 5, totalNum);
        setPage(nextPage, true);
    }
}

// ----------------- 公有接口 ---------------------
int PagiNation::page () {
    return _pageNow;
}

int PagiNation::pageSize () {
    return _pageSize;
}

int PagiNation::total () {
    return _total;
}

int PagiNation::buttonCount () {
    return _buttonCount;
}

Fluent::Alignment PagiNation::alignment () {
    return _align;
}

void PagiNation::setButtonCount (int buttonCount, bool needEmit) {
    _buttonCount = buttonCount;
    validateButtonCount();
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

void PagiNation::setAlign (Fluent::Alignment align, bool needEmit) {
    if (align == _align) return;
    _align = align;
    setWidgetAlign();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

void PagiNation::setPageSize (int pageSize, bool needEmit) {
    _pageNow = 1;
    _pageSize = (pageSize > 0) ? pageSize : DEFAULT_PAGE_SIZE;  // 验证
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

void PagiNation::setTotal (int total, bool needEmit) {
    _total = (total < 0) ? 0 : total;
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

void PagiNation::setPage (int pageNow, bool needEmit) {
    _pageNow = (pageNow > 0) ? pageNow : 1;  // 验证页码
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

void PagiNation::setPage (int pageNow, int total, bool needEmit) {
    _pageNow = (pageNow > 0) ? pageNow : 1;  // 验证页码
    _total = (total < 0) ? 0 : total;
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
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
