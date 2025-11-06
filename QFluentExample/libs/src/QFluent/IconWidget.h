#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <QWidget>

#include "Define.h"
#include "FluentIcon.h"

class QIcon;
class IconWidgetPrivate;
class QFLUENT_EXPORT IconWidget : public QWidget
{
    Q_OBJECT
    Q_Q_CREATE(IconWidget)
    Q_PROPERTY_CREATE_Q_H(ThemeType::ThemeMode, IconTheme)

public:
    explicit IconWidget(QWidget *parent = nullptr);
    explicit IconWidget(const QIcon &icon, QWidget *parent = nullptr);
    explicit IconWidget(const QString &iconPath, QWidget *parent = nullptr);
    explicit IconWidget(const FluentIconBase &icon, QWidget* parent = nullptr);
    ~IconWidget();

    void setIcon(const QIcon &icon);
    void setIcon(const QString &iconPath);
    void setFluentIcon(const FluentIconBase &icon);
    QIcon getIcon() const;

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

};

#endif // ICONWIDGET_H
