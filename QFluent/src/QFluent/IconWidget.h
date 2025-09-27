#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QPainter>
#include <QPropertyAnimation>

#include "Icon.h"
#include "Property.h"

class QFLUENT_EXPORT IconWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ getIcon WRITE setIcon)

public:
    explicit IconWidget(QWidget *parent = nullptr);
    explicit IconWidget(const QIcon &icon, QWidget *parent = nullptr);
    explicit IconWidget(const QString &iconPath, QWidget *parent = nullptr);
    explicit IconWidget(IconType::FLuentIcon icon, QWidget *parent = nullptr);

    void setIcon(const QIcon &icon);
    void setIcon(const QString &iconPath);
    void setIcon(IconType::FLuentIcon icon);
    QIcon getIcon() const;

    QSize sizeHint() const override;

    bool isFluentIcon() { return m_isFluentIcon; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QIcon m_icon;
    IconType::FLuentIcon m_fluentIcon{IconType::FLuentIcon::NONE};
    bool m_isFluentIcon = false;
};

#endif // ICONWIDGET_H
