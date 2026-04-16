#pragma once

#include <QFrame>

#include "FluentGlobal.h"
#include "PushButton.h"

#include <functional>

using std::function;

enum class PaginationItemType {
    Button = 1,
    Ellipsis = 2
};

struct PaginationData {
    PaginationItemType type;  // 1: 按钮  2: 点
    int labelNum;
    bool chosen;
};

class QFLUENT_EXPORT PaginationButton: public TransparentTogglePushButton {
    Q_OBJECT
public:
    explicit PaginationButton(const QString& text, QWidget *parent = nullptr);

signals:
    void cClick(PaginationButton*);

private slots:
    void handleClick();
};

class QHBoxLayout;
class TransparentToolButton;

class QFLUENT_EXPORT PagiNation: public QFrame {
    Q_OBJECT
public:
    PagiNation (QWidget *parent, Fluent::Alignment align = Fluent::Alignment::Align_Right, int buttonCount = 7);
    PagiNation (QPoint point, QWidget *parent, Fluent::Alignment align = Fluent::Alignment::Align_Right, int buttonCount = 7);
    PagiNation (QSize size, QWidget *parent, Fluent::Alignment align = Fluent::Alignment::Align_Right, int buttonCount = 7);
    PagiNation (QRect rect, QWidget *parent, Fluent::Alignment align = Fluent::Alignment::Align_Right, int buttonCount = 7);
    // ✅ 删除析构函数声明 - QLayout会自动管理spacer

private:
    static constexpr int DEFAULT_PAGE_SIZE = 10;
    static constexpr int DEFAULT_HEIGHT = 30;
    static constexpr int MIN_BUTTON_COUNT = 5;
    static constexpr int DEFAULT_BUTTON_COUNT = 7;

    Fluent::Alignment m_align;
    int m_buttonCount;
    int m_height;
    QHBoxLayout *m_BJ, *m_BJMain;
    // ✅ 删除spacer指针 - 让QLayout管理它们
    int m_pageNow = 1, m_total = 0, m_pageSize = DEFAULT_PAGE_SIZE;
    TransparentToolButton *m_prevBtn, *m_nextBtn, *m_prevFBtn, *m_nextFBtn;
    QFrame *m_mainBox;

signals:
    /** 页码变更信号 */
    void pageChanged (int pageNow, int pageSize);

public:
    int page () const;
    int pageSize () const;
    int total () const;
    int buttonCount () const;
    Fluent::Alignment alignment () const;
    void setPaginationButtonCount (int buttonCount, bool needEmit = false);
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
    void renderBtn (QList<PaginationData> list, bool allDisabled = false);
    void setWidgetAlign ();
    void validatePaginationButtonCount();

private slots:
    void handleClick (PaginationButton *self);
    void toPrev1 ();
    void toPrev5 ();
    void toNext1 ();
    void toNext5 ();
};
