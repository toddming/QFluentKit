#ifndef SAMPLE_CARD_H
#define SAMPLE_CARD_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>

#include "QFluent/CardWidget.h"
#include "QFluent/layout/FlowLayout.h"

class IconWidget;
class FlowLayout;
class CardWidget;

class SampleCard : public CardWidget
{
    Q_OBJECT

public:
    SampleCard(const QIcon &icon, const QString &title, const QString &content, 
               const QString &routeKey, int index, QWidget *parent = nullptr);

signals:
    void clicked(const QString &routeKey, int index);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int m_index;
    QString m_routeKey;
    IconWidget *m_iconWidget;
    QLabel *m_titleLabel;
    QLabel *m_contentLabel;

    void initWidget();
};

class SampleCardView : public QWidget
{
    Q_OBJECT

public:
    explicit SampleCardView(const QString &title, QWidget *parent = nullptr);

    void addSampleCard(const QIcon &icon, const QString &title, const QString &content, 
                       const QString &routeKey, int index);
    void addSampleCard(const QString &iconPath, const QString &title, const QString &content, 
                       const QString &routeKey, int index);

private:
    QLabel *m_titleLabel;
    QVBoxLayout *m_vBoxLayout;
    FlowLayout *m_flowLayout;

    void initWidget();
    void createFlowLayout();
};

#endif // SAMPLE_CARD_H
