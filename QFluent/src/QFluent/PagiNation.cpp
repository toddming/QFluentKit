#include "PagiNation.h"

#include "Label.h"
#include "ToolButton.h"
#include <QLayout>

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

PagiNation::PagiNation (QWidget *parent, PagiNationType::Alignment align, int buttonCount): QFrame(parent), _align(align), _buttonCount(buttonCount)
{
    _height = 30;
    setFixedHeight(_height + 4);
    init();
}

PagiNation::PagiNation (QPoint point, QWidget *parent, PagiNationType::Alignment align, int buttonCount): QFrame(parent), _align(align), _buttonCount(buttonCount)
{
    _height = 30;
    setFixedHeight(_height + 4);
    move(point);
    init();
}

PagiNation::PagiNation (QSize size, QWidget *parent, PagiNationType::Alignment align, int buttonCount): QFrame(parent), _align(align), _buttonCount(buttonCount)
{
    _height = size.height();
    resize(size);
    setMinimumWidth(size.width());
    setFixedHeight(_height + 4);
    init();
}

PagiNation::PagiNation (QRect rect, QWidget *parent, PagiNationType::Alignment align, int buttonCount): QFrame(parent), _align(align), _buttonCount(buttonCount)
{
    _height = rect.height();
    move(rect.x(), rect.y());
    resize(rect.size());
    setMinimumWidth(rect.width());
    setFixedHeight(_height + 4);
    init();
}

// ----------------- 私有方法 ---------------------

void PagiNation::init () {

    if (_buttonCount % 2 != 1 || _buttonCount < 5) _buttonCount = 7;

    BJ = new QHBoxLayout(this);
    BJ->setSpacing(0);
    BJ->setContentsMargins(0, 0, 0, 0);

    TH_left = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    TH_right = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

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

    QSizePolicy CL(QSizePolicy::Maximum, QSizePolicy::Expanding); // 布局策略

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

    BJ->addWidget(prevFBtn);
    BJ->addSpacing(2);
    BJ->addWidget(prevBtn);
    BJ->addWidget(mainBox);
    BJ->addWidget(nextBtn);
    BJ->addSpacing(2);
    BJ->addWidget(nextFBtn);

    setWidgetAlign();

    computePage();
}

/** 计算当前页码展示 */
void PagiNation::computePage () {
    QList<__PagiNation_DATA> list;

    // 页码总数
    int totalNum = ceil(double(_total) / double(_pageSize));

    if (totalNum < _pageNow) _pageNow = totalNum == 0 ? 1 : totalNum;

    if (totalNum == 0) {
        __PagiNation_DATA data = {1, 1, true};
        list.push_back(data);
        renderBtn(list);
        return;
    }

    if (totalNum <= _buttonCount) { // 页码总数小于等于按钮数量, 直接返回页码总数条数据
        for (int i = 1; i <= totalNum; i ++) {
            __PagiNation_DATA data = {1, i, _pageNow == i};
            list.push_back(data);
        }
        renderBtn(list);
        return;
    }

    int midIndex = (_buttonCount - 1) / 2; // 左侧按钮数量(即中间按钮索引)
    int signLocation = midIndex % 2 == 0 ? midIndex / 2 : (midIndex - 1) / 2;

    QVector<int> _leftArr(midIndex), _rightArr(midIndex); // 预设左右边按钮集合
    for (int i = 1; i <= midIndex; i ++) {
        _leftArr[i - 1] = i;
        _rightArr[i - 1] = i + midIndex + 1;
    }

    QString position = "center"; // 当前页码所在位置, 左边(左边无需分隔点), 左侧中间按钮位置(左边无需分隔点), 中间(左右都需要分隔点), 右侧中间按钮位置(右边无需分隔点), 右边(右边无需分隔点)
    int midLabel = _pageNow; // 中间按钮展示的页码数据
    if (_pageNow < midIndex + 1) {
        position = "left";
        midLabel = midIndex + 1;
    } else if (_pageNow == midIndex + 1) {
        position = "centerLeft";
    } else if (_pageNow > totalNum - midIndex) {
        position = "right";
        midLabel = totalNum - midIndex;
    } else if (_pageNow == totalNum - midIndex) {
        position = "centerRight";
    }

    QVector<__PagiNation_DATA> leftArr(midIndex), rightArr(midIndex);
    for (int i = 0; i < midIndex; i ++) {
        // 给左边按钮集合leftArr赋予真正的输出值
        if (position == "left" || position == "centerLeft") {
            __PagiNation_DATA data = {1, i + 1, _pageNow == i + 1};
            leftArr[i] = data;
        } else {
            if (i < signLocation) {
                __PagiNation_DATA data = {1, i + 1, false};
                leftArr[i] = data;
            } else {
                __PagiNation_DATA data = {1, midLabel - (midIndex - i), false};
                leftArr[i] = data;
            }
        }

        // 给右边按钮集合rightArr赋予真正的输出值
        if (position == "right" || position == "centerRight") {
            __PagiNation_DATA data = {1, totalNum - (midIndex - (i + 1)), _pageNow == totalNum - (midIndex - (i + 1))};
            rightArr[i] = data;
        } else {
            if (i > midIndex - signLocation - 1) {
                __PagiNation_DATA data = {1, totalNum - (midIndex - (i + 1)), false};
                rightArr[i] = data;
            } else {
                __PagiNation_DATA data = {1, midLabel + (i + 1), false};
                rightArr[i] = data;
            }
        }
    }

    // 给中间按钮赋予真正的输出值
    __PagiNation_DATA midBtnData = {1, midLabel, position != "left" && position != "right"};

    // 添加分隔点数据 并输出最终数据
    if (position == "left" || position == "centerLeft") {
        for (int i = 0; i < midIndex; i ++) {
            list.push_back(leftArr[i]);
        }
        list.push_back(midBtnData);
        for (int i = 0; i < midIndex; i ++) {
            if (midIndex - signLocation == i) {
                __PagiNation_DATA data = {2, 0, false};
                list.push_back(data);
            }
            list.push_back(rightArr[i]);
        }
    } else if (position == "right" || position == "centerRight") {
        for (int i = 0; i < midIndex; i ++) {
            if (signLocation == i) {
                __PagiNation_DATA data = {2, 0, false};
                list.push_back(data);
            }
            list.push_back(leftArr[i]);
        }
        list.push_back(midBtnData);
        for (int i = 0; i < midIndex; i ++) {
            list.push_back(rightArr[i]);
        }
    } else {
        for (int i = 0; i < midIndex; i ++) {
            if (signLocation == i) {
                __PagiNation_DATA data = {2, 0, false};
                list.push_back(data);
            }
            list.push_back(leftArr[i]);
        }
        list.push_back(midBtnData);
        for (int i = 0; i < midIndex; i ++) {
            if (midIndex - signLocation == i) {
                __PagiNation_DATA data = {2, 0, false};
                list.push_back(data);
            }
            list.push_back(rightArr[i]);
        }
    }

    renderBtn(list);
}

/** 渲染按钮列表 */
void PagiNation::renderBtn (QList<__PagiNation_DATA> list) {
    qDeleteAll(mainBox->findChildren<Button*>());
    qDeleteAll(mainBox->findChildren<QLabel*>());
    for (QList<__PagiNation_DATA>::iterator p = list.begin(); p != list.end(); p ++) {
        __PagiNation_DATA data = *p;
        QSizePolicy CL(QSizePolicy::Maximum, QSizePolicy::Expanding); // 布局策略
        if (data.type == 1) {
            Button *btn = new Button(QString::number(data.labelNum), mainBox);
            btn->setFocusPolicy(Qt::NoFocus);
            btn->setFixedHeight(_height);
            btn->setSizePolicy(CL);
            connect(btn, SIGNAL(cClick(Button*)), this, SLOT(handleClick(Button*)));
            if (data.choosed) {
                btn->setCheckable(true);
                btn->setChecked(true);
            } else {
                btn->setCheckable(false);
            }
            btn->show();
            BJ_main->addWidget(btn);
        } else {
            BodyLabel *btn = new BodyLabel("...", mainBox);
            btn->setFixedHeight(_height);
            btn->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            btn->show();
            BJ_main->addWidget(btn);
        }
    }
}

/** 设置控件对齐 */
void PagiNation::setWidgetAlign () {
    BJ->removeItem(TH_left);
    BJ->removeItem(TH_right);
    if (_align == PagiNationType::Alignment::Align_Left) {
        BJ->addItem(TH_right);
    } else if (_align == PagiNationType::Alignment::Align_Right) {
        BJ->insertItem(0, TH_left);
    } else {
        BJ->insertItem(0, TH_left);
        BJ->addItem(TH_right);
    }
}

/** 点击页码按钮 槽函数 */
void PagiNation::handleClick (Button *self) {
    int pageNow = self->text().toInt();
    if (pageNow != _pageNow) {
        _pageNow = pageNow;
        computePage();
        emit pageChanged(pageNow, _pageSize);
    }
}

/** 页码后退1 */
void PagiNation::toPrev1 () {
    if (_pageNow > 1) setPage(--_pageNow, true);
}

/** 页码后退5 */
void PagiNation::toPrev5 () {
    if (_pageNow > 1) {
        _pageNow = _pageNow - 5 > 0 ? _pageNow - 5 : 1;
        setPage(_pageNow, true);
    }
}

/** 页码前进1 */
void PagiNation::toNext1 () {
    int totalNum = ceil(double(_total) / double(_pageSize));
    if (_pageNow < totalNum) setPage(++_pageNow, true);
}

/** 页码前进5 */
void PagiNation::toNext5 () {
    int totalNum = ceil(double(_total) / double(_pageSize));
    if (_pageNow < totalNum) {
        _pageNow = _pageNow + 5 < totalNum ? _pageNow + 5 : totalNum;
        setPage(_pageNow, true);
    }
}

// ----------------- 公有方法 ---------------------

/** 获取当前页码 page */
int PagiNation::page () {
    return _pageNow;
}

/** 获取当前每页条数 pagesize */
int PagiNation::pageSize () {
    return _pageSize;
}

/** 获取当前数据总条数 pagesize */
int PagiNation::total () {
    return _total;
}

/** 获取当前最大按钮数 pagesize */
int PagiNation::buttonCount () {
    return _buttonCount;
}

/** 获取当前对齐方式 pagesize */
PagiNationType::Alignment PagiNation::alignment () {
    return _align;
}

/** 设置最大按钮数量 */
void PagiNation::setButtonCount (int buttonCount, bool needEmit) {
    _buttonCount = buttonCount;
    if (_buttonCount % 2 != 1 || _buttonCount < 5) _buttonCount = 7;
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

/** 设置左右对齐方式 */
void PagiNation::setAlign (PagiNationType::Alignment align, bool needEmit) {
    if (align == _align) return;
    _align = align;
    setWidgetAlign();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

/** 设置每页条数 */
void PagiNation::setPageSize (int pageSize, bool needEmit) {
    _pageNow = 1;
    _pageSize = pageSize;
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

/** 设置总条数(用于计算总页数) */
void PagiNation::setTotal (int total, bool needEmit) {
    if (total < 0) total = 0;
    _total = total;
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

/** 设置页码 */
void PagiNation::setPage (int pageNow, bool needEmit) {
    _pageNow = pageNow;
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

/** 设置页码 */
void PagiNation::setPage (int pageNow, int total, bool needEmit) {
    _pageNow = pageNow;
    _total = total;
    computePage();
    if (needEmit) emit pageChanged(_pageNow, _pageSize);
}

/** 连接 "页码变更" 信号 */
void PagiNation::connectPageChange (function<void(int, int)> method) {
    connect(this, &PagiNation::pageChanged, method);
}

/** 连接 "页码变更" 信号 */
void PagiNation::connectPageChange (function<void(int)> method) {
    connect(this, &PagiNation::pageChanged, method);
}

/** 连接 "页码变更" 信号 */
void PagiNation::connectPageChange (function<void()> method) {
    connect(this, &PagiNation::pageChanged, method);
}









