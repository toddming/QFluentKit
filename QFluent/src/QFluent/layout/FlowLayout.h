#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QTimer>
#include <QLayout>
#include <QEasingCurve>

#include "FluentGlobal.h"

QT_BEGIN_NAMESPACE

class QPropertyAnimation;
class QParallelAnimationGroup;
class QFLUENT_EXPORT FlowLayout : public QLayout
{
    Q_OBJECT
    Q_DISABLE_COPY(FlowLayout)

public:
    explicit FlowLayout(QWidget *parent = nullptr, bool needAni = false, bool isTight = false);
    ~FlowLayout();

    void addItem(QLayoutItem *item) override;
    void insertItem(int index, QLayoutItem *item);
    void addWidget(QWidget *widget);
    void insertWidget(int index, QWidget *widget);
    void setAnimation(int duration, QEasingCurve::Type ease = QEasingCurve::Linear);

    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;
    void removeWidget(QWidget *widget);

    void removeAllWidgets();
    void takeAllWidgets();

    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;

    QSize sizeHint() const override;
    QSize minimumSize() const override;

    void setVerticalSpacing(int spacing);
    int verticalSpacing() const;
    void setHorizontalSpacing(int spacing);
    int horizontalSpacing() const;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;
    void setGeometry(const QRect &rect) override;

private:
    QVector<QLayoutItem *> _items;
    QVector<QPropertyAnimation *> _animations;
    QParallelAnimationGroup *_aniGroup;
    int _verticalSpacing;
    int _horizontalSpacing;
    int _duration;
    QEasingCurve::Type _ease;
    bool _needAni;
    bool _isTight;
    QTimer *_debounceTimer;
    QWidget *_wParent;
    bool _isInstalledEventFilter;

    void _onWidgetAdded(QWidget *widget, int index = -1);
    int _calculateHeight(const QRect &rect) const;  // 新增：const 方法，仅计算高度
    int _doLayout(const QRect &rect, bool move);    // 保持非 const，用于实际布局
};

QT_END_NAMESPACE

#endif // FLOWLAYOUT_H
