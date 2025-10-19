#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <QWidget>

#include "Define.h"

class QIcon;
class IconWidgetPrivate;
class QFLUENT_EXPORT IconWidget : public QWidget
{
    Q_OBJECT
    Q_Q_CREATE(IconWidget)

public:
    explicit IconWidget(QWidget *parent = nullptr);
    explicit IconWidget(const QIcon &icon, QWidget *parent = nullptr);
    explicit IconWidget(const QString &iconPath, QWidget *parent = nullptr);
    explicit IconWidget(IconType::FLuentIcon icon, QWidget *parent = nullptr);
    ~IconWidget();

    void setIcon(const QIcon &icon);
    void setIcon(const QString &iconPath);
    void setIcon(IconType::FLuentIcon icon);
    QIcon getIcon() const;

    QSize sizeHint() const override;

    bool isFluentIcon();

protected:
    void paintEvent(QPaintEvent *event) override;

};

#endif // ICONWIDGET_H
