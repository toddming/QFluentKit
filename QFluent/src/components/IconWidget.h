#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QPainter>
#include <QPropertyAnimation>

#include "FluentIcon.h"
#include "../Property.h"

class QFLUENT_EXPORT IconWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ getIcon WRITE setIcon)

public:
    explicit IconWidget(QWidget *parent = nullptr);
    explicit IconWidget(const QIcon &icon, QWidget *parent = nullptr);
    explicit IconWidget(const QString &iconPath, QWidget *parent = nullptr);
    explicit IconWidget(Icon::IconType icon, QWidget *parent = nullptr);

    void setIcon(const QIcon &icon);
    void setIcon(const QString &iconPath);
    void setIcon(Icon::IconType icon);
    QIcon getIcon() const;

    QSize sizeHint() const override;

    bool isFluentIcon() { return m_isFluentIcon; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QIcon m_icon;
    Icon::IconType m_fluentIcon{Icon::IconType::NONE};
    bool m_isFluentIcon = false;
};

#endif // ICONWIDGET_H
