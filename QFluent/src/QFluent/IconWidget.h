#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <QWidget>

#include "FluentGlobal.h"
#include "FluentIcon.h"

class QIcon;
class IconWidgetPrivate;
class QFLUENT_EXPORT IconWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(d_ptr, IconWidget)

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

    void setIconTheme(Fluent::ThemeMode theme);
    Fluent::ThemeMode getIconTheme();

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QScopedPointer<IconWidgetPrivate> d_ptr;

};

#endif // ICONWIDGET_H
