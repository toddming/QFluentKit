// home_interface.h
#ifndef HOME_INTERFACE_H
#define HOME_INTERFACE_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

#include "QFluent/ScrollArea.h"

class LinkCardView;
class SampleCardView;

class BannerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BannerWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVBoxLayout *m_vBoxLayout;
    QLabel *m_galleryLabel;
    QPixmap m_banner;
    LinkCardView *m_linkCardView;

    void setupUI();
    void setupLinks();
};

class HomeInterface : public ScrollArea
{
    Q_OBJECT

public:
    explicit HomeInterface(QWidget *parent = nullptr);

private:
    BannerWidget *m_banner;
    QWidget *m_view;
    QVBoxLayout *m_vBoxLayout;

    void initWidget();
    void loadSamples();
};

#endif // HOME_INTERFACE_H
