#ifndef GALLERY_INTERFACE_H
#define GALLERY_INTERFACE_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QUrl>
#include <QEvent>
#include <QDesktopServices>
#include <QScrollArea>

#include "QFluent/ScrollArea.h"
#include "QFluent/PushButton.h"
#include "QFluent/ToolButton.h"
#include "QFluent/IconWidget.h"
#include "QFluent/Label.h"


class SeparatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SeparatorWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;
};

class ToolBar : public QWidget
{
    Q_OBJECT

public:
    ToolBar(const QString &title, const QString &subtitle, QWidget *parent = nullptr);

private slots:
    void initWidget();

private:
    TitleLabel *titleLabel;
    CaptionLabel *subtitleLabel;
    PushButton *documentButton;
    PushButton *sourceButton;
    ToolButton *themeButton;
    SeparatorWidget *separator;
    ToolButton *supportButton;
    ToolButton *feedbackButton;
    QVBoxLayout *vBoxLayout;
    QHBoxLayout *buttonLayout;
};

class ExampleCard : public QWidget
{
    Q_OBJECT

public:
    ExampleCard(const QString &title, QWidget *widget, const QString &sourcePath,
                int stretch = 0, QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;

private:
    void initWidget();
    void initLayout();

    QWidget *widget;
    int stretch;
    StrongBodyLabel *titleLabel;
    QFrame *card;
    QFrame *sourceWidget;
    QString sourcePath;
    BodyLabel *sourcePathLabel;
    IconWidget *linkIcon;
    QVBoxLayout *vBoxLayout;
    QVBoxLayout *cardLayout;
    QHBoxLayout *topLayout;
    QHBoxLayout *bottomLayout;
};

class GalleryInterface : public ScrollArea
{
    Q_OBJECT

public:
    GalleryInterface(const QString &title, const QString &subtitle, QWidget *parent = nullptr);

    ExampleCard* addExampleCard(const QString &title, QWidget *widget, const QString &sourcePath = "", int stretch = 0);
    void scrollToCard(int index);

    QLayout *layout() const;

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    QWidget *view;
    ToolBar *toolBar;
    QVBoxLayout *vBoxLayout;
};

#endif // GALLERY_INTERFACE_H
