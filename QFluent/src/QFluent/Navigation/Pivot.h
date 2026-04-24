#pragma once

#include <QMap>
#include <QWidget>

#include "FluentIcon.h"
#include "FluentGlobal.h"
#include "QFluent/PushButton.h"

class QHBoxLayout;
class FluentAnimation;
class PivotItem : public PushButton {
    Q_OBJECT
    Q_PROPERTY(bool isSelected READ isSelected WRITE setSelected NOTIFY isSelectedChanged)

public:
    explicit PivotItem(const QString &text = QString(), QWidget *parent = nullptr);
    bool isSelected() const;
    void setSelected(bool isSelected);

signals:
    void itemClicked(bool);
    void isSelectedChanged(bool);  // 信号定义移到 Q_PROPERTY 之前

protected:
    void drawIcon(QPainter* painter, const QRectF& rect) override;

private:
    bool m_isSelected = false;
};

class QFLUENT_EXPORT Pivot : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString currentRouteKey READ currentRouteKey NOTIFY currentRouteKeyChanged)

public:
    explicit Pivot(QWidget *parent = nullptr);
    ~Pivot() override;

    void addItem(const QString &routeKey, const QString &text, const QIcon &icon = {});
    void addItem(const QString &routeKey, const QString &text, Fluent::IconType type);
    void addWidget(const QString &routeKey, PivotItem *widget);
    void insertItem(int index, const QString &routeKey, const QString &text, const QIcon &icon = {});
    void insertItem(int index, const QString &routeKey, const QString &text, Fluent::IconType type);
    void insertWidget(int index, const QString &routeKey, PivotItem *widget);
    void removeWidget(const QString &routeKey);
    void clear();
    PivotItem *currentItem() const;
    QString currentRouteKey() const;
    void setCurrentItem(const QString &routeKey);
    void setItemFontSize(int size);
    void setItemText(const QString &routeKey, const QString &text);
    void setIndicatorColor(const QColor &light, const QColor &dark);

signals:
    void currentItemChanged(const QString &routeKey);
    void currentRouteKeyChanged(const QString &routeKey);

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onItemClicked();

private:
    void adjustIndicatorPos();
    PivotItem *widget(const QString &routeKey) const;

    QMap<QString, PivotItem *> m_items;
    QString m_currentRouteKey;
    QColor m_lightIndicatorColor;
    QColor m_darkIndicatorColor;
    QHBoxLayout *m_hBoxLayout;
    FluentAnimation *m_slideAni;
};

