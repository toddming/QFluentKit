#pragma once

#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QColor>
#include <QWidget>
#include <QFrame>


class MaskDialogBase : public QDialog
{
    Q_OBJECT

public:
    explicit MaskDialogBase(QWidget* parent = nullptr);
    ~MaskDialogBase() override = default;

    // 获取中心 widget，用于添加内容
    QWidget* centerWidget() const;

    // 设置遮罩颜色
    void setMaskColor(const QColor& color);

    // 设置阴影效果
    void setShadowEffect(int blurRadius = 60, const QPoint& offset = QPoint(0, 10), const QColor& color = QColor(0, 0, 0, 100));

    // 是否点击遮罩可关闭
    bool isClosableOnMaskClicked() const;
    void setClosableOnMaskClicked(bool isClosable);

    // 重写 showEvent 实现淡入动画
    void showEvent(QShowEvent* event) override;

    // 重写 done 实现淡出动画
    void done(int code) override;

    // 重写 resizeEvent 同步遮罩大小
    void resizeEvent(QResizeEvent* event) override;

    // 事件过滤器：处理父窗口 resize 和遮罩点击
    bool eventFilter(QObject* obj, QEvent* event) override;

protected:
    // 动画结束后调用，用于 clean up effect
    void _onDone(int code);

private:
    QWidget* m_windowMask = nullptr;
    QFrame* m_centerWidget = nullptr;

    bool m_isClosableOnMaskClicked = false;
};
