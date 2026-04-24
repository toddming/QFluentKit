#pragma once

#include <QWidget>
#include <QIcon>

#include "FluentGlobal.h"
#include "FluentIcon.h"

class QPainter;
class QRectF;

class QFLUENT_EXPORT IconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IconWidget(QWidget *parent = nullptr);
    explicit IconWidget(const QIcon &icon, QWidget *parent = nullptr);
    explicit IconWidget(Fluent::IconType type, QWidget *parent = nullptr);

    void setIcon(const QIcon &icon);
    void setIcon(Fluent::IconType type);
    QIcon icon() const;

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    virtual void drawIcon(QPainter *painter, const QRectF &rect);

private:
    QIcon m_icon;
};
