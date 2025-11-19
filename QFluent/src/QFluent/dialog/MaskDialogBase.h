#pragma once

#include <QDialog>

#include "FluentGlobal.h"

class QEvent;
class QColor;
class QWidget;
class QResizeEvent;
class QHBoxLayout;
class MaskDialogBasePrivate;
class QFLUENT_EXPORT MaskDialogBase : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, MaskDialogBase)

public:
    explicit MaskDialogBase(QWidget* parent = nullptr);
    ~MaskDialogBase();

    QHBoxLayout* hBoxLayout();
    // 获取中心 widget，用于添加内容
    QWidget* centerWidget() const;

    // 设置遮罩颜色
    void setMaskColor(const QColor& color);

    // 设置阴影效果
    void setShadowEffect(int blurRadius = 60, const QPoint& offset = QPoint(0, 10), const QColor& color = QColor(0, 0, 0, 100));

    // 重写 showEvent 实现淡入动画
    void showEvent(QShowEvent* event) override;

    // 重写 done 实现淡出动画
    void done(int code) override;

    // 重写 resizeEvent 同步遮罩大小
    void resizeEvent(QResizeEvent* event) override;

    void setIsClosableOnMaskClicked(bool enable);

    bool getIsClosableOnMaskClicked();

protected:
    explicit MaskDialogBase(MaskDialogBasePrivate& dd, QWidget* parent = nullptr);
    void onDone(int code);
    // 事件过滤器：处理父窗口 resize 和遮罩点击
    bool eventFilter(QObject* obj, QEvent* event) override;

    QScopedPointer<MaskDialogBasePrivate> d_ptr;

};
