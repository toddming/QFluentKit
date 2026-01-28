#pragma once

#include <QFrame>

#include "FluentGlobal.h"
#include "PushButton.h"

using namespace std;

struct __PagiNation_DATA {
    int type; // 1: 按钮  2: 点
    int labelNum;
    bool choosed;
};

class QFLUENT_EXPORT Button: public TransparentTogglePushButton {
    Q_OBJECT
public:
    explicit Button(const QString& text, QWidget *parent = nullptr);

signals:
    void cClick(Button*);

private slots:
    void handleClick();
};

class QHBoxLayout;
class QSpacerItem;
class TransparentToolButton;
class QFLUENT_EXPORT PagiNation: public QFrame {
    Q_OBJECT
public:
    PagiNation (QWidget *parent, Fluent::Alignment align = Fluent::Alignment::Align_Right, int buttonCount = 7);
    PagiNation (QPoint point, QWidget *parent, Fluent::Alignment align = Fluent::Alignment::Align_Right, int buttonCount = 7);
    PagiNation (QSize size, QWidget *parent, Fluent::Alignment align = Fluent::Alignment::Align_Right, int buttonCount = 7);
    PagiNation (QRect rect, QWidget *parent, Fluent::Alignment align = Fluent::Alignment::Align_Right, int buttonCount = 7);

private:
    Fluent::Alignment _align;
    int _buttonCount;
    int _height;
    QHBoxLayout *BJ, *BJ_main;
    QSpacerItem *TH_left, *TH_right;
    int _pageNow = 1, _total = 0, _pageSize = 10;
    TransparentToolButton *prevBtn, *nextBtn, *prevFBtn, *nextFBtn;
    QFrame *mainBox;

signals:
    /** 页码变更信号 */
    void pageChanged (int pageNow, int pageSize);

public:
    int page ();
    int pageSize ();
    int total ();
    int buttonCount ();
    Fluent::Alignment alignment ();
    void setButtonCount (int buttonCount, bool needEmit = false);
    void setAlign (Fluent::Alignment align, bool needEmit = false);
    void setPageSize (int pageSize, bool needEmit = false);
    void setTotal (int total, bool needEmit = false);
    void setPage (int pageNow, bool needEmit = false);
    void setPage (int pageNow, int total, bool needEmit = false);

    void connectPageChange (function<void(int, int)> method);
    void connectPageChange (function<void(int)> method);
    void connectPageChange (function<void()> method);

private:
    void init ();
    void computePage ();
    void renderBtn (QList<__PagiNation_DATA> list, bool allDisabled = false); // 增加禁用参数
    void setWidgetAlign ();

private slots:
    void handleClick (Button *self);
    void toPrev1 ();
    void toPrev5 ();
    void toNext1 ();
    void toNext5 ();
};
