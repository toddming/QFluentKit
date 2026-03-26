#include "Loading.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "Progress/IndeterminateProgressRing.h"


Loading::Loading(const QString &content, QWidget *parent)
    : MaskDialogBase(parent)
{
    setMaskColor(QColor(0, 0, 0, 153));

    QWidget *w = centerWidget();

    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setSpacing(12);

    m_progressRing = new IndeterminateProgressRing(w);
    m_progressRing->setFixedSize(45, 45);
    m_progressRing->setStrokeWidth(4);
    m_progressRing->setCustomBarColor(Qt::white, Qt::white);
    lay->addWidget(m_progressRing, 0, Qt::AlignHCenter);

    auto contentLabel = new QLabel(w);
    contentLabel->setStyleSheet("QLabel{color: white; font-weight: bold; font-size: 16px;}");
    contentLabel->setAlignment(Qt::AlignHCenter);
    contentLabel->setText(content.isEmpty() ? "loading..." : content);
    lay->addWidget(contentLabel, 0, Qt::AlignHCenter);

    setIsClosableOnMaskClicked(true);

    setShadowEffect(60, QPoint(0, 10), QColor(0, 0, 0, 50));
}

void Loading::showEvent(QShowEvent *event)
{
    if (m_progressRing) {
        m_progressRing->start();
    }
    MaskDialogBase::showEvent(event);
}

void Loading::hideEvent(QHideEvent *event)
{
    if (m_progressRing) {
        m_progressRing->stop();
    }
    MaskDialogBase::hideEvent(event);
}
